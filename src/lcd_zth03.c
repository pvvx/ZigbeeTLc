/*
 * lcd_zth03.c
 * Created: pvvx, 30.11.2024
 */
#include "tl_common.h"
#if (BOARD == BOARD_ZTH03)
#include "chip_8258/timer.h"

#include "i2c_drv.h"
#include "lcd.h"
#include "device.h"

#define _SCR_CODE_SEC_

#define TH03_I2C_ADDR		0x3E // BL55028

#define I2C_TCLK_US	24 // 24 us

scr_data_t scr;

/* 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F*/
const u8 display_numbers[] = {
		// 76543210
		0b011110011, // 0
		0b000000011, // 1
		0b010110101, // 2
		0b010010111, // 3
		0b001000111, // 4
		0b011010110, // 5
		0b011110110, // 6
		0b000010011, // 7
		0b011110111, // 8
		0b011010111, // 9
		0b001110111, // A
		0b011100110, // b
		0b011110000, // C
		0b010100111, // d
		0b011110100, // E
		0b001110100  // F
};

#define LCD_SYM_b  0b011100110 // "b"
#define LCD_SYM_H  0b001100111 // "H"
#define LCD_SYM_h  0b001100110 // "h"
#define LCD_SYM_i  0b000100000 // "i"
#define LCD_SYM_L  0b011100000 // "L"
#define LCD_SYM_o  0b010100110 // "o"
#define LCD_SYM_t  0b011100100 // "t"
#define LCD_SYM_0  0b011110011 // "0"
#define LCD_SYM_E  0b011110100 // "E"
#define LCD_SYM_A  0b001110111 // "A"
#define LCD_SYM_a  0b011110110 // "a"
#define LCD_SYM_P  0b001110101 // "P"


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
		0xea, // Set IC Operation(ICSET): Software Reset, Internal oscillator circuit
		0xd8, // Mode Set (MODE SET): Display enable, 1/3 Bias, power saving
		0xbc, // Display control (DISCTL): Power save mode 3, FRAME flip, Power save mode 1
		0x80, // load data pointer
		0xf0, // blink control off,  0xf2 - blink
		0xfc, // All pixel control (APCTL): Normal
		0x60,
		0x00, 0x00,000,0x00,0x00,0x00,0x00,0x00,0x00
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

_SCR_CODE_SEC_
void update_lcd(void){
	if(!scr.display_off
	&& memcmp(&scr.display_cmp_buff[1], scr.display_buff, sizeof(scr.display_buff))) {
		memcpy(&scr.display_cmp_buff[1], scr.display_buff, sizeof(scr.display_buff));
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
	if (symbol & 0x20)
		scr.display_buff[3] |= BIT(5);
	else
		scr.display_buff[3] &= ~(BIT(5));
	if (symbol & 0x40)
		scr.display_buff[3] |= BIT(7); //"-"
	else
		scr.display_buff[3] &= ~BIT(7); //"-"
	if (symbol & 0x80)
		scr.display_buff[2] |= BIT(3); // "_"
	else
		scr.display_buff[2] &= ~BIT(3); // "_"
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
	scr.display_buff[3] &= ~(BIT(0) | BIT(2) | BIT(4)); // 0x15
	state = (state & 1) | ((state << 1) & 4) | ((state << 2) & 0x10);
	scr.display_buff[3] |= state;
}


_SCR_CODE_SEC_
void show_ble_symbol(bool state){
	if (state)
		scr.display_buff[4] |= BIT(3);
	else 
		scr.display_buff[4] &= ~BIT(3);
}

_SCR_CODE_SEC_
void show_battery_symbol(bool state){
	if (state)
		scr.display_buff[3] |= BIT(6);
	else
		scr.display_buff[3] &= ~BIT(6);
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
	scr.display_buff[3] &= ~(BIT(5) | BIT(7)); // "bat" & smiley
	scr.display_buff[2] = 0;

	if (symbol & 0x20)
		scr.display_buff[3] |= BIT(5); // "°Г"
	if (symbol & 0x40)
		scr.display_buff[3] |= BIT(7); //"-"
	if (symbol & 0x80)
		scr.display_buff[2] |= BIT(3); // "_"

	if (number > 19995) {
   		scr.display_buff[0] = LCD_SYM_H; // "H"
   		scr.display_buff[1] = LCD_SYM_i; // "i"
	} else if (number < -19995) {
   		scr.display_buff[0] = LCD_SYM_L; // "L"
   		scr.display_buff[1] = LCD_SYM_o; // "o"
	} else {
		/* number: -19995..19995 */
		scr.display_buff[0] = 0;
		scr.display_buff[1] = 0;
		if (number > 1999 || number < -1999) {
			/* number: -19995..-2000, 2000..19995 */
			// round(div 10)
			number += 5;
			number /= 10;
			// show no point: -1999..-200, 200..1999
		} else {
			// show point: -199.9..199.9
			scr.display_buff[1] = BIT(3); // point,
		}
		/* show: -1999..1999 */
		if (number < 0) {
			number = -number;
			if(number > 99)
				scr.display_buff[3] = BIT(3); // "-"
			else
				scr.display_buff[0] = BIT(2); // "-"
		}
		if (number > 999) scr.display_buff[0] |= BIT(3); // "1" 1000..1999
		if (number > 99) scr.display_buff[0] |= display_numbers[number / 100 % 10];
		if (number > 9) scr.display_buff[1] |= display_numbers[number / 10 % 10];
		else scr.display_buff[1] |= LCD_SYM_0; // "0"
	    scr.display_buff[2] |= display_numbers[number %10];
	}
}

/* -9 .. 99 */
_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
void show_small_number(s16 number, bool percent){
	scr.display_buff[4] &= BIT(3); // connect
	scr.display_buff[5] = percent? BIT(3) : 0;
	if (number > 99) {
		scr.display_buff[4] |= LCD_SYM_H; // "H"
		scr.display_buff[5] |= LCD_SYM_i; // "i"
	} else if (number < -9) {
		scr.display_buff[4] |= LCD_SYM_L; // "L"
		scr.display_buff[5] |= LCD_SYM_o; // "o"
	} else {
		if (number < 0) {
			number = -number;
			scr.display_buff[4] = BIT(2); // "-"
		}
		if (number > 9) scr.display_buff[4] |= display_numbers[number / 10 % 10];
		scr.display_buff[5] |= display_numbers[number %10];
	}
}

void display_off(void) {
	send_i2c_byte(TH03_I2C_ADDR << 1, 0xd0);
	send_i2c_byte(TH03_I2C_ADDR << 1, 0xEA);
	scr.display_off = 1;
}

void init_lcd(void){
	scr.display_off = g_zcl_thermostatUICfgAttrs.display_off;
	scr.i2c_address = TH03_I2C_ADDR << 1;
	if(scr.display_off) {
		display_off();
	} else {
		if(lcd_send_i2c_buf((u8 *) lcd_init_cmd, sizeof(lcd_init_cmd))) {
			display_off();
		} else {
			pm_wait_us(200);
			scr.blink_flg = 0;
			scr.display_cmp_buff[0] = 0;
			memset(&scr.display_buff, 0xff, sizeof(scr.display_buff));
			update_lcd();
		}
	}
}

#if	USE_DISPLAY_CLOCK
_SCR_CODE_SEC_
void show_clock(void) {
	u32 tmp = wrk.utc_time_sec / 60;
	u32 min = tmp % 60;
	u32 hrs = (tmp / 60) % 24;
	scr.display_buff[0] = 0;
	scr.display_buff[1] = display_numbers[(hrs / 10) % 10];
	scr.display_buff[2] = display_numbers[hrs % 10];
	scr.display_buff[3] &= BIT(6); // bat
	scr.display_buff[4] &= BIT(3); // ble - connect
	scr.display_buff[4] |= display_numbers[(min / 10) % 10];
	scr.display_buff[5] = display_numbers[min % 10];
}
#endif // USE_DISPLAY_CLOCK

void show_ble_ota(void) {
	scr.display_buff[0] = LCD_SYM_o; // "o"
	scr.display_buff[1] = LCD_SYM_t; // "t"
	scr.display_buff[2] = LCD_SYM_a; // "a"
	scr.display_buff[3] &= BIT(6); // "bat"
	scr.display_buff[4] = BIT(3); // "ble"
	scr.display_buff[5] = 0; // " "
	scr.blink_flg = 0xf2;
	update_lcd();
}

void show_err_sensors(void) {
	scr.display_buff[0] = LCD_SYM_E; // E
	scr.display_buff[1] = LCD_SYM_E; // E
	scr.display_buff[2] = 0;
	scr.display_buff[3] &= BIT(6); // "bat"
	scr.display_buff[4] = LCD_SYM_E; // E
	scr.display_buff[5] = LCD_SYM_E; // E
}

void show_reset_screen(void) {
	scr.display_buff[0] = LCD_SYM_o; // "o"
	scr.display_buff[1] = LCD_SYM_o; // "o"
	scr.display_buff[2] = LCD_SYM_o; // "o"
	scr.display_buff[3] = 0; // " "
	scr.display_buff[4] = 0; // " "
	scr.display_buff[5] = 0; // " "
	scr.blink_flg = 0xf2;
	update_lcd();
}

#endif // DEVICE_TYPE == DEVICE_ZTH03
