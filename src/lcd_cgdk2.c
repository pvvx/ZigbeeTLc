//#include "compiler.h"
#include "tl_common.h"

#if BOARD == BOARD_CGDK2

#include "chip_8258/timer.h"
#include "app_i2c.h"
#include "lcd.h"
#include "device.h"
#include <sensors.h>


#define CGDK2_I2C_ADDR		0x3E // BU9792AFUV

u8 i2c_address_lcd;
u8 lcd_blink;
u8 display_buff[LCD_BUF_SIZE];
u8 display_cmp_buff[LCD_BUF_SIZE];

// #define lcd_send_i2c_byte(a)  send_i2c_byte(i2c_address_lcd, a)
#define lcd_send_i2c_buf(b, a)  send_i2c_bytes(i2c_address_lcd, (u8 *) b, a)


/*
  CGDK2 LCD buffer:  byte.bit

       ---------1.5-------------      O 17.6  :--17.6---
       |                       |              |
 BLE  ||    |   |   |   |   |  |             17.6
 1.4  ||   1.6 1.7 1.3 1.2 1.1 |              |
      ||    |   |   |   |   |  |              :--17.5---
       |                       |              |
       -------------------------             17.6
                                              |
                                              :--17.4---

   |   2.7---2.3---5.7  3.3---4.7---5.6       0.7---0.6--17.3
   |    |           |    |           |         |           |
   |   2.6         3.7  3.2         4.3       0.3        17.2
   |    |           |    |           |         |           |
  1.0  2.5---2.2---3.6  3.1---4.6---4.2       0.2---0.5--17.1
   |    |           |    |           |         |           |
   |   2.4         3.5  3.0         4.1       0.1        17.0
   |    |           |    |           |   5.4   |           |
   |   2.0---2.1---3.4  4.4---4.5---4.0   *   0.0---0.4---5.5

  ------------------------------5.4--------------------------

   5.3---6.7---8.3  7.7---7.3---8.2   %   9.7---9.3--17.7
    |           |    |           |   8.1   |           |
   5.2         6.3  7.6         8.7       9.6        10.7
    |           |    |           |         |           |
   5.1---6.6---6.2  7.5---7.2---8.6       9.5---9.2--10.6
    |           |    |           |         |           |
   5.0         6.1  7.4         8.5       9.4        10.5
    |           |    |           |   8.0   |           |
   6.4---6.5---6.0  7.0---7.1---8.4   *   9.0---9.1--10.4


none: 1.4, 10.0-10.3, 11.0-16.7
*/

#define TMP_SYM_C	0xA0	// "°C"
#define TMP_SYM_F	0x60	// "°F"

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

const u8 digits[16][DEF_CGDK22_SUMBOL_SIGMENTS + 1] = {
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
    {1, 2, 8, 9, 10, 11, 12, 13, 0, 0, 0, 0, 0, 0}  // F
};

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
	reg_i2c_id = i2c_address_lcd;
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

void send_to_lcd(void){
	unsigned int buff_index;
	u8 * p = display_buff;
	if (i2c_address_lcd) {
		unsigned char r = irq_disable();
		if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
		reg_i2c_id = i2c_address_lcd;
		reg_i2c_adr_dat = 0xE800; // 0xe8 - Set IC Operarion(ICSET): Do not execute Software Reset, Internal oscillator circuit; 0x00 - ADSET
		reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);

		for(buff_index = 0; buff_index < sizeof(display_buff) - 1; buff_index++) {
			reg_i2c_do = *p++;
			reg_i2c_ctrl = FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		}
		reg_i2c_do = *p;
		reg_i2c_ctrl = FLD_I2C_CMD_DO | FLD_I2C_CMD_STOP;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);

		reg_i2c_adr = 0xC8; // 0xc8 - Mode Set (MODE SET): Display ON, 1/3 Bias
		reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_STOP;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		irq_restore(r);
	}
}

void update_lcd(void){
	if (memcmp(display_cmp_buff, display_buff, sizeof(display_buff))) {
		send_to_lcd();
		memcpy(display_cmp_buff, display_buff, sizeof(display_buff));
	}
	if(lcd_blink == 0xf2) {
		lcd_blink = 0xf0;
		lcd_send_i2c_buf(&lcd_blink, 1);
	}
}

void show_reboot_screen(void) {
	memset(&display_buff, 0xff, sizeof(display_buff));
	update_lcd();
}

void init_lcd(void){
	i2c_address_lcd = scan_i2c_addr(CGDK2_I2C_ADDR << 1);
	if (i2c_address_lcd) { // LCD CGDK2_I2C_ADDR ?
//		if(cfg.flg2.screen_off) {
//			lcd_send_i2c_byte(0xEA); // BU9792AFUV reset
//		} else {
			lcd_send_i2c_buf((u8 *) lcd_init_cmd, sizeof(lcd_init_cmd));
			pm_wait_us(200);
			show_reboot_screen();
//		}
	}
}

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
void show_temp_symbol(u8 symbol) {
	if (symbol & 0x20)
		display_buff[17] |= BIT(6);
	else
		display_buff[17] &= ~(BIT(6));
	if (symbol & 0x40)
		display_buff[17] |= BIT(5); //"-"
	else
		display_buff[17] &= ~BIT(5); //"-"
	if (symbol & 0x80)
		display_buff[17] |= BIT(4); // "_"
	else
		display_buff[17] &= ~BIT(4); // "_"
}

/* CGDK22 no symbol 'smiley' !
 * =5 -> "---" happy, != 5 -> "    " sad */
void show_smiley(u8 state){
	(void) state;
//	if (state & 1)
//		display_buff[x] |= BIT(x);
//	else
//		display_buff[x] &= ~BIT(x);
}

void show_battery_symbol(bool state){
	display_buff[1] &= ~(BIT(1) | BIT(2) | BIT(3) | BIT(5) | BIT(6) | BIT(7));
	if (state) {
		display_buff[1] |= BIT(5);
		if (measured_data.battery_level >= 32) {
			display_buff[1] |= BIT(1);
			if (measured_data.battery_level >= 66) {
				display_buff[1] |= BIT(2);
				if (measured_data.battery_level >= 98) {
					display_buff[1] |= BIT(3);
					if (measured_data.battery_level >= 134) {
						display_buff[1] |= BIT(7);
						if (measured_data.battery_level >= 166) {
							display_buff[1] |= BIT(6);
						}
					}
				}
			}
		}
	}
}

/* CGDK22 no symbol 'ble' ! */
void show_ble_symbol(bool state){
	if (state)
		display_buff[1] |= BIT(4);
	else
		display_buff[1] &= ~BIT(4);
}

/* number in 0.1 (-995..19995), Show: -99 .. -9.9 .. 199.9 .. 1999 */
__attribute__((optimize("-Os"))) void show_big_number_x10(int16_t number, u8 symbol) {
	if(symbol==1)
		show_temp_symbol(TMP_SYM_C);
	else if(symbol==2)
		show_temp_symbol(TMP_SYM_F);
	else
		show_temp_symbol(0);
	display_buff[0] = 0;
	display_buff[1] &= ~(BIT(0));
	display_buff[2] = 0;
	display_buff[3] = 0;
	display_buff[4] = 0;
	display_buff[5] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	display_buff[17] &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3));
	if (number > 19995) {
		// "Hi"
		display_buff[2] |= BIT(0) | BIT(2) | BIT(4) | BIT(5) | BIT(6) | BIT(7);
		display_buff[3] |= BIT(0) | BIT(1) | BIT(3) | BIT(4) | BIT(5) | BIT(6);
		display_buff[4] |= BIT(4);
	} else if (number < -995) {
		// "Lo"
		display_buff[2] |= BIT(0) | BIT(1) | BIT(4) | BIT(5) | BIT(6) | BIT(7);
		display_buff[3] |= BIT(0) | BIT(1) | BIT(4);
		display_buff[4] |= BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(5) | BIT(6);
	} else {
		/* number: -995..19995 */
		if (number > 1995 || number < -95) {
			// no point, show: -99..1999
			if (number < 0){
				number = -number;
				display_buff[2] |= BIT(2); // "-"
			}
			number = (number / 10) + ((number % 10) > 5); // round(div 10)
		} else { // show: -9.9..199.9
			display_buff[5] |= BIT(4); // point
			if (number < 0){
				number = -number;
				display_buff[2] |= BIT(2); // "-"
			}
		}
		/* number: -99..1999 */
		if (number > 999) display_buff[1] |= BIT(0); // "1" 1000..1999
		if (number > 99) cgdk22_set_digit(display_buff, number / 100 % 10, top_left);
		if (number > 9) cgdk22_set_digit(display_buff, number / 10 % 10, top_middle);
		else cgdk22_set_digit(display_buff, 0, top_middle);
		cgdk22_set_digit(display_buff, number % 10, top_right);
	}
}

/* number in 0.1 (-99..999) -> show:  -9.9 .. 99.9 */
__attribute__((optimize("-Os"))) void show_small_number_x10(int16_t number, bool percent){
	display_buff[5] &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3));
	display_buff[6] = 0;
	display_buff[7] = 0;
	display_buff[8] = 0;
	display_buff[9] = 0;
	display_buff[10] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	display_buff[17] &= ~(BIT(7));
	if (percent)
		display_buff[8] |= BIT(1); // "%"
	if (number > 9995) {
		// "Hi"
		display_buff[5] |= BIT(0) | BIT(1) | BIT(2) | BIT(3);
		display_buff[6] |= BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(6);
		display_buff[7] |= BIT(0) | BIT(4) | BIT(5) | BIT(7);
	} else if (number < -995) {
		//"Lo"
		display_buff[5] |= BIT(0) | BIT(1) | BIT(2) | BIT(3);
		display_buff[6] |= BIT(0) | BIT(4) | BIT(5);
		display_buff[7] |= BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(5);
		display_buff[8] |= BIT(4) | BIT(5) | BIT(6);
	} else {
		/* number: -99..999 */
		if (number > 995 || number < -95) {
			// no point, show: -99..999
			if (number < 0){
				number = -number;
				display_buff[6] |= BIT(6); // "-"
			}
			number = (number / 10) + ((number % 10) > 5); // round(div 10)
		} else { // show: -9.9..99.9
			display_buff[8] |= BIT(0); // point
			if (number < 0){
				number = -number;
				display_buff[6] |= BIT(6); // "-"
			}
		}
		/* number: -99..999 */
		if (number > 99) cgdk22_set_digit(display_buff, number / 100 % 10, bottom_left);
		if (number > 9) cgdk22_set_digit(display_buff, number / 10 % 10, bottom_middle);
		else cgdk22_set_digit(display_buff, 0, bottom_middle);
		cgdk22_set_digit(display_buff, number % 10, bottom_right);
	}
}

void show_blink_screen(void) {
	memset(&display_buff, 0, sizeof(display_buff));
	display_buff[0] = BIT(5); // "-"
	display_buff[1] = BIT(4); // "ble"
	display_buff[2] = BIT(2); // "-"
	display_buff[4] = BIT(6); // "-"
	display_buff[5] = BIT(4); // "-----"
	update_lcd();
	lcd_blink = 0xf2;
	lcd_send_i2c_buf(&lcd_blink, 1);
}


#endif // BOARD == BOARD_CGDK2
