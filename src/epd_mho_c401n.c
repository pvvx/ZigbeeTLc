/*
 * epd_mho_c401n.c
 * Author: kloemi & pvvx
 */
#include "tl_common.h"
#if BOARD == BOARD_MHO_C401N
#include "chip_8258/timer.h"

#include "lcd.h"
#include "device.h"

#define LOW     0
#define HIGH    1

RAM scr_data_t scr;

const u8 T_LUT_ping[5] = {0x07B, 0x081, 0x0E4, 0x0E7, 0x008};
const u8 T_LUT_init[14] = {0x082, 0x068, 0x050, 0x0E8, 0x0D0, 0x0A8, 0x065, 0x07B, 0x081, 0x0E4, 0x0E7, 0x008, 0x0AC, 0x02B };
const u8 T_LUT_work[9] = {0x082, 0x080, 0x000, 0x0C0, 0x080, 0x080, 0x062, 0x0AC, 0x02B};

//----------------------------------
// define segments
// the data in the arrays consists of {byte, bit} pairs of each segment
//----------------------------------
const u8 top_left[22] =   { 8, 7,  9, 2,  9, 1,  9, 0,  8, 5,  8, 0,  8, 1,  8, 2,  8, 3,  8, 4,  8, 6};
const u8 top_middle[22] = {10, 7, 11, 2, 11, 1, 11, 0, 10, 5, 10, 0, 10, 1, 10, 2, 10, 3, 10, 4, 10, 6};
const u8 top_right[22] =  {12, 7, 13, 2, 13, 1, 13, 0, 12, 5, 12, 0, 12, 1, 12, 2, 12, 3, 12, 4, 12, 6};
const u8 bottom_left[22] = {1, 7,  2, 2,  2, 1,  2, 0,  1, 5,  1, 0,  1, 1,  1, 2,  1, 3,  1, 4,  1, 6};
const u8 bottom_right[22]= {3, 7,  4, 2,  4, 1,  4, 0,  3, 5,  3, 0,  3, 1,  3, 2,  3, 3,  3, 4,  3, 6};

// These values closely reproduce times captured with logic analyser
#define delay_SPI_end_cycle() sleep_us(2)
#define delay_EPD_SCL_pulse() sleep_us(2)

/*
Now define how each digit maps to the segments:
          1
 10 :-----------
    |           |
  9 |           | 2
    |     11    |
  8 :-----------: 3
    |           |
  7 |           | 4
    |     5     |
  6 :----------- 
*/

const u8 digits[22][11] = {
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0},  // 0
    {2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0},   // 1
    {1, 2, 3, 5, 6, 7, 8, 10, 11, 0, 0}, // 2
    {1, 2, 3, 4, 5, 6, 10, 11, 0, 0, 0}, // 3
    {2, 3, 4, 8, 9, 10, 11, 0, 0, 0, 0}, // 4
    {1, 3, 4, 5, 6, 8, 9, 10, 11, 0, 0}, // 5
    {1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0}, // 6
    {1, 2, 3, 4, 10, 0, 0, 0, 0, 0, 0},  // 7
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, // 8
    {1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 0}, // 9
    {1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 0}, // A  10
    {3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0}, // b  11
    {5, 6, 7, 8, 11, 0, 0, 0, 0, 0, 0},  // c  12
    {2, 3, 4, 5, 6, 7, 8, 11, 0, 0, 0},  // d  13
    {1, 5, 6, 7, 8, 9, 10, 11, 0, 0, 0}, // E  14
    {1, 6, 7, 8, 9, 10, 11, 0, 0, 0, 0}, // F  15
    {5, 6, 7, 8, 9, 10, 0, 0, 0, 0, 0},  // L  16
	{4, 5, 6, 7, 11, 0, 0, 0, 0, 0, 0},  // o  17
	{2, 3, 4, 6, 7, 8, 9, 10, 11, 0, 0}, // H  18
	{3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0},   // i  19
	{5, 6, 7, 8, 9, 10, 11, 0, 0, 0, 0}, // t  20
	{1, 2, 4, 5, 6, 7, 11, 0, 0, 0, 0}   // a  21
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
    gpio_write(EPD_CSB, LOW);
    delay_EPD_SCL_pulse();

    // send the first bit, this indicates if the following is a command or data
    gpio_write(EPD_SDA, cd);
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
 		scr.display_buff[14] |= BIT(4); // "Г", "%", "( )", "."
	else
		scr.display_buff[14] &= ~BIT(4); // "Г", "%", "( )", "."
	if (symbol & 0x40)
		scr.display_buff[14] |= BIT(2); //"-"
	else
		scr.display_buff[14] &= ~BIT(2); //"-"
	if (symbol & 0x80)
		scr.display_buff[14] |= BIT(0); // "_"
	else
		scr.display_buff[14] &= ~BIT(0); // "_"
}
/* 0 = "   " off,
 * 1 = " o "
 * 2 = "o^o"
 * 3 = "o-o"
 * 4 = "oVo"
 * 5 = "vVv" happy
 * 6 = "^-^" sad
 * 7 = "oOo" */
_SCR_CODE_SEC_
void show_smiley(u8 state){
 	// off
	scr.display_buff[5] &= ~(BIT(2) | BIT(4) | BIT(6)); // do not reset %
	scr.display_buff[6] = 0;
	scr.display_buff[7] &= ~(BIT(4));
	if (state) {
		scr.display_buff[7] |= BIT(0); /* (  ) */
	}
	switch(state & 7) {
		case 1:
			scr.display_buff[5] |= BIT(4);
			scr.display_buff[6] |= BIT(0);
			break;
		case 2:
			scr.display_buff[5] |= BIT(6);
			scr.display_buff[6] |= BIT(4) | BIT(6);
			break;
		case 3:
			scr.display_buff[6] |= BIT(0) | BIT(4) | BIT(6);
			scr.display_buff[7] |= BIT(2);
			break;
		case 4:
			scr.display_buff[5] |= BIT(2);
			scr.display_buff[6] |= BIT(0) | BIT(2) | BIT(4) | BIT(6);
			scr.display_buff[7] |= BIT(2);
			break;
		case 5:
			scr.display_buff[5] |= BIT(4) | BIT(6);
			scr.display_buff[6] |= BIT(4) | BIT(6);
			break;
		case 6:
			scr.display_buff[6] |= BIT(0) | BIT(4) | BIT(6);
			break;
		case 7:
			scr.display_buff[5] |= BIT(4);
			scr.display_buff[6] |= BIT(0) | BIT(4) | BIT(6);
			break;
	}
}

_SCR_CODE_SEC_
void show_battery_symbol(bool state){
 	if (state)
		scr.display_buff[14] |= BIT(6);
	else
		scr.display_buff[14] &= ~BIT(6);
}

_SCR_CODE_SEC_
void show_ble_symbol(bool state){
 	if (state)
		scr.display_buff[0] |= BIT(4); // "ble"
	else
		scr.display_buff[0] &= ~BIT(4);
}

_SCR_CODE_SEC_
void show_connected_symbol(bool state){
#if USE_DISPLAY_CONNECT_SYMBOL == 2
 	if (!state)
#else
 	if (state)
#endif
		scr.display_buff[0] |= BIT(0); // "*"
	else
		scr.display_buff[0] &= ~BIT(0);
}

_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
static void epd_set_digit(u8 *buf, u8 digit, const u8 *segments) {
    // set the segments, there are up to 11 segments in a digit
    int segment_byte;
    int segment_bit;
    for (int i = 0; i < 11; i++) {
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

	scr.display_buff[14] &= ~(BIT(0) | BIT(2) | BIT(4));
 	if (symbol & 0x20)
 		scr.display_buff[14] |= BIT(4); // "Г", "%", "( )", "."
	if (symbol & 0x40)
		scr.display_buff[14] |= BIT(2); //"-"
	if (symbol & 0x80)
		scr.display_buff[14] |= BIT(0); // "_"

	scr.display_buff[8] = 0;
	scr.display_buff[9] = 0;
	scr.display_buff[10] = 0;
	scr.display_buff[11] = 0;
	scr.display_buff[12] = 0;
	scr.display_buff[13] = 0;
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
			scr.display_buff[11] |= BIT(4); // point top
		}
		/* show: -99..1999 */
		if (number < 0) {
			number = -number;
			scr.display_buff[8] = BIT(6); // "-"
		}
		/* number: -99..1999 */
		if (number > 999) scr.display_buff[7] |= BIT(4); // "1" 1000..1999
		if (number > 99) epd_set_digit(scr.display_buff, number / 100 % 10, top_left);
		if (number > 9) epd_set_digit(scr.display_buff, number / 10 % 10, top_middle);
		else epd_set_digit(scr.display_buff, 0, top_middle);
		epd_set_digit(scr.display_buff, number % 10, top_right);
	}
}

/* -9 .. 99 */
_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
void show_small_number(s16 number, bool percent){
	scr.display_buff[1] = 0;
	scr.display_buff[2] = 0;
	scr.display_buff[3] = 0;
	scr.display_buff[4] = 0;
	scr.display_buff[5] &= ~BIT(0);
	if (percent)
		scr.display_buff[5] |= BIT(0); // "%"
	if (number > 99) {
		// "Hi"
		epd_set_digit(scr.display_buff, Symbol_H, bottom_left);
		epd_set_digit(scr.display_buff, Symbol_i, bottom_right);
	} else if (number < -9) {
		// "Lo"
		epd_set_digit(scr.display_buff, Symbol_L, bottom_left);
		epd_set_digit(scr.display_buff, Symbol_o, bottom_right);
	} else {
		if (number < 0) {
			number = -number;
			scr.display_buff[1] |= BIT(6); // "-"
		}
		if (number > 9) epd_set_digit(scr.display_buff, number / 10 % 10, bottom_left);
		epd_set_digit(scr.display_buff, number % 10, bottom_right);
	}
}


void init_lcd(void) {
	// pulse RST_N low for 110 microseconds
    gpio_write(EPD_RST, LOW);
    sleep_us(110);
	scr.display_off = g_zcl_thermostatUICfgAttrs.display_off;
    scr.stage = 1; // Update/Init, stage 1
    scr.updated = 0;
	memset(scr.display_buff, 0, sizeof(scr.display_buff));
	memset(scr.display_cmp_buff, 0, sizeof(scr.display_cmp_buff));
    gpio_write(EPD_RST, HIGH);
	//scr.display_buff[15] = 0;
#if PM_ENABLE
	cpu_set_gpio_wakeup(EPD_BUSY, Level_High, 1);
#endif
}

_SCR_CODE_SEC_
__attribute__((optimize("-Os"))) int task_lcd(void) {
	while (gpio_read(EPD_BUSY)) {
		switch (scr.stage) {
		case 1: // Update/Init, stage 1
			transmit_blk(0, T_LUT_ping, sizeof(T_LUT_ping));
			scr.stage = 2;
			break;
		case 2: // Update/Init, stage 2
			if (scr.updated == 0) {
				transmit_blk(0, T_LUT_init, sizeof(T_LUT_init));
			} else {
				transmit_blk(0, T_LUT_work, sizeof(T_LUT_work));
			}
			scr.stage = 3;
			break;
		case 3: // Update/Init, stage 3
			transmit(0, 0x040);
			transmit(0, 0x0A9);
			transmit(0, 0x0A8);
			transmit_blk(1, scr.display_cmp_buff, sizeof(scr.display_cmp_buff));
			transmit(0, 0x0AB);
			transmit(0, 0x0AA);
			transmit(0, 0x0AF);
			if (scr.updated) {
				scr.stage = 4;
				// EPD_BUSY: ~500 ms
			} else {
				scr.updated = 1;
				scr.stage = 2;
				// EPD_BUSY: ~1000 ms
			}
			pm_wait_us(200);
			break;
		case 4: // Update, stage 4
			transmit(0, 0x0AE);
			transmit(0, 0x028);
			transmit(0, 0x0AD);
			scr.init = 1;
		default:
			if((!scr.display_off) // g_zcl_thermostatUICfgAttrs.display_off
			&& memcmp(scr.display_cmp_buff, scr.display_buff, sizeof(scr.display_buff))) {
				memcpy(scr.display_cmp_buff, scr.display_buff, sizeof(scr.display_cmp_buff));
				scr.stage = 1;
			} else
				scr.stage = 0;
		}
		if(!scr.stage)
			break;
	}
#if PM_ENABLE
	cpu_set_gpio_wakeup(EPD_BUSY, Level_High , scr.stage != 0);
#endif
	return scr.stage;
}


#if	USE_CLOCK

void show_clock(void) {
	uint32_t tmp = utc_time_sec / 60;
	uint32_t min = tmp % 60;
	uint32_t hrs = tmp / 60 % 24;
	memset(scr.display_buff, 0, sizeof(scr.display_buff));
	epd_set_digit(scr.display_buff, min / 10 % 10, bottom_left);
	epd_set_digit(scr.display_buff, min % 10, bottom_right);
	epd_set_digit(scr.display_buff, hrs / 10 % 10, top_left);
	epd_set_digit(scr.display_buff, hrs % 10, top_middle);
}
#endif // USE_CLOCK

void show_ble_ota(void) {
	memset(scr.display_buff, 0, LCD_BUF_SIZE);
	epd_set_digit(scr.display_buff, Symbol_o, top_left);
	epd_set_digit(scr.display_buff, Symbol_t, top_middle);
	epd_set_digit(scr.display_buff, Symbol_a, top_right);
	scr.display_buff[9] |= BIT(6); // "ble"
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

#endif // BOARD == BOARD_MHO_C401N
