#include "tl_common.h"
#if BOARD == BOARD_LKTMZL02
#include "chip_8258/timer.h"
#include "lcd.h"
#include "device.h"
#include "sensors.h"
#include "battery.h"

#define VKL060_I2C_ADDR		0x3E // VKL060

#define _SCR_CODE_SEC_

#define I2C_TCLK_US	2 // 2 us

RAM scr_data_t scr;

/* 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F*/
const u8 display_numbers[] = {
		// 76543210
		0b011111010, // 0
		0b000001010, // 1
		0b011010110, // 2
		0b010011110, // 3
		0b000101110, // 4
		0b010111100, // 5
		0b011111100, // 6
		0b000011010, // 7
		0b011111110, // 8
		0b010111110, // 9
		0b001111110, // A
		0b011101100, // b
		0b011110000, // C
		0b011001110, // d
		0b011110100, // E
		0b001110100  // F
};
                    //76543210
#define LCD_SYM_b  0b011101100 // "b"
#define LCD_SYM_H  0b001101110 // "H"
#define LCD_SYM_h  0b001101100 // "h"
#define LCD_SYM_i  0b001000000 // "i"
#define LCD_SYM_L  0b011100000 // "L"
#define LCD_SYM_o  0b011001100 // "o"
#define LCD_SYM_t  0b011100100 // "t"
#define LCD_SYM_0  0b011111010 // "0"
#define LCD_SYM_A  0b001111110 // "A"
#define LCD_SYM_a  0b011011110 // "a"
#define LCD_SYM_P  0b001110110 // "P"
#define LCD_SYM_E  0b011110100 // "E"

#define LCD_SYM_BLE	0x07	// connect
#define LCD_SYM_BAT	0xf0	// battery


void soft_i2c_start(void) {
	gpio_set_output_en(I2C_SCL_LCD, 0); // SCL set "1"
	gpio_set_output_en(I2C_SDA_LCD, 0); // SDA set "1"
	sleep_us(I2C_TCLK_US);
	gpio_set_output_en(I2C_SDA_LCD, 1); // SDA set "0"
	sleep_us(I2C_TCLK_US);
	gpio_set_output_en(I2C_SCL_LCD, 1); // SCL set "0"
	//sleep_us(10);
}

void soft_i2c_stop(void) {
	gpio_set_output_en(I2C_SDA_LCD, 1); // SDA set "0"
	sleep_us(I2C_TCLK_US);
	gpio_set_output_en(I2C_SCL_LCD, 0); // SCL set "1"
	sleep_us(I2C_TCLK_US);
	gpio_set_output_en(I2C_SDA_LCD, 0); // SDA set "1"
}

int soft_i2c_wr_byte(u8 b) {
	int ret, i = 8;
	while(i--) {
		sleep_us(I2C_TCLK_US/2);
		if(b & 0x80)
			gpio_set_output_en(I2C_SDA_LCD, 0); // SDA set "1"
		else
			gpio_set_output_en(I2C_SDA_LCD, 1); // SDA set "0"
		sleep_us(I2C_TCLK_US/2);
		gpio_set_output_en(I2C_SCL_LCD, 0); // SCL set "1"
		sleep_us(I2C_TCLK_US);
		gpio_set_output_en(I2C_SCL_LCD, 1); // SCL set "0"
		b <<= 1;
	}
	sleep_us(I2C_TCLK_US/2);
	gpio_set_output_en(I2C_SDA_LCD, 0); // SDA set "1"
	sleep_us(I2C_TCLK_US/2);
	gpio_set_output_en(I2C_SCL_LCD, 0); // SCL set "1"
	sleep_us(I2C_TCLK_US);
	ret = gpio_read(I2C_SDA_LCD);
	gpio_set_output_en(I2C_SCL_LCD, 1); // SCL set "0"
	return ret;
}

int soft_i2c_send_buf(u8 addr, u8 * pbuf, int size) {
	int ret = 0;
	soft_i2c_start();
	ret = soft_i2c_wr_byte(addr);
	if(ret == 0) {
		while(size--) {
			ret = soft_i2c_wr_byte(*pbuf);
			if(ret)
				break;
			pbuf++;
		}
	}
	soft_i2c_stop();
	return ret;
}

int soft_i2c_send_byte(u8 addr, u8 b) {
	int ret;
	soft_i2c_start();
	ret = soft_i2c_wr_byte(addr);
	if(ret == 0)
		soft_i2c_wr_byte(b);
	soft_i2c_stop();
	return ret;
}

#define lcd_send_i2c_byte(a)  soft_i2c_send_byte(scr.i2c_address, a)
#define lcd_send_i2c_buf(b, a)  soft_i2c_send_buf(scr.i2c_address, (u8 *) b, a)

const u8 lcd_init_cmd[]	=	{
		// LCD controller initialize:
		0xea, // System Set: Software Reset, Internal oscillator circuit
//		0xe8, // System Set: Internal oscillator circuit
		0xc8, // Mode Set: Display enable, 1/3 Bias
		0xbc, // Display control: 52 Hz, FRAME flip, low power mode1
//		0x80, // load data pointer
//		0xf0, // blink control off,  0xf2 - blink
//		0xfc, // All pixel control (APCTL): Normal
		0x0c, 0,0,0,0,0,0,0
};


_SCR_CODE_SEC_
void send_to_lcd(void){
	if (scr.i2c_address) {
		u8 buf[8];
		buf[0] = 0x0b;
		buf[1] = scr.display_cmp_buff[0];
		buf[2] = scr.display_cmp_buff[1];
		buf[3] = scr.display_cmp_buff[2];
		buf[4] = (scr.display_cmp_buff[4] >> 4) | (scr.display_cmp_buff[3] << 4);
		buf[5] = (scr.display_cmp_buff[5] >> 4) | (scr.display_cmp_buff[4] << 4);
		buf[6] = (scr.display_cmp_buff[6] >> 4) | (scr.display_cmp_buff[5] << 4);
		buf[7] = (scr.display_cmp_buff[6] << 4);
		lcd_send_i2c_buf(buf, sizeof(buf));
		if(scr.blink_flg) {
			lcd_send_i2c_byte(scr.blink_flg);
			if(scr.blink_flg > 0xf0)
				scr.blink_flg = 0xf0;
			else
				scr.blink_flg = 0;
		}
	}
}


/* 0x00 = "  "
 * 0x20 = "°Г"
 * 0x40 = " -"
 * 0x60 = "°F"
 * 0x80 = " _"
 * 0xA0 = "°C"
 * 0xC0 = " ="
 * 0xE0 = "°E" */
_SCR_CODE_SEC_
void show_temp_symbol(u8 symbol) {
	if (symbol & 0x20)
		scr.display_buff[3] |= BIT(1);
	else
		scr.display_buff[3] &= ~(BIT(1));
	if (symbol & 0x40)
		scr.display_buff[3] |= BIT(0); //"-"
	else
		scr.display_buff[3] &= ~BIT(0); //"-"
	if (symbol & 0x80)
		scr.display_buff[3] |= BIT(2); // "_"
	else
		scr.display_buff[3] &= ~BIT(2); // "_"
}

_SCR_CODE_SEC_
void show_ble_symbol(bool state){
	if (state)
		scr.display_buff[0] |= LCD_SYM_BLE;
	else 
		scr.display_buff[0] &= ~LCD_SYM_BLE;
}

_SCR_CODE_SEC_
void show_battery_symbol(bool state, u8 battery_level){
	scr.display_buff[0] &= 0x0f;
	if (state) {
		scr.display_buff[0] |= BIT(7);
#if ZIGBEE_BATT_LEVEL
		if (battery_level >= 50) {
			scr.display_buff[0] |= BIT(5);
			if (battery_level >= 100) {
				scr.display_buff[0] |= BIT(6);
				if (battery_level >= 160) {
					scr.display_buff[0] |= BIT(4);
				}
			}
		}
#else
		if (battery_level >= 25) {
			scr.display_buff[0] |= BIT(5);
			if (battery_level >= 50) {
				scr.display_buff[0] |= BIT(6);
				if (battery_level >= 80) {
					scr.display_buff[0] |= BIT(4);
				}
			}
		}
#endif
	}
}

/* number:
 * in 0.1 (-19995..19995), Show: -1999 .. -199.9 .. 199.9 .. 1999
 * symbol:
 * 0x00 = "  "
 * 0x20 = "°Г"
 * 0x40 = " -"
 * 0x60 = "°F"
 * 0x80 = " _"
 * 0xA0 = "°C"
 * 0xC0 = " ="
 * 0xE0 = "°E" */
_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
void show_big_number_x10(s16 number, u8 symbol){

	scr.display_buff[3] = 0;
	if (symbol & 0x20)
		scr.display_buff[3] |= BIT(1); // "°Г"
	if (symbol & 0x40)
		scr.display_buff[3] |= BIT(0); //"-"
	if (symbol & 0x80)
		scr.display_buff[3] |= BIT(2); // "_"

	scr.display_buff[4] = 0;

	if (number > 19995) {
   		scr.display_buff[1] = LCD_SYM_H; // "H"
   		scr.display_buff[2] = LCD_SYM_i; // "i"
	} else if (number < -9995) {
   		scr.display_buff[1] = LCD_SYM_L; // "L"
   		scr.display_buff[2] = LCD_SYM_o; // "o"
	} else {
		/* number: -19995..19995 */
		scr.display_buff[1] = 0;
		scr.display_buff[2] = 0;
		if (number > 1999 || number < -1999) {
			/* number: -19995..-2000, 2000..19995 */
			// round(div 10)
			number += 5;
			number /= 10;
			// show no point: -1999..-200, 200..1999
		} else {
			// show point: -199.9..199.9
			scr.display_buff[3] |= BIT(3); // point top
		}
		/* show: -1999..1999 */
		if (number < 0) {
			number = -number;
			if(number > 99)
				scr.display_buff[1] = BIT(0); // "-"
			else
				scr.display_buff[1] = BIT(2); // "-"
		}
		/* number: -99..1999 */
		if (number > 999) scr.display_buff[0] |= BIT(3); // "1" 1000..1999
		else scr.display_buff[0] &= ~(BIT(3)); // "0" -999..999
		if (number > 99) scr.display_buff[1] |= display_numbers[number / 100 % 10];
		if (number > 9) scr.display_buff[2] |= display_numbers[number / 10 % 10];
		else scr.display_buff[2] |= LCD_SYM_0; // "0"
	    scr.display_buff[4] = display_numbers[number %10];
	}
}

/* -9 .. 99 */
_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
void show_small_number(s16 number, bool percent){
//	scr.display_buff[5] = 0;
	scr.display_buff[6] = percent? BIT(0) : 0;
	if (number > 99) {
		scr.display_buff[5] |= LCD_SYM_H; // "H"
		scr.display_buff[6] |= LCD_SYM_i; // "i"
	} else if (number < -9) {
		scr.display_buff[5] |= LCD_SYM_L; // "L"
		scr.display_buff[6] |= LCD_SYM_o; // "o"
	} else {
		if (number < 0) {
			number = -number;
			scr.display_buff[5] = BIT(2); // "-"
		}
		if (number > 9) scr.display_buff[5] = display_numbers[number / 10 % 10];
		scr.display_buff[6] |= display_numbers[number %10];
	}
}

_SCR_CODE_SEC_
void update_lcd(void){
	if(!scr.display_off
	 && memcmp(scr.display_cmp_buff, scr.display_buff, sizeof(scr.display_buff))) {
		memcpy(scr.display_cmp_buff, scr.display_buff, sizeof(scr.display_buff));
		send_to_lcd();
	}
}
void display_off(void) {
	scr.display_off = 1;
	soft_i2c_send_byte(VKL060_I2C_ADDR << 1, 0xea);
//	soft_i2c_send_byte(VKL060_I2C_ADDR << 1, 0xd0);
}

void init_lcd(void){
	scr.display_off = g_zcl_thermostatUICfgAttrs.display_off;
	scr.i2c_address = VKL060_I2C_ADDR << 1;
	if(scr.display_off) {
		display_off(); // 2.0 uA
	} else {
		if(lcd_send_i2c_buf((u8 *) lcd_init_cmd, sizeof(lcd_init_cmd)))
			display_off();
		else {
			pm_wait_us(200);
			scr.blink_flg = 0;
			memset(&scr.display_buff, 0xff, sizeof(scr.display_buff));
			update_lcd();
		} // 8.6 uA
	}
}

#if	USE_DISPLAY_CLOCK
_SCR_CODE_SEC_
void show_clock(void) {
	uint32_t tmp = utc_time_sec / 60;
	uint32_t min = tmp % 60;
	uint32_t hrs = (tmp / 60) % 24;
	scr.display_buff[0] &= ~BIT(3);
	scr.display_buff[1] = display_numbers[(hrs / 10) % 10];
	scr.display_buff[2] = display_numbers[hrs % 10];
	scr.display_buff[3] = 0;
	scr.display_buff[4] = 0;
	scr.display_buff[5] = display_numbers[(min / 10) % 10];
	scr.display_buff[6] = display_numbers[min % 10];
}
#endif // USE_DISPLAY_CLOCK

void show_err_sensors(void) {
	scr.display_buff[0] &= LCD_SYM_BAT | LCD_SYM_BLE;
	scr.display_buff[1] = LCD_SYM_E; // "E"
	scr.display_buff[2] = LCD_SYM_E; // "E"
	scr.display_buff[3] = 0;
	scr.display_buff[4] = 0;
	scr.display_buff[5] = LCD_SYM_E; // "E"
	scr.display_buff[6] = LCD_SYM_E; // "E"

}

void show_reset_screen(void) {
	scr.display_buff[0] = 0;
	scr.display_buff[1] = LCD_SYM_o; // "o"
	scr.display_buff[2] = LCD_SYM_o; // "o"
	scr.display_buff[3] = 0;
	scr.display_buff[4] = 0;
	scr.display_buff[5] = LCD_SYM_o; // "o"
	scr.display_buff[6] = LCD_SYM_o; // "o"
	scr.blink_flg = 0xf2;
	update_lcd();
}

void show_ble_ota(void) {
	scr.display_buff[0] &= LCD_SYM_BAT;
	scr.display_buff[0] |= LCD_SYM_BLE;
	scr.display_buff[1] = LCD_SYM_o; // "o"
	scr.display_buff[2] = LCD_SYM_t; // "t"
	scr.display_buff[3] = 0;
	scr.display_buff[4] = LCD_SYM_a; // "a"
	scr.display_buff[5] = 0;
	scr.display_buff[6] = 0;
	scr.blink_flg = 0xf2;
	update_lcd();
}


#endif // BOARD_LKTMZL02
