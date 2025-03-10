#include "tl_common.h"
#if (BOARD == BOARD_CGG1)
#include "chip_8258/timer.h"
#include "lcd.h"
#include "device.h"

#define DEF_EPD_SUMBOL_SIGMENTS	13
#define DEF_EPD_REFRESH_CNT		32

//----------------------------------
// define display commands
//----------------------------------
#define PANEL_SETTING 0x00
#define POWER_SETTING 0x01
#define POWER_OFF 0x02
#define POWER_OFF_SEQUENCE_SETTING 0x03
#define POWER_ON 0x04
#define DISPLAY_REFRESH 0x12
#define PARTIAL_DISPLAY_REFRESH 0x15
#define DATA_START_TRANSMISSION_1 0x18
#define DATA_START_TRANSMISSION_2 0x1c
#define LUT_FOR_VCOM 0x20
#define LUT_CMD_0x23 0x23
#define LUT_CMD_0x24 0x24
#define LUT_CMD_0x25 0x25
#define LUT_CMD_0x26 0x26
#define PLL_CONTROL 0x30

#define LOW     0
#define HIGH    1

RAM scr_data_t scr;

//----------------------------------
// LUTV, LUT_KK and LUT_KW values taken from the actual device with a
// logic analyzer
//----------------------------------
const u8 T_LUTV_init[15] = {0x47, 0x47, 0x01,  0x87, 0x87, 0x01,  0x47, 0x47, 0x01,  0x87, 0x87, 0x01,  0x81, 0x81, 0x01};
const u8 T_LUT_KK_init[15] = {0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x81, 0x81, 0x01};
const u8 T_LUT_KW_init[15] = {0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x81, 0x81, 0x01};
const u8 T_LUT_KK_update[15] = {0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x81, 0x81, 0x01};
const u8 T_LUT_KW_update[15] = {0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x81, 0x81, 0x01};

//----------------------------------
// define segments
// the data in the arrays consists of {byte, bit} pairs of each segment
//----------------------------------
const u8 top_left[DEF_EPD_SUMBOL_SIGMENTS*2] = {9, 4, 8, 5, 8, 6, 8, 7, 7, 0, 7, 1, 9, 2, 14, 4, 14, 5, 14, 6, 14, 7, 13, 0, 9, 3};
const u8 top_middle[DEF_EPD_SUMBOL_SIGMENTS*2] = {6, 3, 5, 7, 4, 0, 4, 1, 4, 6, 4, 7, 5, 4, 5, 0, 6, 6, 6, 5, 6, 4, 6, 2, 4, 5};
const u8 top_right[DEF_EPD_SUMBOL_SIGMENTS*2] = {1, 6, 1, 7, 1, 0, 2, 6, 2, 3, 2, 1, 2, 0, 3, 7, 2, 2, 2, 4, 2, 7, 1, 1, 2, 5};
const u8 bottom_left[DEF_EPD_SUMBOL_SIGMENTS*2] = {13, 1, 13, 2, 14, 2, 14, 0, 15, 0, 16, 6, 16, 5, 16, 4, 16, 7, 15, 6, 14, 1, 14, 3, 15, 7};
const u8 bottom_middle[DEF_EPD_SUMBOL_SIGMENTS*2] = {9, 6, 8, 3, 8, 2, 8, 1, 8, 4, 7, 3, 10, 5, 10, 4, 10, 6, 9, 5, 9, 0, 10, 7, 9, 7};
const u8 bottom_right[DEF_EPD_SUMBOL_SIGMENTS*2] = {5, 1, 5, 2, 7, 7, 3, 2, 3, 4, 0, 1, 0, 0, 3, 5, 3, 3, 3, 0, 7, 6, 6, 0, 3, 1};

#define delay_SPI_end_cycle() sleep_us(2)
#define delay_EPD_SCL_pulse() sleep_us(2)

/*
Now define how each digit maps to the segments:
          1
 12 :-----------: 2
    |           |
 11 |           | 3
    |     13    |
 10 :-----------: 4
    |           |
  9 |           | 5
    |     7     |
  8 :-----------: 6
*/

const u8 digits[22][DEF_EPD_SUMBOL_SIGMENTS + 1] = {
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
    {2, 3, 4, 5, 6, 7, 8, 9, 10, 13, 0, 0, 0, 0},   // d
    {1, 2, 4, 6, 7, 8, 9, 10, 11, 12, 0, 0, 0, 0},  // E
    {1, 2, 8, 9, 10, 11, 12, 13, 0, 0, 0, 0, 0, 0}, // F
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

_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
void transmit(u8 cd, u8 data_to_send) {
    gpio_write(EPD_SCL, LOW);
    // enable SPI
    gpio_write(EPD_CSB, LOW);
    delay_EPD_SCL_pulse();

    // send the first bit, this indicates if the following is a command or data
    if (cd != 0)
        gpio_write(EPD_SDA, HIGH);
    else
        gpio_write(EPD_SDA, LOW);
    delay_EPD_SCL_pulse();
    gpio_write(EPD_SCL, HIGH);
    delay_EPD_SCL_pulse();

    // send 8 bits
    for (int i = 0; i < 8; i++) {
        // start the clock cycle
        gpio_write(EPD_SCL, LOW);
        // set the MOSI according to the data
        if (data_to_send & 0x80)
            gpio_write(EPD_SDA, HIGH);
        else
            gpio_write(EPD_SDA, LOW);
        // prepare for the next bit
        data_to_send = (data_to_send << 1);
        delay_EPD_SCL_pulse();
        // the data is read at rising clock (halfway the time MOSI is set)
        gpio_write(EPD_SCL, HIGH);
        delay_EPD_SCL_pulse();
    }

    // finish by ending the clock cycle and disabling SPI
    gpio_write(EPD_SCL, LOW);
    delay_SPI_end_cycle();
    gpio_write(EPD_CSB, HIGH);
    delay_SPI_end_cycle();
}

inline void transmit_blk(u8 cd, const u8 * pdata, size_t size_data) {
	for (int i = 0; i < size_data; i++)
		transmit(cd, pdata[i]);
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
		scr.display_buff[1] |= BIT(3) | BIT(2);
	else
		scr.display_buff[1] &= ~(BIT(3) | BIT(2));
	if (symbol & 0x40)
		scr.display_buff[1] |= BIT(4); //"-"
	else
		scr.display_buff[1] &= ~BIT(4); //"-"
	if (symbol & 0x80)
		scr.display_buff[1] |= BIT(5); // "_"
	else
		scr.display_buff[1] &= ~BIT(5); // "_"
}

/* CGG1 no symbol 'smiley' !
 * =5 -> "---" happy, != 5 -> "    " sad */
_SCR_CODE_SEC_
void show_smiley(u8 state){
	if (state == SMILE_HAPPY)
		scr.display_buff[7] |= BIT(2);
	else
		scr.display_buff[7] &= ~BIT(2);
}

_SCR_CODE_SEC_
void show_battery_symbol(bool state, u8 battery_level) {
	scr.display_buff[5] &= ~(BIT(5) | BIT(6));
	scr.display_buff[6] &= ~(BIT(1) | BIT(7));
	scr.display_buff[7] &= ~(BIT(4) | BIT(5));
	if (state) {
		scr.display_buff[7] |= BIT(4);
#if ZIGBEE_BATT_LEVEL
		if (battery_level >= 32) {
			scr.display_buff[5] |= BIT(6);
			if (battery_level >= 66) {
				scr.display_buff[5] |= BIT(5);
				if (battery_level >= 98) {
					scr.display_buff[6] |= BIT(7);
					if (battery_level >= 134) {
						scr.display_buff[6] |= BIT(1);
						if (battery_level >= 166) {
							scr.display_buff[7] |= BIT(5);
						}
					}
				}
			}
		}
#else
		if (battery_level >= 16) {
			scr.display_buff[5] |= BIT(6);
			if (battery_level >= 33) {
				scr.display_buff[5] |= BIT(5);
				if (battery_level >= 49) {
					scr.display_buff[6] |= BIT(7);
					if (battery_level >= 67) {
						scr.display_buff[6] |= BIT(1);
						if (battery_level >= 83) {
							scr.display_buff[7] |= BIT(5);
						}
					}
				}
			}
		}
#endif
	}
}

_SCR_CODE_SEC_
void show_ble_symbol(bool state) {
	if (state)
		scr.display_buff[9] |= BIT(1); // "ble"
	else
		scr.display_buff[9] &= ~BIT(1);
}

_SCR_CODE_SEC_
void show_connected_symbol(bool state) {
if (state)
	scr.display_buff[7] |= BIT(2); // "---"
else
	scr.display_buff[7] &= ~BIT(2);
}

_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
static void epd_set_digit(u8 *buf, u8 digit, const u8 *segments) {
    // set the segments, there are up to 11 segments in a digit
    int segment_byte;
    int segment_bit;
    for (int i = 0; i < DEF_EPD_SUMBOL_SIGMENTS; i++) {
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
void show_big_number_x10(s16 number, u8 symbol){
	scr.display_buff[1] = 0; // &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3)  | BIT(4) | BIT(5) | BIT(6) | BIT(7));

	if (symbol & 0x20)
		scr.display_buff[1] |= BIT(3) | BIT(2); // "°Г"
	if (symbol & 0x40)
		scr.display_buff[1] |= BIT(4); //"-"
	if (symbol & 0x80)
		scr.display_buff[1] |= BIT(5); // "_"

	scr.display_buff[2] = 0;
	scr.display_buff[3] &= ~(BIT(6) | BIT(7));
	scr.display_buff[4] = 0;
	scr.display_buff[5] &= ~(BIT(0) | BIT(4) | BIT(7));
	scr.display_buff[6] &= ~(BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6));
	scr.display_buff[7] &= ~(BIT(0) | BIT(1));
	scr.display_buff[8] &= ~(BIT(5) | BIT(6) | BIT(7));
	scr.display_buff[9] &= ~(BIT(2) | BIT(3) | BIT(4));
	scr.display_buff[13] &= ~BIT(0);
	scr.display_buff[14] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	scr.display_buff[15] &= ~(BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5));

	if (number > 19995) {
		// "Hi"
		epd_set_digit(scr.display_buff, Symbol_H, top_left);
		epd_set_digit(scr.display_buff, Symbol_i, top_middle);
	} else if (number < -995) {
		// "Lo"
		epd_set_digit(scr.display_buff, Symbol_L, top_left);
		epd_set_digit(scr.display_buff, Symbol_o, top_middle);
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
			scr.display_buff[3] |= BIT(6); // point top
		}
		/* show: -99..1999 */
		if (number < 0) {
			number = -number;
			scr.display_buff[9] |= BIT(3); // "-"
		}
		/* number: -99..1999 */
		if (number > 999) scr.display_buff[15] |= BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5); // "1" 1000..1999
		if (number > 99) epd_set_digit(scr.display_buff, number / 100 % 10, top_left);
		if (number > 9) epd_set_digit(scr.display_buff, number / 10 % 10, top_middle);
		else epd_set_digit(scr.display_buff, 0, top_middle);
		epd_set_digit(scr.display_buff, number % 10, top_right);
	}
}

/* number in 0.1 (-995..9995) -> show:  -99 .. -9.9 .. 99.9 .. 999 */
_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
void show_small_number_x10(s16 number, bool percent){
	scr.display_buff[0] &= ~(BIT(0) | BIT(1) | BIT(2));
	scr.display_buff[3] &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5));
	scr.display_buff[5] &= ~(BIT(1) | BIT(2) | BIT(3));
	scr.display_buff[6] &= ~BIT(0);
	scr.display_buff[7] &= ~(BIT(3) | BIT(6) | BIT(7));
	scr.display_buff[8] &= ~(BIT(1) | BIT(2) | BIT(3) | BIT(4));
	scr.display_buff[9] &= ~(BIT(0) | BIT(5) | BIT(6) | BIT(7));
	scr.display_buff[10] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	scr.display_buff[13] &= ~(BIT(1) | BIT(2));
	scr.display_buff[14] &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3));
	scr.display_buff[15] &= ~(BIT(0) | BIT(6) | BIT(7));
	scr.display_buff[16] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	if (percent)
		scr.display_buff[5] |= BIT(3); // "%"
	if (number > 9995) {
		// "Hi"
		epd_set_digit(scr.display_buff, Symbol_H, bottom_left);
		epd_set_digit(scr.display_buff, Symbol_i, bottom_middle);
	} else if (number < -995) {
		//"Lo"
		epd_set_digit(scr.display_buff, Symbol_L, bottom_left);
		epd_set_digit(scr.display_buff, Symbol_o, bottom_middle);
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
			scr.display_buff[0] |= BIT(2); // point bottom
		}
		/* show: -99..999 */
		if (number < 0) {
			number = -number;
			scr.display_buff[15] |= BIT(7); // "-"
		}
		/* number: -99..999 */
		if (number > 99) epd_set_digit(scr.display_buff, number / 100 % 10, bottom_left);
		if (number > 9) epd_set_digit(scr.display_buff, number / 10 % 10, bottom_middle);
		else epd_set_digit(scr.display_buff, 0, bottom_middle);
		epd_set_digit(scr.display_buff, number % 10, bottom_right);
	}
}

void init_lcd(void) {
	// pulse RST_N low for 110 microseconds
    gpio_write(EPD_RST, LOW);
    gpio_is_output_en(EPD_RST);
    pm_wait_us(110);
	scr.display_off = g_zcl_thermostatUICfgAttrs.display_off;
#ifdef 	DEF_EPD_REFRESH_CNT
	scr.refresh_cnt = DEF_EPD_REFRESH_CNT;
#endif
    scr.updated = 0;
    scr.stage = 1;
    scr.init = 1;
	memset(scr.display_buff, 0, sizeof(scr.display_buff));
	memset(scr.display_cmp_buff, 0, sizeof(scr.display_cmp_buff));
    gpio_write(EPD_RST, HIGH);
    //bls_pm_setWakeupSource(PM_WAKEUP_PAD | PM_WAKEUP_TIMER);  // gpio pad wakeup suspend/deepsleep
    // EPD_BUSY: Low 866 us
#if PM_ENABLE
	cpu_set_gpio_wakeup(EPD_BUSY, Level_High, 1);
#endif
}

_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
int task_lcd(void) {
	while (gpio_read(EPD_BUSY)) {
		switch (scr.stage) {
		case 1: // Update/Init lcd, stage 1
			// send Charge Pump ON command
			transmit(0, POWER_ON);
			// EPD_BUSY: Low 32 ms from reset, 47.5 ms in refresh cycle
			scr.stage = 2;
			// pm_wait_us(200); ?
			break;
		case 2: // Update/Init lcd, stage 2
			if (scr.updated == 0) {
				transmit(0, PANEL_SETTING);
				transmit(1, 0x0F);

				transmit(0, POWER_SETTING);
				transmit(1, 0x32); // transmit(1, 0x32);
				transmit(1, 0x32); // transmit(1, 0x32);
				transmit(0, POWER_OFF_SEQUENCE_SETTING);
				transmit(1, 0x00);
				// Frame Rate Control
				transmit(0, PLL_CONTROL);
				if (scr.init)
					transmit(1, 0x03);
				else {
					transmit(1, 0x07);
					transmit(0, PARTIAL_DISPLAY_REFRESH);
					transmit(1, 0x00);
					transmit(1, 0x87);
					transmit(1, 0x01);
					transmit(0, POWER_OFF_SEQUENCE_SETTING);
					transmit(1, 0x06);
				}
				// send the e-paper voltage settings (waves)
				transmit(0, LUT_FOR_VCOM);
				transmit_blk(1, T_LUTV_init, sizeof(T_LUTV_init));
				if (scr.init) {
					scr.init = 0;
					transmit(0, LUT_CMD_0x23);
					transmit_blk(1, T_LUT_KK_init, sizeof(T_LUT_KK_init));
					transmit(0, LUT_CMD_0x26);
					transmit_blk(1, T_LUT_KW_init, sizeof(T_LUT_KW_init));
					// start an initialization sequence (white - all 0x00)
					scr.stage = 2;
				} else {
					transmit(0, LUT_CMD_0x23);
					transmit_blk(1, T_LUTV_init, sizeof(T_LUTV_init));
					transmit(0, LUT_CMD_0x24);
					transmit_blk(1, T_LUT_KK_update, sizeof(T_LUT_KK_update));
					transmit(0, LUT_CMD_0x25);
					transmit_blk(1, T_LUT_KW_update, sizeof(T_LUT_KW_update));
					transmit(0, LUT_CMD_0x26);
					transmit_blk(1, T_LUTV_init, sizeof(T_LUTV_init));
					// send the actual data
					scr.stage = 3;
				}
			} else {
				scr.stage = 3;
			}
			// send the actual data
			transmit(0, DATA_START_TRANSMISSION_1);
			for (int i = 0; i < sizeof(scr.display_cmp_buff); i++)
				transmit(1, scr.display_cmp_buff[i]^0xFF);
			// Refresh
			transmit(0, DISPLAY_REFRESH);
			// EPD_BUSY: Low 1217 ms from reset, 608.5 ms in refresh cycle
			// pm_wait_us(200); ?
			break;
		case 3: // Update lcd, stage 3
			// send Charge Pump OFF command
			transmit(0, POWER_OFF);
			transmit(1, 0x03);
			// EPD_BUSY: Low 9.82 ms in refresh cycle
			scr.updated = 1;
			scr.stage = 0;
			// pm_wait_us(200);
			break;
		default:
			if(!scr.display_off // g_zcl_thermostatUICfgAttrs.display_off
			&& memcmp(scr.display_cmp_buff, scr.display_buff, sizeof(scr.display_buff))) {
				memcpy(scr.display_cmp_buff, scr.display_buff, sizeof(scr.display_buff));
//				lcd_flg.b.send_notify = lcd_flg.b.notify_on; // set flag LCD for send notify
#ifdef DEF_EPD_REFRESH_CNT
				if (scr.refresh_cnt) {
					scr.refresh_cnt--;
					scr.init = 0;
					scr.stage = 1;
				} else {
					init_lcd(); // pulse RST_N low for 110 microseconds
					// pm_wait_us(200); ?
				}
#else
				scr.stage = 1;
#endif
			} else {
				scr.stage = 0;
			}
		}
		if(!scr.stage)
			break;
	}
#if PM_ENABLE
	cpu_set_gpio_wakeup(EPD_BUSY, Level_High, scr.stage != 0);
#endif
	return scr.stage;
}

#if	USE_DISPLAY_CLOCK
_SCR_CODE_SEC_
void show_clock(void) {
	u32 tmp = wrk.utc_time_sec / 60;
	u32 min = tmp % 60;
	u32 hrs = tmp / 60 % 24;
	memset(scr.display_buff, 0, sizeof(scr.display_buff));
	epd_set_digit(scr.display_buff, min / 10 % 10, bottom_left);
	epd_set_digit(scr.display_buff, min % 10, bottom_middle);
	epd_set_digit(scr.display_buff, hrs / 10 % 10, top_left);
	epd_set_digit(scr.display_buff, hrs % 10, top_middle);
}
#endif // USE_CLOCK

void show_ble_ota(void) {
	memset(scr.display_buff, 0, LCD_BUF_SIZE);
	epd_set_digit(scr.display_buff, Symbol_o, top_left);
	epd_set_digit(scr.display_buff, Symbol_t, top_middle);
	epd_set_digit(scr.display_buff, Symbol_a, top_right);
	scr.display_buff[9] |= BIT(1); // "ble"
}

void show_err_sensors(void) {
	memset(scr.display_buff, 0, LCD_BUF_SIZE);
	epd_set_digit(scr.display_buff, Symbol_E, top_left);
	epd_set_digit(scr.display_buff, Symbol_E, top_middle);
	epd_set_digit(scr.display_buff, Symbol_E, bottom_left);
	epd_set_digit(scr.display_buff, Symbol_E, bottom_right);
}

void show_reset_screen(void) {
	memset(scr.display_buff, 0, LCD_BUF_SIZE);
	epd_set_digit(scr.display_buff, Symbol_o, top_left);
	epd_set_digit(scr.display_buff, Symbol_o, top_middle);
	epd_set_digit(scr.display_buff, Symbol_o, bottom_left);
	epd_set_digit(scr.display_buff, Symbol_o, bottom_right);
	while(task_lcd())
		sleep_us(USE_EPD*1000);
}


#endif // (BOARD == BOARD_CGG1)
