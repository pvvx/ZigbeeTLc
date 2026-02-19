#include "tl_common.h"
#if (BOARD == BOARD_ZTH05)
#include "chip_8258/timer.h"
#include "i2c_drv.h"
#include "lcd.h"
#include "device.h"

scr_data_t scr;

#define lcd_send_i2c_byte(a)  send_i2c_byte(scr.i2c_address, a)
#define lcd_send_i2c_buf(b, a)  send_i2c_bytes(scr.i2c_address, (u8 *) b, a)

/* t,H,h,L,o,i */
#define LCD_SYM_H	0b001100111	// "H"
#define LCD_SYM_i	0b001000000	// "i"
#define LCD_SYM_L	0b011100000	// "L"
#define LCD_SYM_o	0b011000110	// "o"
#define LCD_SYM_E	0b011110010 // "E"
#define LCD_SYM_a	0b011010111	// "a"
#define LCD_SYM_t	0b011100010	// "t"

/* 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F*/
const u8 display_numbers[] = {
		// 76543210
		0b011110101, // 0  0xf5
		0b000000101, // 1  0x05
		0b011010011, // 2  0xd3
		0b010010111, // 3  0x97
		0b000100111, // 4  0x27
		0b010110110, // 5  0xb6
		0b011110110, // 6  0xf6
		0b000010101, // 7  0x15
		0b011110111, // 8  0xf7
		0b010110111, // 9  0xb7
		0b001110111, // A  0x77
		0b011100110, // b  0xe6
		0b011110000, // C  0xf0
		0b011000111, // d  0xc7
		0b011110010, // E  0xf2
		0b001110010  // F  0x72
};

/* Test cmd ():
 * 0400007ceaa49cacbcf0fcc808ffffffff,
 * 0400007cf3c8 - blink
 */
//const u8 lcd_init_cmd[] = {0xb6,0xfc, 0xc8, 0xe8, 0x08, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
const u8 lcd_init_cmd[]	=	{
		// LCD controller initialize:
		0xea, // Set IC 2Operation(ICSET): Software Reset, Internal oscillator circuit
		0xd8, // Mode Set (MODE SET): Display enable, 1/3 Bias, power saving
		0xbc, // Display control (DISCTL): Power save mode 3, FRAME flip, Power save mode 1
		0xf0, // blink control off,  0xf2 - blink
		0xfc, // All pixel control (APCTL): Normal
		0x08, // addr buffer 
		0x00,0x00,000,0x11,0x00,0x00
};

_SCR_CODE_SEC_
void send_to_lcd(void){
	if (scr.i2c_address) {
		lcd_send_i2c_buf(scr.display_cmp_buff, sizeof(scr.display_cmp_buff));
		if(scr.blink_flg) {
			lcd_send_i2c_byte(scr.blink_flg);
			if(scr.blink_flg > 0xf0)
				scr.blink_flg = 0xf0;
			else
				scr.blink_flg = 0;
		}
	}
}
/*
void send_to_lcd(void){
	if (scr.i2c_address) {
		if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
		u8 * p = scr.display_cmp_buff;
		reg_i2c_id = scr.i2c_address;
		reg_i2c_adr = 0x08;	// addr:8
		reg_i2c_do = *p++;
		reg_i2c_di = *p++;
		reg_i2c_ctrl = FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_DI | FLD_I2C_CMD_START;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		reg_i2c_do = *p++;
		reg_i2c_di = *p++;
		reg_i2c_ctrl = FLD_I2C_CMD_DO | FLD_I2C_CMD_DI;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		reg_i2c_do = *p++;
		reg_i2c_di = *p;
		reg_i2c_ctrl = FLD_I2C_CMD_DO | FLD_I2C_CMD_DI | FLD_I2C_CMD_STOP;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	}
}
*/

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
	scr.display_buff[3] &= ~(BIT(6)| BIT(7));
	if(symbol & 0x20) {
		scr.display_buff[3] |= BIT(6);
	}
	if(symbol & 0x40) {
		scr.display_buff[3] |= BIT(7);
	}
	if(symbol & 0x80) {
		scr.display_buff[2] |= BIT(3);
	} else {
		scr.display_buff[2] &= ~BIT(3);
	}
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
	scr.display_buff[3] &= ~(BIT(0) | BIT(1)| BIT(4));
	if(state & 1) {
		scr.display_buff[3] |= BIT(0);
	}
	if(state & 2) {
		scr.display_buff[3] |= BIT(1);
	}
	if(state & 4) {
		scr.display_buff[3] |= BIT(4);
	}
}

_SCR_CODE_SEC_
void show_ble_symbol(bool state){
	if (state)
		scr.display_buff[4] |= BIT(3);
	else
		scr.display_buff[4] &= ~BIT(3);
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

_SCR_CODE_SEC_
void show_battery_symbol(bool state){
	if (state)
		scr.display_buff[3] |= BIT(5);
	else
		scr.display_buff[3] &= ~BIT(5);
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
void show_big_number_x10(s16 number, u8 symbol){

	scr.display_buff[2] = 0;
	scr.display_buff[3] &= ~(BIT(6)| BIT(7));
	if(symbol & 0x20)
		scr.display_buff[3] |= BIT(6);
	if(symbol & 0x40)
		scr.display_buff[3] |= BIT(7);
	if(symbol & 0x80)
		scr.display_buff[2] |= BIT(3);

	if (number > 19995) {
   		scr.display_buff[0] = LCD_SYM_H; // "H"
   		scr.display_buff[1] = LCD_SYM_i; // "i"
	} else if (number < -995) {
   		scr.display_buff[0] = LCD_SYM_L; // "L"
   		scr.display_buff[1] = LCD_SYM_o; // "o"
	} else {
		scr.display_buff[0] = 0;
		/* number: -995..19995 */
		scr.display_buff[5] = 0;
		if (number > 1999 || number < -99) {
			/* number: -995..-100, 2000..19995 */
			// round(div 10)
			number += 5;
			number /= 10;
			// show no point: -99..-10, 200..1999
			scr.display_buff[1] = 0; // no point top
		} else {
			// show point: -9.9..199.9
			scr.display_buff[1] = BIT(3); // point top
		}
		/* show: -99..1999 */
		if (number < 0) {
			number = -number;
			scr.display_buff[0] = BIT(1); // "-"
		}
		/* number: -99..1999 */
		if (number > 999)
			scr.display_buff[0] |= BIT(3); // "1" 1000..1999
		if (number > 99)
			scr.display_buff[0] |= display_numbers[number / 100 % 10];
		if (number > 9)
			scr.display_buff[1] |= display_numbers[number / 10 % 10];
		else
			scr.display_buff[1] |= 0xF5; // "0"
	    scr.display_buff[2] |= display_numbers[number %10];
	}
}

/* -9 .. 99 */
_SCR_CODE_SEC_
void show_small_number(s16 number, bool percent){
	scr.display_buff[4] &= BIT(3); // and "oo"
	scr.display_buff[5] = percent? BIT(3): 0; // "%"
	if (number > 99) {
		scr.display_buff[4] |= LCD_SYM_H; // "H"
		scr.display_buff[5] |= LCD_SYM_i; // "i"
	} else if (number < -9) {
		scr.display_buff[4] |= LCD_SYM_L; // "L"
		scr.display_buff[5] |= LCD_SYM_o; // "o"
	} else {
		if (number < 0) {
			number = -number;
			scr.display_buff[4] |= BIT(1); // "-"
		}
		if (number > 9)
			scr.display_buff[4] |= display_numbers[number / 10];
		scr.display_buff[5] |= display_numbers[number %10];
	}
}

void update_lcd(void){
	if(!scr.display_off
	&& memcmp(&scr.display_cmp_buff[1], scr.display_buff, sizeof(scr.display_buff))) {
		memcpy(&scr.display_cmp_buff[1], scr.display_buff, sizeof(scr.display_buff));
		send_to_lcd();
	}
}

void display_off(void) {
//	send_i2c_byte(BL55028_I2C_ADDR << 1, 0xd0);
	send_i2c_byte(BL55028_I2C_ADDR << 1, 0xEA);
	scr.display_off = 1;
}

void init_lcd(void){
	scr.display_off = g_zcl_thermostatUICfgAttrs.display_off;
	scr.i2c_address = BL55028_I2C_ADDR << 1;
	if(scr.display_off) {
		display_off(); // lcd reset
	} else {
		if(lcd_send_i2c_buf((u8 *) lcd_init_cmd, sizeof(lcd_init_cmd))) {
			display_off();
		} else {
			pm_wait_us(200);
			scr.blink_flg = 0;
			scr.display_cmp_buff[0] = 8;
			memset(&scr.display_buff, 0xff, sizeof(scr.display_buff));
			update_lcd();
		}
	}
}

#if	USE_DISPLAY_CLOCK
void show_clock(void) {
	u32 tmp = wrk.utc_time_sec / 60;
	u32 min = tmp % 60;
	u32 hrs = (tmp / 60) % 24;
	scr.display_buff[0] = display_numbers[hrs / 10];
	scr.display_buff[1] = display_numbers[hrs % 10];
	scr.display_buff[2] = 0;
	scr.display_buff[3] = 0;
	scr.display_buff[4] = display_numbers[min / 10];
	scr.display_buff[5] = display_numbers[min % 10];
}
#endif // USE_DISPLAY_CLOCK

void show_ble_ota(void) {
	scr.display_buff[0] = LCD_SYM_o; // "o"
	scr.display_buff[1] = LCD_SYM_t; // "t"
	scr.display_buff[2] = LCD_SYM_a; // "a"
	scr.display_buff[3] &= BIT(5); // "bat"
	scr.display_buff[4] = BIT(3); // "ble"
	scr.display_buff[5] = 0;
	scr.blink_flg = 0xf2; // flash screen
	update_lcd();
}

void show_err_sensors(void) {
	scr.display_buff[0] = 0b011110010; // E
	scr.display_buff[1] = 0b011110010; // E
	scr.display_buff[2] = 0;
	scr.display_buff[3] &= BIT(5); // "bat"
	scr.display_buff[4] &= BIT(3); // "ble"
	scr.display_buff[4] |= 0b011110010; // E
	scr.display_buff[5] = 0b011110010; // E
}

void show_reset_screen(void) {
	scr.display_buff[0] = LCD_SYM_o; // "o"
	scr.display_buff[1] = LCD_SYM_o; // "o"
	scr.display_buff[2] = LCD_SYM_o; // "o"
	scr.display_buff[3] &= BIT(5); // "bat"
	scr.display_buff[4] &= BIT(3); // "ble"
	scr.display_buff[5] = 0;
	scr.blink_flg = 0xf2; // flash screen
	update_lcd();
}


#endif // (BOARD == BOARD_ZTH05)
