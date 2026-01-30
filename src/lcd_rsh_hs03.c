#include "tl_common.h"
#if BOARD == BOARD_RSH_HS03
#include "chip_8258/timer.h"
#include "i2c_drv.h"
#include "lcd.h"
#include "device.h"

#define lcd_send_i2c_byte(a)  send_i2c_byte(scr.i2c_address, a)
#define lcd_send_i2c_buf(b, a)  send_i2c_bytes(scr.i2c_address, (u8 *) b, a)

RAM scr_data_t scr;

#define CLEAR_SEGMENTS 0xFF

/*
 *  RSH-HS03-V2.0 LCD buffer:  byte.bit
 *

 RSSI indicator     Battery level indicator

                    ---------6.1-----------
              |     |                     |
          |   |     |  |   |   |   |   |  ||
      |   |   |     | 7.7 7.6 7.5 7.4 6.0 ||
  |   |   |   |     |  |   |   |   |   |  ||
 3.7 3.3 4.3 6.3    |                     |
                    ---------6.1-----------



                --4.7--         --5.7--            --6.7--
         |    |         |     |         |        |         |
         |   3.2       4.6   4.2       5.6      5.3       6.6
         |    |         |     |         |        |         |      o 6.2
--3.6-- 7.3     --3.1--         --4.1--            --5.2--          +--- 6.2
         |    |         |     |         |        |         |     6.2|
  TEMP   |   3.0       4.5   4.0       5.5      5.1       6.5       ---- 7.1
  ICON   |    |         |     |         |        |         |     6.2|
  3.5           --4.4--         --5.4--     *      --6.4--          ---- 7.2
                                           5.0

  ---------------------------------3.4------------------------------------

               ---0.3---      ---1.3---           ---2.3---      %
  HUMID       |         |    |         |         |         |    1.7
  ICON       0.6       0.2  1.6       1.2       2.7       2.2
  0.7         |         |    |         |         |         |
               ---0.5---      ---1.5---           ---2.6---
              |         |    |         |         |         |
             0.4       0.1  1.4       1.1       2.5       2.1
              |         |    |         |   2.4   |         |
               ---0.0---      ---1.0---     *     ---2.0---

*/

#define SEGMENTS_IN_DIGIT	7
#define LCD_DIGITS_COUNT	6  // 2 rows with 3 digits each

enum {
	TOP_LEFT = 0,
	TOP_MIDDLE,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_MIDDLE,
	BOTTOM_RIGHT,
	INVALID_POS
} digit_pos;

/* Segments mapping in lcd_seg_* arrays below:
 *
 *  --3--
 * |     |
 * 4     2
 * |     |
 *  --6--
 * |     |
 * 5     1
 * |     |
 *  --0--
 */

const u8 lcd_seg_ram_offset[LCD_DIGITS_COUNT][SEGMENTS_IN_DIGIT] = {
		{4, 4, 4, 4, 3, 3, 3},
		{5, 5, 5, 5, 4, 4, 4},
		{6, 6, 6, 6, 5, 5, 5},
		{0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1},
		{2, 2, 2, 2, 2, 2, 2}
};

const u8 lcd_seg_bit[LCD_DIGITS_COUNT][SEGMENTS_IN_DIGIT] = {
		{4, 5, 6, 7, 2, 0, 1},
		{4, 5, 6, 7, 2, 0, 1},
		{4, 5, 6, 7, 3, 1, 2},
		{0, 1, 2, 3, 6, 4, 5},
		{0, 1, 2, 3, 6, 4, 5},
		{0, 1, 2, 3, 7, 5, 6}
};

enum {
	SYMBOL_A = 10,
	SYMBOL_b = 11,
	SYMBOL_C = 12,
	SYMBOL_d = 13,
	SYMBOL_E = 14,
	SYMBOL_F = 15,
	SYMBOL_o = 16,
	SYMBOL_t = 17,
	INVALID_SYM = 18
} letter_symbols;

const u8 display_numbers[INVALID_SYM][SEGMENTS_IN_DIGIT + 1] = {
		/* First number in the row indicates the number of segments used
		 * to show the symbol. Zeros at the end of rows are unused array entries.
		 */
		{6, 0, 1, 2, 3, 4, 5, 0}, // 0
		{2, 1, 2, 0, 0, 0, 0, 0}, // 1
		{5, 0, 2, 3, 5, 6, 0, 0}, // 2
		{5, 0, 1, 2, 3, 6, 0, 0}, // 3
		{4, 1, 2, 4, 6, 0, 0, 0}, // 4
		{5, 0, 1, 3, 4, 6, 0, 0}, // 5
		{6, 0, 1, 3, 4, 5, 6, 0}, // 6
		{3, 1, 2, 3, 0, 0, 0, 0}, // 7
		{7, 0, 1, 2, 3, 4, 5, 6}, // 8
		{6, 0, 1, 2, 3, 4, 6, 0}, // 9
		{6, 1, 2, 3, 4, 5, 6, 0}, // A
		{5, 0, 1, 4, 5 ,6, 0, 0}, // b
		{4, 0, 3, 4, 5, 0, 0, 0}, // C
		{5, 0, 1, 2, 5, 6, 0, 0}, // d
		{5, 0, 3, 4, 5, 6, 0, 0}, // E
		{4, 3, 4, 5, 6, 0, 0, 0}, // F
		{4, 0, 1, 5, 6, 0, 0, 0}, // o
		{4, 0, 4, 5, 6, 0, 0, 0}  // t
};

const u8 lcd_init_cmd[]	= {
		// LCD controller initialization sequence:
		0xea, // Set I2C Operation(ICSET): Software Reset, Internal oscillator circuit
		0xd8, // Mode Set (MODE SET): Display enable, 1/3 Bias, power saving
		0xbc, // Display control (DISCTL): Power save mode 3, FRAME flip, Power save mode 1
		0xf0, // blink control off,  0xf2 - blink
		0xfc, // All pixel control (APCTL): Normal
		0x00, // Set Display RAM start address
		0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00  // Write Display RAM default values
};

/* position - position on the screen: 0-2 top line (temperature), 3-5 Bottom line (humidity)
 * digit - symbol to show on the LCD (0-9, A-F, o, t) in this position
 */
_SCR_CODE_SEC_
static void bs55072_set_digit(u8 position, u8 digit) {
	if (position >= INVALID_POS)
		return;

	// Clear segments first
	for (int i = 0; i < 7; i++)
		scr.display_buff[(lcd_seg_ram_offset[position][i])] &= ~(1 << lcd_seg_bit[position][i]);

	// If digit is not valid then just clear the position on LCD
	if (digit >= INVALID_SYM)
		return;

	// Light up needed segments
	const u8 seg_number = display_numbers[digit][0];
	for (int i = 1; i <= seg_number; i++) {
		const u8 curr_seg = display_numbers[digit][i];
		scr.display_buff[(lcd_seg_ram_offset[position][curr_seg])] |= 1 << lcd_seg_bit[position][curr_seg];
	}
}

_SCR_CODE_SEC_
void send_to_lcd(void) {
	if (scr.i2c_address) {
		lcd_send_i2c_buf(scr.display_cmp_buff, sizeof(scr.display_cmp_buff));
		if (scr.blink_flg) {
			lcd_send_i2c_byte(scr.blink_flg);
			if (scr.blink_flg > 0xf0)
				scr.blink_flg = 0xf0;
			else
				scr.blink_flg = 0;
		}
	}
}

void init_lcd(void){
	scr.display_off = g_zcl_thermostatUICfgAttrs.display_off;
	scr.i2c_address = BL55072_I2C_ADDR << 1;
	if (scr.display_off) {
		display_off();
	} else {
		if (lcd_send_i2c_buf((u8 *) lcd_init_cmd, sizeof(lcd_init_cmd))) {
			display_off();
		} else {
			scr.blink_flg = 0;
			scr.display_cmp_buff[0] = 0;  // Set Display RAM address to 0
			memset(&(scr.display_cmp_buff[1]), 0xff, LCD_BUF_SIZE);
			scr.display_buff[3] |= 0x10; // Show horizontal delimiter line
		}
	}
}

void update_lcd(void){
	if (!scr.display_off &&
		memcmp(&(scr.display_cmp_buff[1]), scr.display_buff, sizeof(scr.display_buff))) {
		memcpy(&(scr.display_cmp_buff[1]), scr.display_buff, sizeof(scr.display_buff));
		send_to_lcd();
	}
}

void display_off(void) {
	send_i2c_byte(BL55072_I2C_ADDR << 1, 0xea);
	scr.display_off = 1;
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
	scr.display_buff[6] &= ~(BIT(2));
	scr.display_buff[7] &= ~(BIT(1)| BIT(2));
	if (symbol & 0x20) {
		scr.display_buff[6] |= BIT(2);
	}
	if (symbol & 0x40) {
		scr.display_buff[7] |= BIT(1);
	}
	if (symbol & 0x80) {
		scr.display_buff[7] |= BIT(2);
	}
	if (symbol == 0x60 || symbol == 0xa0) {
		scr.display_buff[3] |= BIT(5); // show temperature icon
	} else {
		scr.display_buff[3] &= ~(BIT(5)); // hide temperature icon
	}
}

_SCR_CODE_SEC_
void show_connected_symbol(bool state) {
	scr.display_buff[3] &= ~(BIT(3) | BIT(7));
	scr.display_buff[4] &= ~BIT(3);
	scr.display_buff[6] &= ~BIT(3);

	if (state) {
		s8 rssi = g_sysDiags.lastMessageRSSI;
		scr.display_buff[3] |= BIT(7);
		if (rssi >= -84) {
			scr.display_buff[3] |= BIT(3);
			if (rssi >= -74) {
				scr.display_buff[4] |= BIT(3);
				if (rssi >= -64)
					scr.display_buff[6] |= BIT(3);
			}
		}
	}
}

_SCR_CODE_SEC_
void show_ble_symbol(bool state) {
	// No BLE symbol on this LCD
}

_SCR_CODE_SEC_
void show_battery_symbol(bool state, u8 battery_level) {
	scr.display_buff[6] &= ~(BIT(0) | BIT(1));
	scr.display_buff[7] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	if (state) {
		scr.display_buff[6] |= BIT(1);
#if ZIGBEE_BATT_LEVEL
		if (battery_level >= 32) {
			scr.display_buff[7] |= BIT(7);
			if (battery_level >= 66) {
				scr.display_buff[7] |= BIT(6);
				if (battery_level >= 98) {
					scr.display_buff[7] |= BIT(5);
					if (battery_level >= 134) {
						scr.display_buff[7] |= BIT(4);
						if (battery_level >= 166) {
							scr.display_buff[6] |= BIT(0);
						}
					}
				}
			}
		}
#else
		if (battery_level >= 16) {
			scr.display_buff[7] |= BIT(7);
			if (battery_level >= 33) {
				scr.display_buff[7] |= BIT(6);
				if (battery_level >= 49) {
					scr.display_buff[7] |= BIT(5);
					if (battery_level >= 67) {
						scr.display_buff[7] |= BIT(4);
						if (battery_level >= 83) {
							scr.display_buff[6] |= BIT(0);
						}
					}
				}
			}
		}
#endif
	}
}

/* number:
 * in 0.1 (-995..19995), Show: -99 .. -9.9 .. 199.9 .. 1999
 * sybmbol:
 * 0x00 = "  "
 * 0x20 = "°Г"
 * 0x40 = " -"
 * 0x60 = "°F"
 * 0x80 = " _"
 * 0xA0 = "°C"
 * 0xC0 = " ="
 * 0xE0 = "°E" */
_SCR_CODE_SEC_
void show_big_number_x10(s16 number, u8 symbol) {
	show_temp_symbol(symbol);

	if (number > 19995) {
		// "Hi"
		bs55072_set_digit(TOP_LEFT, CLEAR_SEGMENTS);
		bs55072_set_digit(TOP_MIDDLE, CLEAR_SEGMENTS);
		bs55072_set_digit(TOP_RIGHT, CLEAR_SEGMENTS);
		scr.display_buff[3] &= ~(BIT(6)); // no "-" sign
		scr.display_buff[3] |= 0x03;
		scr.display_buff[4] |= 0x61;
		scr.display_buff[5] = 0;
	} else if (number < -995) {
		// "Lo"
		bs55072_set_digit(TOP_LEFT, CLEAR_SEGMENTS);
		bs55072_set_digit(TOP_MIDDLE, CLEAR_SEGMENTS);
		bs55072_set_digit(TOP_RIGHT, CLEAR_SEGMENTS);
		scr.display_buff[3] &= ~(BIT(6)); // no "-" sign
		scr.display_buff[3] |= 0x05;
		scr.display_buff[4] |= 0x13;
		scr.display_buff[5] |= 0x30;
	} else {
		/* number: -995..19995 */
		if (number > 1999 || number < -99) {
			/* number: -995..-100, 2000..19995 */
			scr.display_buff[5] &= ~(BIT(0)); // hide decimal point
			if (number < 0)
				number -= 5;
			else
				number += 5;
			number /= 10; // round(div 10)
		} else {
			// show: -9.9..199.9
			scr.display_buff[5] |= BIT(0); // show decimal point
		}

		if (number < 0) {
			number = -number;
			scr.display_buff[3] |= BIT(6); // show "-"
		} else {
			scr.display_buff[3] &= ~(BIT(6)); // hide "-" sign
		}

		/* number: -99..1999 */
		if (number > 999) scr.display_buff[7] |= BIT(3); // show extra "1" 1000..1999
		else scr.display_buff[7] &= ~(BIT(3)); // remove first "1"

		if (number > 99) bs55072_set_digit(TOP_LEFT, number / 100 % 10);
		else bs55072_set_digit(TOP_LEFT, CLEAR_SEGMENTS);  // Clear digit

		if (number > 9) bs55072_set_digit(TOP_MIDDLE, number / 10 % 10);
		else bs55072_set_digit(TOP_MIDDLE, 0);

		bs55072_set_digit(TOP_RIGHT, number % 10);
	}
}

/* number in 0.1 (-99..9999) -> show:  -9.9 .. 999 */
_SCR_CODE_SEC_
void show_small_number_x10(s16 number, bool percent) {
	if (number > 9995) {
		// "Hi"
		scr.display_buff[0] = 0x76;
		scr.display_buff[1] = 0x10;
		scr.display_buff[2] = 0;
	} else if (number < -99) {
		//"Lo"
		scr.display_buff[0] = 0x51;
		scr.display_buff[1] = 0x33;
		scr.display_buff[2] = 0;
	} else {
		/* number: -99..9999 */
		if (number > 999) {
			// no decimal point, show: 100..999
			scr.display_buff[2] &= ~(BIT(4));
			number = (number + 5) / 10; // round (div 10)
		} else { // show: -9.9..99.9
			scr.display_buff[2] |= BIT(4); // show decimal point on bottom row
		}

		/* number: -99..999 */
		if (number > 99) bs55072_set_digit(BOTTOM_LEFT, number / 100 % 10);
		else {
			bs55072_set_digit(BOTTOM_LEFT, CLEAR_SEGMENTS);  // Clear digit
			if (number < 0) {
				// show "-" sign instead of digit
				scr.display_buff[0] |= BIT(5);
				number = -number;
			}
		}

		if (number > 9) bs55072_set_digit(BOTTOM_MIDDLE, number / 10 % 10);
		else bs55072_set_digit(BOTTOM_MIDDLE, 0);

		bs55072_set_digit(BOTTOM_RIGHT, number % 10);
	}

	if (percent) {
		scr.display_buff[1] |= BIT(7); // show "%"
		scr.display_buff[0] |= BIT(7); // show humidity icon
	} else {
		scr.display_buff[1] &= ~(BIT(7)); // hide "%"
		scr.display_buff[0] &= ~(BIT(7)); // hide humidity icon
	}
}

void show_err_sensors(void) {
	bs55072_set_digit(TOP_LEFT, SYMBOL_E);
	bs55072_set_digit(TOP_MIDDLE, SYMBOL_E);
	bs55072_set_digit(TOP_RIGHT, CLEAR_SEGMENTS);
	bs55072_set_digit(BOTTOM_LEFT, SYMBOL_E);
	bs55072_set_digit(BOTTOM_MIDDLE, SYMBOL_E);
	scr.display_buff[2] = 0;
}

void show_ble_ota(void) {
	bs55072_set_digit(TOP_LEFT, SYMBOL_o);
	bs55072_set_digit(TOP_MIDDLE, SYMBOL_t);
	bs55072_set_digit(TOP_RIGHT, SYMBOL_A);
	show_ble_symbol(true);
	scr.display_buff[0] = 0;
	scr.display_buff[1] = 0;
	scr.display_buff[2] = 0;
	scr.blink_flg = 0xf2; // flash screen
	update_lcd();
}

void show_reset_screen(void) {
	bs55072_set_digit(TOP_LEFT, SYMBOL_o);
	bs55072_set_digit(TOP_MIDDLE, SYMBOL_o);
	bs55072_set_digit(TOP_RIGHT, CLEAR_SEGMENTS);
	bs55072_set_digit(BOTTOM_LEFT, SYMBOL_o);
	bs55072_set_digit(BOTTOM_MIDDLE, SYMBOL_o);
	scr.display_buff[2] = 0;
	scr.blink_flg = 0xf2; // flash screen
	update_lcd();
}

#if	USE_DISPLAY_CLOCK
void show_clock(void) {
	u32 tmp = wrk.utc_time_sec / 60;
	u32 min = tmp % 60;
	u32 hrs = (tmp / 60) % 24;
	bs55072_set_digit(TOP_LEFT, hrs / 10);
	bs55072_set_digit(TOP_MIDDLE, hrs % 10);
	bs55072_set_digit(TOP_RIGHT, CLEAR_SEGMENTS);
	bs55072_set_digit(BOTTOM_LEFT, min / 10);
	bs55072_set_digit(BOTTOM_MIDDLE, min % 10);
	bs55072_set_digit(BOTTOM_RIGHT, CLEAR_SEGMENTS);
}
#endif // USE_DISPLAY_CLOCK

#endif // BOARD == BOARD_RSH_HS03
