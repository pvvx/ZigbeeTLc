/*
 * lcd_mho_c122.c
 * Created: pvvx, 28.05.2023 
 * Edited by: FaBjE
 *
 *  https://github.com/pvvx/ATC_MiThermometer/issues/339
 */
#include "tl_common.h"
#if BOARD == BOARD_MHO_C122
#include "chip_8258/timer.h"

#include "i2c_drv.h"
#include "lcd.h"
#include "device.h"

#define _SCR_CODE_SEC_

RAM scr_data_t scr;

#define lcd_send_i2c_byte(a)  send_i2c_byte(scr.i2c_address, a)
#define lcd_send_i2c_buf(b, a)  send_i2c_bytes(scr.i2c_address, (u8 *) b, a)

#define LCD_SYM_Top_E	0b10010111	// "E"
#define LCD_SYM_Top_H	0b01100111	// "H"
#define LCD_SYM_Top_i	0b00000100	// "i"
#define LCD_SYM_Top_L	0b10000101	// "L"
#define LCD_SYM_Top_o	0b11000110	// "o"

#define LCD_SYM_Top_t	0b10000111	// "t"
#define LCD_SYM_Top_a	0b11110110	// "a"

#define LCD_SYM_Bot_E	0b01111001	// "E"
#define LCD_SYM_Bot_H   0b01110110	// "H"
#define LCD_SYM_Bot_i	0b00010000	// "i"
#define LCD_SYM_Bot_L	0b01010001	// "L"
#define LCD_SYM_Bot_o	0b00110011	// "o"


const u8 lcd_init_cmd_b14[] =	{0x80,0x3B,0x80,0x02,0x80,0x0F,0x80,0x95,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x19,0x80,0x28,0x80,0xE3,0x80,0x11};
								//	{0x80,0x40,0xC0,byte1,0xC0,byte2,0xC0,byte3,0xC0,byte4,0xC0,byte5,0xC0,byte6};
const u8 lcd_init_clr_b14[] =	{0x80,0x40,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0};

/* 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F*/
const u8 display_numbers[] = {
		0b11110101, // 0
		0b01100000, // 1
		0b10110110, // 2
		0b11110010, // 3
		0b01100011, // 4
		0b11010011, // 5
		0b11010111, // 6
		0b01110000, // 7
		0b11110111, // 8
		0b11110011, // 9
		0b01110111, // A
		0b11000111, // b
		0b10010101, // C
		0b11100110, // d
		0b10010111, // E
		0b00010111};  // F
const u8 display_small_numbers[] = {
		0b01011111, // 0
		0b00000110, // 1
		0b00111101, // 2
		0b00101111, // 3
		0b01100110, // 4
		0b01101011, // 5
		0b01111011, // 6
		0b00001110, // 7
		0b01111111, // 8
		0b01101111, // 9
		0b01111110, // A
		0b01110011, // b
		0b01011001, // C
		0b00110111, // d
		0b01111001, // E
		0b01111000};  // F

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
		reg_i2c_adr_dat = 0x4080;
		reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		reg_i2c_adr = 0xC0;
		for(buff_index = 0; buff_index < sizeof(scr.display_buff); buff_index++) {
			reg_i2c_do = *p++;
			reg_i2c_ctrl = FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		}
		reg_i2c_ctrl = FLD_I2C_CMD_STOP;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		irq_restore(r);
	}
}

void init_lcd(void){
	scr.display_off = g_zcl_thermostatUICfgAttrs.display_off;
	scr.i2c_address = scan_i2c_addr(B14_I2C_ADDR << 1);
	if (scr.i2c_address) {
// 		GPIO_PB6 set in app_config.h!
//		gpio_setup_up_down_resistor(GPIO_PB6, PM_PIN_PULLUP_10K); // LCD on low temp needs this, its an unknown pin going to the LCD controller chip
//		pm_wait_ms(50); // LCD_INIT_DELAY()
		lcd_send_i2c_buf((u8 *) lcd_init_cmd_b14, sizeof(lcd_init_cmd_b14));
		lcd_send_i2c_buf((u8 *) lcd_init_clr_b14, sizeof(lcd_init_clr_b14));
		memset(&scr.display_buff, 0xff, sizeof(scr.display_buff));
		send_to_lcd();
	} else
		scr.display_off = 1;
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
	scr.display_buff[3] &= ~(BIT(0) | BIT(1) | BIT(2));
	if (symbol & 0x20)
		scr.display_buff[3] |= BIT(0);
	if (symbol & 0x40)
		scr.display_buff[3] |= BIT(1); //"-"
	if (symbol & 0x80)
		scr.display_buff[3] |= BIT(2); // "_"
}

/* 0 = "     " off,
 * 1 = " ^-^ "
 * 2 = " -^- "
 * 3 = " ooo "
 * 4 = "(   )"
 * 5 = "(^-^)" happy
 * 6 = "(-^-)" sad
 * 7 = "(ooo)" */
_SCR_CODE_SEC_
void show_smiley(u8 state){
	scr.display_buff[1] &= ~BIT(3);
	scr.display_buff[3] &= ~(BIT(5) | BIT(6));

	if(state & 1)
		scr.display_buff[3] |= BIT(5); // Happy mouth
	if(state & 2)
		scr.display_buff[3] |= BIT(6); // Sad mouth
	if(state & 4)
		scr.display_buff[1] |= BIT(3); // Smiley contour
}

_SCR_CODE_SEC_
void show_ble_symbol(bool state){
	if (state)
		scr.display_buff[4] |= BIT(7);
	else 
		scr.display_buff[4] &= ~BIT(7);
}

_SCR_CODE_SEC_
void show_connected_symbol(bool state){
 	if (state)
		scr.display_buff[3] |= BIT(7); // "*"
	else
		scr.display_buff[3] &= ~BIT(7);
}

_SCR_CODE_SEC_
void show_battery_symbol(bool state){
	if (state)
		scr.display_buff[5] |= BIT(7);
	else 
		scr.display_buff[5] &= ~BIT(7);
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

	scr.display_buff[3] &= ~(BIT(0) | BIT(1) | BIT(2));
	if (symbol & 0x20)
		scr.display_buff[3] |= BIT(0);
	if (symbol & 0x40)
		scr.display_buff[3] |= BIT(1); //"-"
	if (symbol & 0x80)
		scr.display_buff[3] |= BIT(2); // "_"

	scr.display_buff[0] = 0;
	scr.display_buff[1] &= BIT(3); // Clear digit (except smiley contour)
	scr.display_buff[2] = 0;

	if (number > 19995) {
   		scr.display_buff[0] |= LCD_SYM_Top_H; // "H"
   		scr.display_buff[1] |= LCD_SYM_Top_i; // "i"
	} else if (number < -995) {
   		scr.display_buff[0] |= LCD_SYM_Top_L; // "L"
   		scr.display_buff[1] |= LCD_SYM_Top_o; // "o"
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
			scr.display_buff[2] = BIT(3); // point top
		}
		/* show: -99..1999 */
		if (number < 0) {
			number = -number;
			scr.display_buff[0] |= BIT(1); // "-"
		}
		/* number: -99..1999 */
		if (number > 999) scr.display_buff[0] |= BIT(3); // "1" 1000..1999
		if (number > 99) scr.display_buff[0] |= display_numbers[number / 100 % 10];
		if (number > 9) scr.display_buff[1] |= display_numbers[number / 10 % 10];
		else scr.display_buff[1] |= display_numbers[0]; // "0"
	    scr.display_buff[2] |= display_numbers[number %10];
	}
}

/* -9 .. 99 */
_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
void show_small_number(s16 number, bool percent){
	scr.display_buff[4] &= BIT(7); //Clear digit (except BLE symbol)
	scr.display_buff[5] &= BIT(7); //Clear digit (except BAT symbol)

	if (percent)
		scr.display_buff[3] |= BIT(4); // %
	else
		scr.display_buff[3] &= ~BIT(4); // %

	if (number > 99) {
		scr.display_buff[5] |= LCD_SYM_Bot_H;  // "H"
		scr.display_buff[4] |= LCD_SYM_Bot_i;  // "i"
	} else if (number < -9) {
		scr.display_buff[5] |= LCD_SYM_Bot_L; // "L"
		scr.display_buff[4] |= LCD_SYM_Bot_o; // "o"
	} else {
		if (number < 0) {
			number = -number;
			scr.display_buff[5] |= BIT(5); // "-"
		}
		if (number > 9) 
			scr.display_buff[5] |= display_small_numbers[number / 10 % 10];

		scr.display_buff[4] |= display_small_numbers[number %10];
	}
}

void show_ble_ota(void) {
	scr.display_buff[0] = LCD_SYM_Top_o; // "o"
	scr.display_buff[1] = LCD_SYM_Top_t; // "t"
	scr.display_buff[2] = LCD_SYM_Top_a; // "a"
	scr.display_buff[3] &= BIT(7); // "(|)"
	scr.display_buff[4] = BIT(7); // "ble"
	scr.display_buff[5] &= BIT(7); // "bat"
	update_lcd();
}

void show_err_sensors(void) {
	scr.display_buff[0] = LCD_SYM_Bot_E; // E
	scr.display_buff[1] = LCD_SYM_Bot_E; // E
	scr.display_buff[2] = 0;
	scr.display_buff[3] &= BIT(7); // "(|)"
	scr.display_buff[4] &= BIT(7); // "ble"
	scr.display_buff[5] &= BIT(7); // "bat"
	scr.display_buff[4] |= LCD_SYM_Top_E; // E
	scr.display_buff[5] |= LCD_SYM_Top_E; // E
}

void show_reset_screen(void) {
	scr.display_buff[0] = LCD_SYM_Top_o; // "o"
	scr.display_buff[1] = LCD_SYM_Top_o; // "o"
	scr.display_buff[2] = 0;
	scr.display_buff[3] = 0;
	scr.display_buff[4] = LCD_SYM_Bot_o; // "o"
	scr.display_buff[5] = LCD_SYM_Bot_o; // "o"
	update_lcd();
}

#if	USE_CLOCK
void show_clock(void) {
	uint32_t tmp = utc_time_sec / 60;
	uint32_t min = tmp % 60;
	uint32_t hrs = tmp / 60 % 24;

	scr.display_buff[0] = 0;
	scr.display_buff[1] &= BIT(3); //Clear digit (except smiley contour)

	scr.display_buff[0] = display_numbers[hrs / 10 % 10];
	scr.display_buff[1] = display_numbers[hrs % 10];
	scr.display_buff[2] = 0;
	scr.display_buff[3] &= BIT(7);

	scr.display_buff[4] &= BIT(7); //Clear digit (except BLE symbol)
	scr.display_buff[5] &= BIT(7); //Clear digit (except BAT symbol)

	scr.display_buff[4] |= display_small_numbers[min % 10];
	scr.display_buff[5] |= display_small_numbers[min / 10 % 10];
}
#endif // USE_CLOCK

#endif // BOARD == BOARD_MHO_C122
