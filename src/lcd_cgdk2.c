
#include "tl_common.h"
#if BOARD == BOARD_CGDK2
#include "chip_8258/timer.h"
#include "i2c_drv.h"
#include "lcd.h"
#include "device.h"

#define _SCR_CODE_SEC_

#define CGDK2_I2C_ADDR		0x3E // BU9792AFUV

#define lcd_send_i2c_byte(a)  send_i2c_byte(scr.i2c_address, a)
#define lcd_send_i2c_buf(b, a)  send_i2c_bytes(scr.i2c_address, (u8 *) b, a)

RAM scr_data_t scr;

#define DEF_CGDK22_SUMBOL_SIGMENTS	13
/*
Now define how each digit maps to the segments:
  12-----1-----2
   |           |
  11           3
   |           |
  10----13-----4
   |           |
   9           5
   |           |
   8-----7-----6
*/

const u8 digits[22][DEF_CGDK22_SUMBOL_SIGMENTS + 1] = {
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0, 0},  // 0
    {2, 3, 4, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0},        // 1
    {1, 2, 3, 4, 6, 7, 8, 9, 10, 12, 13, 0, 0, 0},  // 2
    {1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 13, 0, 0, 0},  // 3
    {2, 3, 4, 5, 6, 10, 11, 12, 13, 0, 0, 0, 0, 0}, // 4
    {1, 2, 4, 5, 6, 7, 8, 10, 11, 12, 13, 0, 0, 0}, // 5
    {1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0, 0}, // 6
    {1, 2, 3, 4, 5, 6, 12, 0, 0, 0, 0, 0, 0, 0},    // 7
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0}, // 8
    {1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 0, 0}, // 9
    {1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 0, 0}, // A
    {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0, 0, 0, 0}, // b
    {1, 2, 6, 7, 8, 9, 10, 11, 12, 0, 0, 0, 0, 0},  // C
    {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0, 0, 0},  // d
    {1, 2, 4, 6, 7, 8, 9, 10, 11, 12, 0, 0, 0, 0},  // E
    {1, 2, 8, 9, 10, 11, 12, 13, 0, 0, 0, 0, 0, 0},  // F
    {2, 6, 7, 8, 9, 10, 11, 12, 0, 0, 0, 0, 0, 0},  // L  16
    {4, 5, 6, 7, 8, 9, 10, 13, 0, 0, 0, 0, 0, 0},   // o  17
    {2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 0, 0, 0}, // H  18
	{9, 9, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // i  19
    {4, 6, 7, 8, 9, 10, 11, 12, 13, 0, 0, 0, 0, 0}, // t  20
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 13, 0, 0, 0}    // a  21
};

#define Symbol_E 14
#define Symbol_L 16
#define Symbol_o 17
#define Symbol_H 18
#define Symbol_i 19
#define Symbol_t 20
#define Symbol_a 21

//----------------------------------
// define segments
// the data in the arrays consists of {byte, bit} pairs of each segment
//----------------------------------
const u8 top_left[DEF_CGDK22_SUMBOL_SIGMENTS*2] = {2, 3, 5, 7, 3, 7, 3, 6, 3, 5, 3, 4, 2, 1, 2, 0, 2, 4, 2, 5, 2, 6, 2, 7, 2, 2};
const u8 top_middle[DEF_CGDK22_SUMBOL_SIGMENTS*2] = {4, 7, 5, 6, 4, 3, 4, 2, 4, 1, 4, 0, 4, 5, 4, 4, 3, 0, 3, 1, 3, 2, 3, 3, 4, 6};
const u8 top_right[DEF_CGDK22_SUMBOL_SIGMENTS*2] = {0, 6, 17, 3, 17, 2, 17, 1, 17, 0, 5, 5, 0, 4, 0, 0, 0, 1, 0, 2, 0, 3, 0, 7, 0, 5};
const u8 bottom_left[DEF_CGDK22_SUMBOL_SIGMENTS*2] = {6, 7, 8, 3, 6, 3, 6, 2, 6, 1, 6, 0, 6, 5, 6, 4, 5, 0, 5, 1, 5, 2, 5, 3, 6, 6};
const u8 bottom_middle[DEF_CGDK22_SUMBOL_SIGMENTS*2] = {7, 3, 8, 2, 8, 7, 8, 6, 8, 5, 8, 4, 7, 1, 7, 0, 7, 4, 7, 5, 7, 6, 7, 7, 7, 2};
const u8 bottom_right[DEF_CGDK22_SUMBOL_SIGMENTS*2] = {9, 3, 17, 7, 10, 7, 10, 6, 10, 5, 10, 4, 9, 1, 9, 0, 9, 4, 9, 5, 9, 6, 9, 7, 9, 2};

/* LCD controller initialize:
1. 0xea - Set IC Operation(ICSET): Software Reset, Internal oscillator circuit
2. 0xbe - Display control (DISCTL): Power save mode3, FRAME flip 1, Normal mode // (0xb6) Power save mode2
3. 0xf0 - Blink control (BLKCTL): Off
4. 0xfc - All pixel control (APCTL): Normal
*/
//const u8 lcd_init_cmd[] = {0xea,0xbe,0xf0,0xfc}; // sleep all 16.6 uA
/* LCD controller initialize
1. 0xea - Set IC Operation(ICSET): Software Reset, Internal oscillator circuit
2. 0xf0 - Blink control (BLKCTL): Off
4. 0xc0 - Mode Set (MODE SET): Display ON ?
2. 0xbc - Display control (DISCTL): Power save mode 3, FRAME flip, Power save mode 1
*/
const u8 lcd_init_cmd[] = {0xea,0xf0, 0xc0, 0xbc}; // sleep all 9.4 uA

/*
static void lcd_send_i2c_buf(u8 * dataBuf, uint32_t dataLen) {
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	u8 * p = dataBuf;
	reg_i2c_id = scr.i2c_address;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	while (dataLen--) {
		reg_i2c_do = *p++;
		reg_i2c_ctrl = FLD_I2C_CMD_DO;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	}
    reg_i2c_ctrl = FLD_I2C_CMD_STOP;
    while (reg_i2c_status & FLD_I2C_CMD_BUSY);
}
*/

_SCR_CODE_SEC_
static void send_to_lcd(void){
	unsigned int buff_index;
	u8 * p = scr.display_cmp_buff;
	if (scr.i2c_address) {
		unsigned char r = irq_disable();
		if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
		else {
			gpio_setup_up_down_resistor(I2C_SCL, PM_PIN_PULLUP_10K);
			gpio_setup_up_down_resistor(I2C_SDA, PM_PIN_PULLUP_10K);
		}
		reg_i2c_id = scr.i2c_address;
		reg_i2c_adr_dat = 0xE800; // 0xe8 - Set IC Operarion(ICSET): Do not execute Software Reset, Internal oscillator circuit; 0x00 - ADSET
		reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);

		for(buff_index = 0; buff_index < sizeof(scr.display_buff) - 1; buff_index++) {
			reg_i2c_do = *p++;
			reg_i2c_ctrl = FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		}
		reg_i2c_do = *p;
		reg_i2c_ctrl = FLD_I2C_CMD_DO | FLD_I2C_CMD_STOP;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);

		reg_i2c_adr = 0xC8; // 0xc8 - Mode Set (MODE SET): Display ON, 1/3 Bias
		if(scr.blink_flg) {
			reg_i2c_do = scr.blink_flg; // 0xf0 - blink off, 0xc8 - Mode Set (MODE SET): Display ON, 1/3 Bias
			reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_STOP;
			if(scr.blink_flg > 0xf0)
				scr.blink_flg = 0xf0;
			else
				scr.blink_flg = 0;
		} else {
			reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR  | FLD_I2C_CMD_STOP;
		}
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		irq_restore(r);
	}
}

_SCR_CODE_SEC_
void update_lcd(void){
	if(scr.display_off)
		return;
	if (memcmp(scr.display_cmp_buff, scr.display_buff, sizeof(scr.display_buff))) {
		memcpy(scr.display_cmp_buff, scr.display_buff, sizeof(scr.display_buff));
		send_to_lcd();
	}
}

_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
static void cgdk22_set_digit(u8 *buf, u8 digit, const u8 *segments) {
    // set the segments, there are up to 11 segments in a digit
    int segment_byte;
    int segment_bit;
    for (int i = 0; i < DEF_CGDK22_SUMBOL_SIGMENTS; i++) {
        // get the segment needed to display the digit 'digit',
        // this is stored in the array 'digits'
        int segment = digits[digit][i] - 1;
        // segment = -1 indicates that there are no more segments to display
        if (segment >= 0) {
            segment_byte = segments[2 * segment];
            segment_bit = segments[1 + 2 * segment];
            buf[segment_byte] |= (1 << segment_bit);
        }
        else
            // there are no more segments to be displayed
            break;
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
		scr.display_buff[17] |= BIT(6);
	else
		scr.display_buff[17] &= ~(BIT(6)); // "°Г"
	if (symbol & 0x40)
		scr.display_buff[17] |= BIT(5); //"-"
	else
		scr.display_buff[17] &= ~BIT(5); //"-"
	if (symbol & 0x80)
		scr.display_buff[17] |= BIT(4); // "_"
	else
		scr.display_buff[17] &= ~BIT(4); // "_"
}

_SCR_CODE_SEC_
void show_battery_symbol(bool state, u8 battery_level){
	scr.display_buff[1] &= ~(BIT(1) | BIT(2) | BIT(3) | BIT(5) | BIT(6) | BIT(7));
	if (state) {
		scr.display_buff[1] |= BIT(5);
#if ZIGBEE_BATT_LEVEL
		if (battery_level >= 32) {
			scr.display_buff[1] |= BIT(1);
			if (battery_level >= 66) {
				scr.display_buff[1] |= BIT(2);
				if (battery_level >= 98) {
					scr.display_buff[1] |= BIT(3);
					if (battery_level >= 134) {
						scr.display_buff[1] |= BIT(7);
						if (battery_level >= 166) {
							scr.display_buff[1] |= BIT(6);
						}
					}
				}
			}
		}
#else
		if (battery_level >= 16) {
			scr.display_buff[1] |= BIT(1);
			if (battery_level >= 33) {
				scr.display_buff[1] |= BIT(2);
				if (battery_level >= 49) {
					scr.display_buff[1] |= BIT(3);
					if (battery_level >= 67) {
						scr.display_buff[1] |= BIT(7);
						if (battery_level >= 83) {
							scr.display_buff[1] |= BIT(6);
						}
					}
				}
			}
		}
#endif
	}
}

_SCR_CODE_SEC_
void show_ble_symbol(bool state){
	if (state)
		scr.display_buff[1] |= BIT(4);
	else
		scr.display_buff[1] &= ~BIT(4);
}

#ifdef USE_DISPLAY_CONNECT_SYMBOL
_SCR_CODE_SEC_
void show_connected_symbol(bool state){
#if USE_DISPLAY_CONNECT_SYMBOL == 2
	show_ble_symbol(!state);
#else
	show_ble_symbol(state);
#endif
}
#endif

/* number:
 * in 0.1 (-995..19995), Show: -99..-9.9 .. 199.9 .. 1999
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
void show_big_number_x10(s16 number, u8 symbol) {

	scr.display_buff[17] &= BIT(7);
	if (symbol & 0x20)
		scr.display_buff[17] |= BIT(6); // "°Г"
	if (symbol & 0x40)
		scr.display_buff[17] |= BIT(5); // "-"
	if (symbol & 0x80)
		scr.display_buff[17] |= BIT(4); // "_"

	scr.display_buff[0] = 0;
	scr.display_buff[1] &= ~(BIT(0));
	scr.display_buff[2] = 0;
	scr.display_buff[3] = 0;
	scr.display_buff[4] = 0;
	scr.display_buff[5] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	if (number > 19995) {
		// "Hi"
		scr.display_buff[2] |= BIT(0) | BIT(2) | BIT(4) | BIT(5) | BIT(6) | BIT(7);
		scr.display_buff[3] |= BIT(0) | BIT(1) | BIT(3) | BIT(4) | BIT(5) | BIT(6);
		scr.display_buff[4] |= BIT(4);
	} else if (number < -995) {
		// "Lo"
		scr.display_buff[2] |= BIT(0) | BIT(1) | BIT(4) | BIT(5) | BIT(6) | BIT(7);
		scr.display_buff[3] |= BIT(0) | BIT(1) | BIT(4);
		scr.display_buff[4] |= BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(5) | BIT(6);
	} else {
		/* number: -995..19995 */
		if (number > 1999 || number < -99) {
			/* number: -995..-100, 2000..19995 */
			// round(div 10)
			number += 5;
			number /= 10;
			// show no point: -99..-10, 200..1999
		} else {
			// show point: -9.9..199.9
			scr.display_buff[5] |= BIT(4); // point top
		}
		/* show: -99..1999 */
		if (number < 0) {
			number = -number;
			scr.display_buff[2] |= BIT(2); // "-"
		}
		/* number: -99..1999 */
		if (number > 999) scr.display_buff[1] |= BIT(0); // "1" 1000..1999
		if (number > 99) cgdk22_set_digit(scr.display_buff, number / 100 % 10, top_left);
		if (number > 9) cgdk22_set_digit(scr.display_buff, number / 10 % 10, top_middle);
		else cgdk22_set_digit(scr.display_buff, 0, top_middle);
		cgdk22_set_digit(scr.display_buff, number % 10, top_right);
	}
}

/* number in 0.1 (-99..999) -> show:  -9.9 .. 99.9 */
_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
void show_small_number_x10(s16 number, bool percent){
	scr.display_buff[5] &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3));
	scr.display_buff[6] = 0;
	scr.display_buff[7] = 0;
	scr.display_buff[8] = 0;
	scr.display_buff[9] = 0;
	scr.display_buff[10] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	scr.display_buff[17] &= ~(BIT(7));
	if (percent)
		scr.display_buff[8] |= BIT(1); // "%"
	if (number > 9995) {
		// "Hi"
		scr.display_buff[5] |= BIT(0) | BIT(1) | BIT(2) | BIT(3);
		scr.display_buff[6] |= BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(6);
		scr.display_buff[7] |= BIT(0) | BIT(4) | BIT(5) | BIT(7);
	} else if (number < -995) {
		//"Lo"
		scr.display_buff[5] |= BIT(0) | BIT(1) | BIT(2) | BIT(3);
		scr.display_buff[6] |= BIT(0) | BIT(4) | BIT(5);
		scr.display_buff[7] |= BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(5);
		scr.display_buff[8] |= BIT(4) | BIT(5) | BIT(6);
	} else {
		/* number: -99..999 */
		if (number > 999 || number < -99) {
			/* number: -995..-100, 1000..9995 */
			// round(div 10)
			number += 5;
			number /= 10;
			// show no point: -99..-10, 100..999
		} else {
			// show point: -9.9..99.9
			scr.display_buff[8] |= BIT(0); // point bottom
		}
		/* show: -99..999 */
		if (number < 0) {
			number = -number;
			scr.display_buff[6] |= BIT(6); // "-"
		}
		/* number: -99..999 */
		if (number > 99) cgdk22_set_digit(scr.display_buff, number / 100 % 10, bottom_left);
		if (number > 9) cgdk22_set_digit(scr.display_buff, number / 10 % 10, bottom_middle);
		else cgdk22_set_digit(scr.display_buff, 0, bottom_middle);
		cgdk22_set_digit(scr.display_buff, number % 10, bottom_right);
	}
}

void display_off(void) {
	send_i2c_byte(CGDK2_I2C_ADDR << 1, 0xd0);
	scr.display_off = 1;
}

void init_lcd(void){
	scr.display_off = g_zcl_thermostatUICfgAttrs.display_off;
	scr.i2c_address = CGDK2_I2C_ADDR << 1;
	if(scr.display_off) {
		display_off();
	} else {
		if(lcd_send_i2c_buf((u8 *) lcd_init_cmd, sizeof(lcd_init_cmd))) {
			display_off();
		} else {
			pm_wait_us(200);
			scr.blink_flg = 0;
			memset(&scr.display_cmp_buff, 0xff, sizeof(scr.display_cmp_buff));
			send_to_lcd();
		}
	}
}


#if	USE_DISPLAY_CLOCK
_SCR_CODE_SEC_
void show_clock(void) {
	u32 tmp = wrk.utc_time_sec / 60;
	u32 min = tmp % 60;
	u32 hrs = tmp / 60 % 24;
	memset(scr.display_buff, 0, sizeof(scr.display_buff));
	display_buff(scr.display_buff, min / 10 % 10, bottom_left);
	display_buff(scr.display_buff, min % 10, bottom_middle);
	display_buff(scr.display_buff, hrs / 10 % 10, top_left);
	display_buff(scr.display_buff, hrs % 10, top_middle);
}
#endif // USE_CLOCK


void show_ble_ota(void) {
	memset(scr.display_buff, 0, LCD_BUF_SIZE);
	cgdk22_set_digit(scr.display_buff, Symbol_o, top_left);
	cgdk22_set_digit(scr.display_buff, Symbol_t, top_middle);
	cgdk22_set_digit(scr.display_buff, Symbol_a, top_right);
	scr.display_buff[9] |= BIT(6); // "ble"
	scr.blink_flg = 0xf2;
	update_lcd();
}

void show_err_sensors(void) {
	memset(scr.display_buff, 0, LCD_BUF_SIZE);
	cgdk22_set_digit(scr.display_buff, Symbol_E, top_left);
	cgdk22_set_digit(scr.display_buff, Symbol_E, top_middle);
	cgdk22_set_digit(scr.display_buff, Symbol_E, bottom_left);
	cgdk22_set_digit(scr.display_buff, Symbol_E, bottom_middle);
}

void show_reset_screen(void) {
	memset(scr.display_buff, 0, LCD_BUF_SIZE);
	cgdk22_set_digit(scr.display_buff, Symbol_o, top_left);
	cgdk22_set_digit(scr.display_buff, Symbol_o, top_middle);
	cgdk22_set_digit(scr.display_buff, Symbol_o, bottom_left);
	cgdk22_set_digit(scr.display_buff, Symbol_o, bottom_middle);
	scr.blink_flg = 0xf2;
	update_lcd();
}


#endif // BOARD == BOARD_CGDK2
