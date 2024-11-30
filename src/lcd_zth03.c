/*
 * lcd_zth03.c
 *
 *      Created: pvvx, 30.11.2024
 *
 */
#include "tl_common.h"
#if (BOARD == BOARD_ZTH03)
#include "chip_8258/timer.h"

#include "i2c_drv.h"
#include "lcd.h"
#include "device.h"

#define _LCD_SPEED_CODE_SEC_ _attribute_ram_code_sec_

#define TH03_I2C_ADDR		0x3E // BL55028

#define I2C_TCLK_US	24 // 24 us


/*
 *  TH03 LCD buffer:  byte.bit

              --0.4--         --1.4--            --2.4--          BAT
       |    |         |     |         |        |         |        3.6
       |   0.6       0.0   1.6       1.0      2.6       2.0
       |    |         |     |         |        |         |      o 3.5
-3.3- 0.3     --0.2--         --1.2--            --2.2--          +--- 3.5
       |    |         |     |         |        |         |     3.5|
       |   0.5       0.1   1.5       1.1      2.5       2.1       ---- 3.7
       |    |         |     |         |        |         |     3.5|
              --0.7--         --1.7--     *      --2.7--          ---- 2.3
                                         1.3

                                        --4.4--         --5.4--
                                      |         |     |         |
          3.0      3.0               4.6       4.0   5.6       5.0
          / \      / \                |         |     |         |
    3.4(  ___  3.2 ___  )3.4            --4.2--         --5.2--
          3.2  / \ 3.2                |         |     |         |
               ___                   4.5       4.1   5.5       5.1     %
               3.0                    |         |     |         |     5.3
                                        --4.7--         --5.7--
                        OO 4.3

  None: 3.1
*/

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
#define LCD_SYM_A  0b001110111 // "A"
#define LCD_SYM_a  0b011110110 // "a"
#define LCD_SYM_P  0b001110101 // "P"

#define LCD_SYM_BLE	0x08	// connect
#define LCD_SYM_BAT	0x40	// battery

u8 display_buff[LCD_BUF_SIZE];
u8 display_cmp_buff[LCD_BUF_SIZE+1];


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

#define lcd_send_i2c_byte(a)  soft_i2c_send_byte(i2c_address_lcd, a)
#define lcd_send_i2c_buf(b, a)  soft_i2c_send_buf(i2c_address_lcd, (u8 *) b, a)

#if 1
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

#else

const u8 lcd_init_cmd[]	=	{
		// LCD controller initialize:
		0xea, // Set IC Operation(ICSET): Software Reset, Internal oscillator circuit
		0xd8, // Mode Set (MODE SET): Display enable, 1/3 Bias, power saving
		0xf0, // blink control off,  0xf2 - blink
		0x00, 0xff,0xff,0xff,0xff,0xff,0xff
};

#endif

_LCD_SPEED_CODE_SEC_
void send_to_lcd(void){
	if(g_zcl_thermostatUICfgAttrs.display_off)
		return;
	if (i2c_address_lcd) {
		lcd_send_i2c_buf(display_cmp_buff, sizeof(display_cmp_buff));
	}
}


void init_lcd(void){
	i2c_address_lcd = TH03_I2C_ADDR << 1;
	//display_cmp_buff[0] = 0;
	if(g_zcl_thermostatUICfgAttrs.display_off) {
		if(lcd_send_i2c_byte(0xD0) || lcd_send_i2c_byte(0xEA)) // LCD reset
			i2c_address_lcd = 0;
	} else {
		if(lcd_send_i2c_buf((u8 *) lcd_init_cmd, sizeof(lcd_init_cmd)))
			i2c_address_lcd = 0;
		else {
			pm_wait_us(200);
			display_cmp_buff[0] = 0;
			send_to_lcd();
		}
	}
}

#if 0
/* 0x00 = "  "
 * 0x20 = "°Г"
 * 0x40 = " -"
 * 0x60 = "°F"
 * 0x80 = " _"
 * 0xA0 = "°C"
 * 0xC0 = " ="
 * 0xE0 = "°E" */
_LCD_SPEED_CODE_SEC_
void show_temp_symbol(u8 symbol) {
	if (symbol & 0x20)
		display_buff[3] |= BIT(5);
	else
		display_buff[3] &= ~(BIT(5));
	if (symbol & 0x40)
		display_buff[3] |= BIT(7); //"-"
	else
		display_buff[3] &= ~BIT(7); //"-"
	if (symbol & 0x80)
		display_buff[2] |= BIT(3); // "_"
	else
		display_buff[2] &= ~BIT(3); // "_"
}
#endif

/* 0 = off, 1 = happy, 2 = sad*/
_LCD_SPEED_CODE_SEC_
void show_smiley(u8 state){
	display_buff[3] &= ~(BIT(0) | BIT(2) | BIT(4));
	if(state == 1)
		display_buff[2] |= BIT(0) | BIT(4);
	else if(state == 2)
		display_buff[2] |= BIT(2) | BIT(4);
}

_LCD_SPEED_CODE_SEC_
void show_ble_symbol(bool state){
	if (state)
		display_buff[4] |= LCD_SYM_BLE;
	else 
		display_buff[4] &= ~LCD_SYM_BLE;
}

_LCD_SPEED_CODE_SEC_
void show_battery_symbol(bool state){
	if (state)
		display_buff[3] |= BIT(6);
	else
		display_buff[3] &= ~BIT(6);
}

/* number in 0.1 (-995..19995), Show: -99 .. -9.9 .. 199.9 .. 1999 */
_LCD_SPEED_CODE_SEC_
__attribute__((optimize("-Os"))) void show_big_number_x10(s16 number, u8 symbol){
	display_buff[3] &= ~BIT(6); // "bat"
	display_buff[2] = 0;
	if(symbol==1) {
		display_buff[3] |= BIT(5);
		display_buff[2] |= BIT(3);
	}
	else if(symbol==2) {
		display_buff[3] |= BIT(5) | BIT(7);
	}
	if (number > 19995) {
   		display_buff[0] = LCD_SYM_H; // "H"
   		display_buff[1] = LCD_SYM_i; // "i"
	} else if (number < -995) {
   		display_buff[0] = LCD_SYM_L; // "L"
   		display_buff[1] = LCD_SYM_o; // "o"
	} else {
		display_buff[0] = 0;
		display_buff[1] = 0;
		/* number: -19995..19995 */
		if (number > 1995 || number < -1995) {
			display_buff[1] = 0; // no point, show: -99..1999
			if (number < 0){
				number = -number;
				display_buff[0] = BIT(2); // "-"
			}
			number = (number + 5) / 10; // round(div 10)
		} else { // show: -199.9..199.9
			display_buff[1] = BIT(3); // point,
			if (number < 0){
				number = -number;
				display_buff[0] = BIT(2); // "-"
			}
		}
		/* number: -99..1999 */
		if (number > 999) display_buff[0] |= BIT(3); // "1" 1000..1999
		if (number > 99) display_buff[0] |= display_numbers[number / 100 % 10];
		if (number > 9) display_buff[1] |= display_numbers[number / 10 % 10];
		else display_buff[1] |= LCD_SYM_0; // "0"
	    display_buff[2] |= display_numbers[number %10];
	}
}

/* -9 .. 99 */
_LCD_SPEED_CODE_SEC_
__attribute__((optimize("-Os"))) void show_small_number(s16 number, bool percent){
	display_buff[4] &= BIT(3); // connect
	display_buff[5] = percent? BIT(3) : 0;
	if (number > 99) {
		display_buff[4] |= LCD_SYM_H; // "H"
		display_buff[5] |= LCD_SYM_i; // "i"
	} else if (number < -9) {
		display_buff[4] |= LCD_SYM_L; // "L"
		display_buff[5] |= LCD_SYM_o; // "o"
	} else {
		if (number < 0) {
			number = -number;
			display_buff[4] = BIT(2); // "-"
		}
		if (number > 9) display_buff[4] |= display_numbers[number / 10 % 10];
		display_buff[5] |= display_numbers[number %10];
	}
}

void show_blink_screen(void) {
	memset(&display_buff, 0, sizeof(display_buff));
	display_buff[0] = BIT(2); // "_"
	display_buff[1] = BIT(2); // "_"
	display_buff[2] = BIT(2); // "_"
	display_buff[4] = BIT(3); // "ble"
	display_cmp_buff[0] = 0xf2;
	send_to_lcd();
}

// #define SHOW_REBOOT_SCREEN()
void show_reboot_screen(void) {
	memset(&display_buff, 0xff, sizeof(display_buff));
	send_to_lcd();
}

#if	USE_DISPLAY_CLOCK
_LCD_SPEED_CODE_SEC_
void show_clock(void) {
	uint32_t tmp = utc_time_sec / 60;
	uint32_t min = tmp % 60;
	uint32_t hrs = (tmp / 60) % 24;
	display_buff[0] = 0;
	display_buff[1] = display_numbers[(hrs / 10) % 10];
	display_buff[2] = display_numbers[hrs % 10];
	display_buff[3] &= BIT(6); // bat
	display_buff[4] &= BIT(3); // connect
	display_buff[4] |= display_numbers[(min / 10) % 10];
	display_buff[5] = display_numbers[min % 10];
}
#endif // USE_DISPLAY_CLOCK

_LCD_SPEED_CODE_SEC_
void update_lcd(void){
	if(g_zcl_thermostatUICfgAttrs.display_off)
		return;
	if(memcmp(&display_cmp_buff[1], display_buff, sizeof(display_buff))) {
		memcpy(&display_cmp_buff[1], display_buff, sizeof(display_buff));
		send_to_lcd();
		if(display_cmp_buff[0] == 0xf2)
			display_cmp_buff[0] = 0xf0;
		else
			display_cmp_buff[0] = 0;
	}
}


#endif // DEVICE_TYPE == DEVICE_TH03
