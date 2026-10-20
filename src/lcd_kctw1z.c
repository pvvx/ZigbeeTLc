#include "tl_common.h"
#if BOARD == BOARD_KCTW1Z
#include "chip_8258/timer.h"
#include "lcd.h"
#include "app_main.h"
#include "sensor_th.h"
#include "battery.h"

#define KCTW1Z_SOFT_I2C_TCLK_US		2

// transmitted display-RAM bytes per frame (stock refresh)
#define KCTW1Z_LCD_DATA_BYTES		12

// VKL144B whole-display blink (datasheet 7.14): 1111 00 BK1 BK0
#define KCTW1Z_BLINK_OFF			0xf0
#define KCTW1Z_BLINK_1HZ			0xf2

// battery icon fill bars: 25% / 50% / 75%
#if ZIGBEE_BATT_LEVEL
#define KCTW1Z_BATT_BAR1_THRESHOLD	50   // 25% of 0..200
#define KCTW1Z_BATT_BAR2_THRESHOLD	100  // 50%
#define KCTW1Z_BATT_BAR3_THRESHOLD	150  // 75%
#else
#define KCTW1Z_BATT_BAR1_THRESHOLD	25   // 25% of 0..100
#define KCTW1Z_BATT_BAR2_THRESHOLD	50   // 50%
#define KCTW1Z_BATT_BAR3_THRESHOLD	75   // 75%
#endif

#define _SCR_CODE_SEC_

RAM scr_data_t scr;
static RAM u8 kctw1z_blink_applied;

const u8 display_numbers[] = {
		0b0111111, // 0
		0b0000110, // 1
		0b1011011, // 2
		0b1001111, // 3
		0b1100110, // 4
		0b1101101, // 5
		0b1111101, // 6
		0b0000111, // 7
		0b1111111, // 8
		0b1101111, // 9
		0b1110111, // A
		0b1111100, // b
		0b0111001, // C
		0b1011110, // d
		0b1111001, // E
		0b1110001  // F
};

// display off (datasheet 8.3: slave 0x7c + C0), keeps display RAM
static const u8 lcd_off_cmd[] = {0xc0};

static int kctw1z_soft_i2c_send_buf(GPIO_PinTypeDef scl, GPIO_PinTypeDef sda, u8 addr, const u8 *pbuf, int size);

#define lcd_soft_send(addr, buf, len)  kctw1z_soft_i2c_send_buf(I2C_SCL_LCD, I2C_SDA_LCD, (addr), (buf), (len))

#define KCTW1Z_SEG(n)	       BIT(((n) - 1) & 7)
#define KCTW1Z_SEG_BYTE(n)     (((n) - 1) >> 3)

static void kctw1z_set_step(u8 step, bool state) {
	// steps >96 fall beyond the transmitted bytes
	u8 idx = KCTW1Z_SEG_BYTE(step);

	if(idx >= LCD_BUF_SIZE) {
		return;
	}
	if(state) {
		scr.display_buff[idx] |= KCTW1Z_SEG(step);
	} else {
		scr.display_buff[idx] &= ~KCTW1Z_SEG(step);
	}
}

// 7-seg bit order matches display_numbers[]: bit0=a bit1=b bit2=c bit3=d
// bit4=e bit5=f bit6=g.
#define KCTW1Z_CHAR_H	0b1110110
#define KCTW1Z_CHAR_i	0b0000100
#define KCTW1Z_CHAR_L	0b0111000
#define KCTW1Z_CHAR_o	0b1011100

static void kctw1z_put_pattern(const u8 seg_steps[7], u8 pattern) {
	for(u8 i = 0; i < 7; i++) {
		kctw1z_set_step(seg_steps[i], pattern & BIT(i));
	}
}

static void kctw1z_put_digit(const u8 seg_steps[7], u8 digit) {
	kctw1z_put_pattern(seg_steps, display_numbers[digit & 0x0f]);
}

static void kctw1z_clear_digit(const u8 seg_steps[7]) {
	for(u8 i = 0; i < 7; i++) {
		kctw1z_set_step(seg_steps[i], false);
	}
}

static const u8 kctw1z_temp_left_digit[7] = {4, 15, 13, 1, 2, 3, 14};
static const u8 kctw1z_temp_right_digit[7] = {12, 23, 21, 9, 10, 11, 22};
static const u8 kctw1z_temp_frac_digit[7] = {48, 43, 41, 45, 46, 47, 42};
static const u8 kctw1z_humi_left_digit[7] = {20, 31, 29, 17, 18, 19, 30};
static const u8 kctw1z_humi_right_digit[7] = {28, 39, 37, 25, 26, 27, 38};

static int lcd_send_stock_frame(u8 addr, const u8 *display_buf) {
	// [0x00] + display RAM, as stock refresh
	u8 buf[1 + KCTW1Z_LCD_DATA_BYTES];

	memset(buf, 0, sizeof(buf));
	memcpy(&buf[1], display_buf, KCTW1Z_LCD_DATA_BYTES);
	return lcd_soft_send(addr, buf, sizeof(buf));
}

static int lcd_send_stock_init(u8 addr) {
	// stock init, byte for byte: TX1 = EA(reset) E8 00(RAM addr 0) + 11*FF + 00,
	// then TX2 = C8(display on, 1/3 bias) BC(53 Hz, frame inversion, LP1)
	u8 buf[15];
	int ret;

	memset(buf, 0, sizeof(buf));
	buf[0] = 0xea;
	buf[1] = 0xe8;
	memset(&buf[3], 0xff, 11);
	ret = lcd_soft_send(addr, buf, sizeof(buf));
	if(ret) {
		return ret;
	}

	buf[0] = 0xc8; // mode set: display ON, 1/3 bias (E=1, M0=0)
	buf[1] = 0xbc; // display control: 53 Hz, frame inversion, LP1
	return lcd_soft_send(addr, buf, 2);
}

static void kctw1z_soft_i2c_release(GPIO_PinTypeDef pin) {
	gpio_set_func(pin, AS_GPIO);
	gpio_write(pin, 1);
	gpio_set_output_en(pin, 0);
	gpio_set_input_en(pin, 1);
	gpio_setup_up_down_resistor(pin, PM_PIN_PULLUP_10K);
}

static void kctw1z_soft_i2c_low(GPIO_PinTypeDef pin) {
	gpio_set_func(pin, AS_GPIO);
	gpio_write(pin, 0);
	gpio_set_output_en(pin, 1);
	gpio_set_input_en(pin, 0);
	gpio_setup_up_down_resistor(pin, PM_PIN_UP_DOWN_FLOAT);
}

static void kctw1z_soft_i2c_start(GPIO_PinTypeDef scl, GPIO_PinTypeDef sda) {
	kctw1z_soft_i2c_release(scl);
	kctw1z_soft_i2c_release(sda);
	sleep_us(KCTW1Z_SOFT_I2C_TCLK_US);
	kctw1z_soft_i2c_low(sda);
	sleep_us(KCTW1Z_SOFT_I2C_TCLK_US);
	kctw1z_soft_i2c_low(scl);
}

static void kctw1z_soft_i2c_stop(GPIO_PinTypeDef scl, GPIO_PinTypeDef sda) {
	kctw1z_soft_i2c_low(sda);
	sleep_us(KCTW1Z_SOFT_I2C_TCLK_US);
	kctw1z_soft_i2c_release(scl);
	sleep_us(KCTW1Z_SOFT_I2C_TCLK_US);
	kctw1z_soft_i2c_release(sda);
}

static int kctw1z_soft_i2c_wr_byte(GPIO_PinTypeDef scl, GPIO_PinTypeDef sda, u8 b) {
	int ret;

	for(int i = 0; i < 8; i++) {
		sleep_us(KCTW1Z_SOFT_I2C_TCLK_US / 2);
		if(b & 0x80) {
			kctw1z_soft_i2c_release(sda);
		} else {
			kctw1z_soft_i2c_low(sda);
		}
		sleep_us(KCTW1Z_SOFT_I2C_TCLK_US / 2);
		kctw1z_soft_i2c_release(scl);
		sleep_us(KCTW1Z_SOFT_I2C_TCLK_US);
		kctw1z_soft_i2c_low(scl);
		b <<= 1;
	}

	sleep_us(KCTW1Z_SOFT_I2C_TCLK_US / 2);
	kctw1z_soft_i2c_release(sda);
	sleep_us(KCTW1Z_SOFT_I2C_TCLK_US / 2);
	kctw1z_soft_i2c_release(scl);
	sleep_us(KCTW1Z_SOFT_I2C_TCLK_US);
	ret = gpio_read(sda);
	kctw1z_soft_i2c_low(scl);
	return ret;
}

static int kctw1z_soft_i2c_send_buf(GPIO_PinTypeDef scl, GPIO_PinTypeDef sda, u8 addr, const u8 *pbuf, int size) {
	int ret = 0;

	kctw1z_soft_i2c_start(scl, sda);
	ret = kctw1z_soft_i2c_wr_byte(scl, sda, addr);
	if(ret == 0) {
		while(size--) {
			ret = kctw1z_soft_i2c_wr_byte(scl, sda, *pbuf++);
			if(ret) {
				break;
			}
		}
	}
	kctw1z_soft_i2c_stop(scl, sda);
	return ret;
}

static u8 lcd_pick_i2c_address(void) {
	return VKL144B_I2C_ADDR << 1;
}


_SCR_CODE_SEC_
void send_to_lcd(void){
	if(scr.i2c_address) {
		lcd_send_stock_frame(scr.i2c_address, scr.display_cmp_buff);
	}
}

_SCR_CODE_SEC_
void show_temp_symbol(u8 symbol) {
	kctw1z_set_step(44, false);
	kctw1z_set_step(63, false);
	kctw1z_set_step(64, false);

	if(symbol == TEMP_SYMBOL_C) {
		kctw1z_set_step(44, true);
		kctw1z_set_step(64, true);
	} else if(symbol == TEMP_SYMBOL_F) {
		kctw1z_set_step(63, true);
		kctw1z_set_step(64, true);
	} else if(symbol == TEMP_SYMBOL_E) {
		kctw1z_set_step(44, true);
		kctw1z_set_step(63, true);
		kctw1z_set_step(64, true);
	}
}

_SCR_CODE_SEC_
void show_ble_symbol(bool state){
	// BT icon only, Zigbee icon belongs to show_connected_symbol()
	kctw1z_set_step(53, state);
}

#ifdef USE_DISPLAY_CONNECT_SYMBOL
// 3-bar RSSI gauge, 4 dB hysteresis, max one bar change per call
static RAM u8 kctw1z_rssi_bars; // 0..3, current gauge state

_SCR_CODE_SEC_
void show_connected_symbol(bool state){
	// step 54 = Zigbee, 62 = base dot, bars: 50 low, 61 mid, 49 high
	static const s8 rssi_bar_up[3]   = {-84, -74, -64};
	static const s8 rssi_bar_down[3] = {-88, -78, -68};

	kctw1z_set_step(54, state);
	kctw1z_set_step(62, state);
	if(state) {
		s8 rssi = g_sysDiags.lastMessageRSSI;
		if(kctw1z_rssi_bars < 3 && rssi >= rssi_bar_up[kctw1z_rssi_bars]) {
			kctw1z_rssi_bars++;
		} else if(kctw1z_rssi_bars > 0 && rssi < rssi_bar_down[kctw1z_rssi_bars - 1]) {
			kctw1z_rssi_bars--;
		}
	} else {
		kctw1z_rssi_bars = 0;
	}
	kctw1z_set_step(50, kctw1z_rssi_bars >= 1);
	kctw1z_set_step(61, kctw1z_rssi_bars >= 2);
	kctw1z_set_step(49, kctw1z_rssi_bars >= 3);
}
#endif

_SCR_CODE_SEC_
void show_battery_symbol(bool state, u8 battery_level){
	kctw1z_set_step(51, false);
	kctw1z_set_step(52, false);
	kctw1z_set_step(55, false);
	kctw1z_set_step(56, false);

	if(!state) {
		return;
	}

	kctw1z_set_step(51, true);
	if(battery_level > KCTW1Z_BATT_BAR1_THRESHOLD) {
		kctw1z_set_step(52, true);
	}
	if(battery_level > KCTW1Z_BATT_BAR2_THRESHOLD) {
		kctw1z_set_step(56, true);
	}
	if(battery_level > KCTW1Z_BATT_BAR3_THRESHOLD) {
		kctw1z_set_step(55, true);
	}
}

_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
void show_big_number_x10(s16 number, u8 symbol){
	bool negative = false;
	scr.blink_flg = KCTW1Z_BLINK_OFF;

	kctw1z_clear_digit(kctw1z_temp_left_digit);
	kctw1z_clear_digit(kctw1z_temp_right_digit);
	kctw1z_clear_digit(kctw1z_temp_frac_digit);
	kctw1z_set_step(5, false);
	kctw1z_set_step(6, false);
	kctw1z_set_step(24, true);

	if(number < 0) {
		negative = true;
		number = -number;
	}

	// above 99.9: reuse the tenths digit as units, hide the decimal point
	// (cf. lcd_lktmzl02 / lcd_zth05)
	if(number > 999) {
		u16 whole = (u16)((number + 5) / 10); // round tenths to degrees

		kctw1z_set_step(24, false); // hide decimal point

		// Three 7-seg digits only reach 999; beyond that show "Hi"/"Lo".
		if(whole > 999) {
			kctw1z_put_pattern(kctw1z_temp_right_digit, negative ? KCTW1Z_CHAR_L : KCTW1Z_CHAR_H);
			kctw1z_put_pattern(kctw1z_temp_frac_digit, negative ? KCTW1Z_CHAR_o : KCTW1Z_CHAR_i);
			show_temp_symbol(symbol);
			return;
		}

		if(negative) {
			kctw1z_set_step(5, true);
		}
		kctw1z_put_digit(kctw1z_temp_left_digit, (whole / 100) % 10);
		kctw1z_put_digit(kctw1z_temp_right_digit, (whole / 10) % 10);
		kctw1z_put_digit(kctw1z_temp_frac_digit, whole % 10);
		show_temp_symbol(symbol);
		return;
	}

	if(negative) {
		kctw1z_set_step(5, true);
	}
	if(number >= 100) {
		kctw1z_put_digit(kctw1z_temp_left_digit, (number / 100) % 10);
	}
	kctw1z_put_digit(kctw1z_temp_right_digit, (number / 10) % 10);
	kctw1z_put_digit(kctw1z_temp_frac_digit, number % 10);
	show_temp_symbol(symbol);
}

_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
void show_small_number(s16 number, bool percent){
	scr.blink_flg = KCTW1Z_BLINK_OFF;
	kctw1z_clear_digit(kctw1z_humi_left_digit);
	kctw1z_clear_digit(kctw1z_humi_right_digit);
	kctw1z_set_step(33, percent);
	kctw1z_set_step(32, true);

	if(number < 0) {
		number = 0;
	}
	if(number > 99) {
		number = 99;
	}

	if(number >= 10) {
		kctw1z_put_digit(kctw1z_humi_left_digit, (number / 10) % 10);
	}
	kctw1z_put_digit(kctw1z_humi_right_digit, number % 10);
}

_SCR_CODE_SEC_
void update_lcd(void){
	if(scr.display_off) {
		return;
	}
	// compare only the transmitted bytes
	if(memcmp(scr.display_cmp_buff, scr.display_buff, KCTW1Z_LCD_DATA_BYTES)) {
		memcpy(scr.display_cmp_buff, scr.display_buff, KCTW1Z_LCD_DATA_BYTES);
		send_to_lcd();
	}
	// Blink mode is independent of display RAM; retry after a failed command.
	if(scr.i2c_address && scr.blink_flg
	 && scr.blink_flg != kctw1z_blink_applied
	 && !lcd_soft_send(scr.i2c_address, &scr.blink_flg, 1)) {
		kctw1z_blink_applied = scr.blink_flg;
	}
}

void display_off(void) {
	u8 addr = scr.i2c_address ? scr.i2c_address : lcd_pick_i2c_address();
	scr.display_off = 1;
	lcd_soft_send(addr, lcd_off_cmd, sizeof(lcd_off_cmd));
}

void init_lcd(void){
	scr.display_off = g_zcl_thermostatUICfgAttrs.display_off;
	scr.i2c_address = lcd_pick_i2c_address();
	if(scr.display_off) {
		display_off();
	} else {
		kctw1z_soft_i2c_release(I2C_SCL_LCD);
		kctw1z_soft_i2c_release(I2C_SDA_LCD);
		if(lcd_send_stock_init(scr.i2c_address)) {
			// no ACK - keep the panel off
			display_off();
		} else {
			sleep_us(200);
			scr.blink_flg = KCTW1Z_BLINK_OFF;
			kctw1z_blink_applied = KCTW1Z_BLINK_OFF;
			memset(&scr.display_buff, 0, sizeof(scr.display_buff));
			memset(&scr.display_cmp_buff, 0xff, sizeof(scr.display_cmp_buff));
			update_lcd();
		}
	}
}

// clear digits and number symbols, keep status icons (battery/BT/Zigbee/signal)
static void kctw1z_clear_number_area(void) {
	kctw1z_clear_digit(kctw1z_temp_left_digit);
	kctw1z_clear_digit(kctw1z_temp_right_digit);
	kctw1z_clear_digit(kctw1z_temp_frac_digit);
	kctw1z_clear_digit(kctw1z_humi_left_digit);
	kctw1z_clear_digit(kctw1z_humi_right_digit);
	kctw1z_set_step(5, false);   // minus
	kctw1z_set_step(6, false);   // left extra
	kctw1z_set_step(24, false);  // temp dot
	kctw1z_set_step(32, false);  // humi divider
	kctw1z_set_step(33, false);  // percent
	kctw1z_set_step(44, false);  // temp unit C
	kctw1z_set_step(63, false);  // temp unit F
	kctw1z_set_step(64, false);  // temp unit degree
}

void show_err_sensors(void) {
	scr.blink_flg = KCTW1Z_BLINK_OFF;
	kctw1z_clear_number_area();
	kctw1z_put_digit(kctw1z_temp_right_digit, 0x0e);
	kctw1z_put_digit(kctw1z_temp_frac_digit, 0x0e);
	kctw1z_put_digit(kctw1z_humi_left_digit, 0x0e);
	kctw1z_put_digit(kctw1z_humi_right_digit, 0x0e);
}

void show_reset_screen(void) {
	kctw1z_clear_number_area();
	kctw1z_put_digit(kctw1z_temp_right_digit, 0);
	kctw1z_put_digit(kctw1z_temp_frac_digit, 0);
	kctw1z_put_digit(kctw1z_humi_left_digit, 0);
	kctw1z_put_digit(kctw1z_humi_right_digit, 0);
	scr.blink_flg = KCTW1Z_BLINK_1HZ;
	update_lcd();
}

void show_ble_ota(void) {
	kctw1z_clear_number_area();
	show_ble_symbol(true);
	kctw1z_put_digit(kctw1z_temp_right_digit, 0);
	kctw1z_put_digit(kctw1z_temp_frac_digit, 0x0f);
	kctw1z_put_digit(kctw1z_humi_left_digit, 0x0a);
	scr.blink_flg = KCTW1Z_BLINK_1HZ;
	update_lcd();
}

#endif // BOARD_KCTW1Z
