#include "tl_common.h"
#if BOARD == BOARD_LYWSD03MMC
#include "chip_8258/timer.h"
#include "i2c_drv.h"
#include "lcd.h"
#include "device.h"

// UART 38400 BAUD
#define LCD_UART_BAUD 38400
#if CLOCK_SYS_CLOCK_HZ == 16000000
#define uartCLKdiv 51 // 16000000/(7+1)/(51+1)=38461.538...
#define bwpc 7
#elif CLOCK_SYS_CLOCK_HZ == 24000000
#define uartCLKdiv 124 // 24000000/(4+1)/(124+1)=38400
#define bwpc 4
#elif CLOCK_SYS_CLOCK_HZ == 32000000
#define uartCLKdiv 103 // 32000000/(7+1)/(103+1)=38461.538...
#define bwpc 7
#elif CLOCK_SYS_CLOCK_HZ == 48000000
#define uartCLKdiv 124 // 48000000/(9+1)/(124+1)=38400
#define bwpc 9
#else
#error "Set UART CLK!"
#endif

typedef struct __attribute__((packed)) _dma_uart_buf_t {
	volatile u32 dma_len;
	u32 head;
	u8 start;
	u8 data[6];
	u8 chk;
	u8 end;
} dma_uart_buf_t;


#define LCD_SYM_H	0x67	// "H"
#define LCD_SYM_i	0x40	// "i"
#define LCD_SYM_L	0xE0	// "L"
#define LCD_SYM_o	0xC6	// "o"
#define LCD_SYM_t	0b11100010	// "t"
#define LCD_SYM_E 	0b11110010 // "E"
#define LCD_SYM_a	0b11010111 // "a"
#define LCD_SYM_0	0xF5	// "0"

#define TMP_SYM_C	0xA0	// "°C"
#define TMP_SYM_F	0x60	// "°F"

scr_data_t scr;
dma_uart_buf_t utxb;

const u8 lcd_init_cmd_b14[] =	{0x80,0x3B,0x80,0x02,0x80,0x0F,0x80,0x95,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x19,0x80,0x28,0x80,0xE3,0x80,0x11};
								//	{0x80,0x40,0xC0,byte1,0xC0,byte2,0xC0,byte3,0xC0,byte4,0xC0,byte5,0xC0,byte6};
const u8 lcd_init_clr_b14[] =	{0x80,0x40,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0};

const u8 lcd_init_b19[]	=	{
		0xea, // Set IC Operation(ICSET): Software Reset, Internal oscillator circuit
		0xa4, // Display control (DISCTL): Normal mode, FRAME flip, Power save mode 1
//		0x9c, // Address set (ADSET): 0x1C ?
		0xac, // Display control (DISCTL): Power save mode 1, FRAME flip, Power save mode 1
		0xbc, // Display control (DISCTL): Power save mode 3, FRAME flip, Power save mode 1
		0xf0, // Blink control (BLKCTL): Off
		0xfc, // All pixel control (APCTL): Normal
		0xc8, // Mode Set (MODE SET): Display ON, 1/3 Bias
		0x00, // Set Address 0
		// Clear 18 bytes RAM BU9792AFUV
		0,0, 0,0, 0,0, 0,0,
		LCD_SYM_o,LCD_SYM_o, 0,LCD_SYM_o, LCD_SYM_o,LCD_SYM_o, 0,0, 0,0
};
/* 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F*/
const u8 display_numbers[] = {
	//76543210
	0b11110101, // 0  0xf5
	0b00000101, // 1  0x05
	0b11010011, // 2  0xd3
	0b10010111, // 3  0x97
	0b00100111, // 4  0x27
	0b10110110, // 5  0xb6
	0b11110110, // 6  0xf6
	0b00010101, // 7  0x15
	0b11110111, // 8  0xf7
	0b10110111, // 9  0xb7
	0b01110111, // A  0x77
	0b11100110, // b  0xe6
	0b11110000, // C  0xf0
	0b11000111, // d  0xc7
	0b11110010, // E  0xf2
	0b01110010  // F  0x72
};

#define lcd_send_i2c_buf(b, a)  send_i2c_bytes(scr.i2c_address, (u8 *) b, a)

/* B1.5, B1.6 (UART LCD)
  u8 * p = scr.display_buff; */
static void lcd_send_uart(u8 *p){
	utxb.start = 0xAA;
	utxb.data[5] = *p++;
	utxb.data[4] = *p++;
	utxb.data[3] = *p++;
	utxb.data[2] = *p++;
	utxb.data[1] = *p++;
	utxb.data[0] = *p;
	utxb.chk = utxb.data[0]^utxb.data[1]^utxb.data[2]^utxb.data[3]^utxb.data[4]^utxb.data[5];
	utxb.end = 0x55;
	utxb.dma_len = sizeof(utxb) - sizeof(utxb.dma_len);
	// init uart
	reg_clk_en0 |= FLD_CLK0_UART_EN;
	///reg_clk_en1 |= FLD_CLK1_DMA_EN;
	uart_reset();
	// reg_uart_clk_div/reg_uart_ctrl0
	REG_ADDR32(0x094) = MASK_VAL(FLD_UART_CLK_DIV, uartCLKdiv, FLD_UART_CLK_DIV_EN, 1)
		|	((MASK_VAL(FLD_UART_BPWC, bwpc)	| (FLD_UART_TX_DMA_EN)) << 16) // set bit width, enable UART DMA mode
			| ((MASK_VAL(FLD_UART_CTRL1_STOP_BIT, 0)) << 24) // 00: 1 bit, 01: 1.5bit 1x: 2bits;
		;
	// reg_dma1_addr/reg_dma1_ctrl
	REG_ADDR32(0xC04) = (unsigned short)((u32)(&utxb)) //set tx buffer address
		| 	(((sizeof(utxb)+15)>>4) << 16); //set tx buffer size
	///reg_dma1_addrHi = 0x04; (in sdk init?)
	reg_dma_chn_en |= FLD_DMA_CHN_UART_TX;
	///reg_dma_chn_irq_msk |= FLD_DMA_IRQ_UART_TX;

	// GPIO_PD7 set TX UART pin
	REG_ADDR8(0x5AF) = (REG_ADDR8(0x5AF) &  (~(BIT(7)|BIT(6)))) | BIT(7);
	BM_CLR(reg_gpio_func(UART_TX_PD7), UART_TX_PD7 & 0xff);
	/// gpio_set_input_en(UART_TX_PD7, 1); ???
	// start send DMA
	reg_dma_tx_rdy0 |= FLD_DMA_CHN_UART_TX; // start tx
	// wait send (3.35 ms), sleep?
	sleep_us(3330); // 13 bytes * 10 bits / 38400 baud = 0.0033854 sec = 3.4 ms power ~3 mA
	//while (reg_dma_tx_rdy0 & FLD_DMA_CHN_UART_TX); ?
	while (!(reg_uart_status1 & FLD_UART_TX_DONE));
	// set low/off power UART
	reg_uart_clk_div = 0;
}

_SCR_CODE_SEC_
u8 reverse(u8 revByte) {
   revByte = (revByte & 0xF0) >> 4 | (revByte & 0x0F) << 4;
   revByte = (revByte & 0xCC) >> 2 | (revByte & 0x33) << 2;
   revByte = (revByte & 0xAA) >> 1 | (revByte & 0x55) << 1;
   return revByte;
}

_SCR_CODE_SEC_
static void send_to_lcd(void) {
	u8 *p = scr.display_cmp_buff;
	unsigned int buff_index;
	unsigned char r = irq_disable();
	if (scr.i2c_address) {
		if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
		else {
			gpio_setup_up_down_resistor(I2C_SCL, PM_PIN_PULLUP_10K);
			gpio_setup_up_down_resistor(I2C_SDA, PM_PIN_PULLUP_10K);
		}
		if (scr.i2c_address == (B14_I2C_ADDR << 1)) {
			// B1.4, B1.7, B2.0
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
		} else { // (scr.i2c_address == (B19_I2C_ADDR << 1))
			// B1.9 BU9792AFUV
			for(buff_index = 0; buff_index < sizeof(scr.display_buff); buff_index++)
				utxb.data[buff_index] = reverse(*p++);
			p = utxb.data;
			reg_i2c_id = scr.i2c_address;
			reg_i2c_adr = 0x04;	// addr: 4
			reg_i2c_do = *p++;
			reg_i2c_di = *p++;
			reg_i2c_ctrl = FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_DI | FLD_I2C_CMD_START;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			reg_i2c_adr_dat = 0;
			reg_i2c_ctrl = FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			reg_i2c_adr = *p++;
			reg_i2c_do = *p++;
			reg_i2c_ctrl = FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			reg_i2c_adr_dat = 0;
			reg_i2c_ctrl = FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			reg_i2c_adr = *p++;
			reg_i2c_do = *p;
			reg_i2c_ctrl = FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_STOP;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);

			reg_i2c_adr = 0xC8; // LCD cmd: 0xc8 - Mode Set (MODE SET): Display ON, 1/3 Bias
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
		}
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	} else {
		// B1.5, B1.6 (UART LCD)
		lcd_send_uart(p);
	}
	irq_restore(r);
}

_SCR_CODE_SEC_
void show_ble_symbol(bool state){
	if(state)
		scr.display_buff[2] |= 0x10;
	else
		scr.display_buff[2] &= ~0x10;
}

#ifdef USE_DISPLAY_CONNECT_SYMBOL
_SCR_CODE_SEC_
void show_connected_symbol(bool state){
#if USE_DISPLAY_CONNECT_SYMBOL == 2
 	if (!state)
#else
 	if (state)
#endif
		scr.display_buff[2] |= 0x10; // "ble"
	else
		scr.display_buff[2] &= ~0x10;
}
#endif

_SCR_CODE_SEC_
void show_battery_symbol(bool state) {
	if(state)
		scr.display_buff[1] |= 0x08;
	else
		scr.display_buff[1] &= ~0x08;
}

/*0=off, 1=happy, 2=sad*/

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
	scr.display_buff[2] &= ~0x07;
	scr.display_buff[2] |= state & 0x07;
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
	scr.display_buff[2] &= ~0xE0;
	scr.display_buff[2] |= symbol & 0xE0;
	if (number > 19995) {
   		scr.display_buff[3] = 0;
   		scr.display_buff[4] = LCD_SYM_i; // "i"
   		scr.display_buff[5] = LCD_SYM_H; // "H"
	} else if (number < -995) {
   		scr.display_buff[3] = 0;
   		scr.display_buff[4] = LCD_SYM_o; // "o"
   		scr.display_buff[5] = LCD_SYM_L; // "L"
	} else {
		/* number: -995..19995 */
		scr.display_buff[5] = 0;
		if (number > 1999 || number < -99) {
			/* number: -995..-100, 2000..19995 */
			// round(div 10)
			number += 5;
			number /= 10;
			// show no point: -99..-10, 200..1999
		} else {
			// show point: -9.9..199.9
			scr.display_buff[4] = BIT(3); // point
		}
		/* show: -99..1999 */
		if (number < 0) {
			number = -number;
			scr.display_buff[5] = BIT(1); // "-"
		}
		/* number: -99..1999 */
		if (number > 999) scr.display_buff[5] |= 0x08; // "1" 1000..1999
		if (number > 99) scr.display_buff[5] |= display_numbers[number / 100 % 10];
		if (number > 9) scr.display_buff[4] |= display_numbers[number / 10 % 10];
		else scr.display_buff[4] |= LCD_SYM_0; // "0"
	    scr.display_buff[3] = display_numbers[number %10];
	}
}

/* -9 .. 99 */
_SCR_CODE_SEC_
__attribute__((optimize("-Os")))
void show_small_number(s16 number, bool percent){
	scr.display_buff[1] = scr.display_buff[1] & 0x08; // and battery
	scr.display_buff[0] = percent?0x08:0x00;
	if (number > 99) {
		scr.display_buff[0] |= LCD_SYM_i; // "i"
		scr.display_buff[1] |= LCD_SYM_H; // "H"
	} else if (number < -9) {
		scr.display_buff[0] |= LCD_SYM_o; // "o"
		scr.display_buff[1] |= LCD_SYM_L; // "L"
	} else {
		if (number < 0) {
			number = -number;
			scr.display_buff[1] = 2; // "-"
		}
		if (number > 9) scr.display_buff[1] |= display_numbers[number / 10 % 10];
		scr.display_buff[0] |= display_numbers[number %10];
	}
}

_SCR_CODE_SEC_
void update_lcd(void){
	if(scr.display_off)
		return;
	if(memcmp(scr.display_cmp_buff, scr.display_buff, sizeof(scr.display_buff))) {
		memcpy(scr.display_cmp_buff, scr.display_buff, sizeof(scr.display_buff));
		send_to_lcd();
	}
}

void display_off(void) {
	send_i2c_byte(B19_I2C_ADDR << 1, 0xea);
	//send_i2c_byte(B19_I2C_ADDR << 1, 0xd0);
	scr.display_off = 1;
}

void init_lcd(void){
	scr.display_off = g_zcl_thermostatUICfgAttrs.display_off;
/*
	//GPIO_PB6 set in app_config.h!
	//gpio_setup_up_down_resistor(GPIO_PB6, PM_PIN_PULLUP_10K); // LCD on low temp needs this, its an unknown pin going to the LCD controller chip
	gpio_set_output_en(GPIO_PB6, 1);
	gpio_write(GPIO_PB6, 0);
	pm_wait_us(110);
	gpio_write(GPIO_PB6, 1);
*/
	scr.i2c_address = scan_i2c_addr(B14_I2C_ADDR << 1);
	if (scr.i2c_address) { // B1.4, B1.7, B2.0
		// pm_wait_ms(50); LCD_INIT_DELAY()
		lcd_send_i2c_buf((u8 *) lcd_init_cmd_b14, sizeof(lcd_init_cmd_b14));
		lcd_send_i2c_buf((u8 *) lcd_init_clr_b14, sizeof(lcd_init_clr_b14));
		if(scr.display_off) {
			memset(&scr.display_cmp_buff, 0, sizeof(scr.display_cmp_buff));
			send_to_lcd();
			return;	// 5.0 uA
		} // 5.4 uA
	} else {
		scr.i2c_address = scan_i2c_addr(B19_I2C_ADDR << 1);
		if (scr.i2c_address) { // B1.9
			scr.blink_flg = 0;
			if(scr.display_off) {
				display_off();
			} else {
				lcd_send_i2c_buf((u8 *) lcd_init_b19, sizeof(lcd_init_b19));
				lcd_send_i2c_buf((u8 *) lcd_init_b19, sizeof(lcd_init_b19));
			}
		} // else B1.5, B1.6 uses UART (scr.i2c_address = 0)
	}
	//memset(&scr.display_buff, 0xff, sizeof(scr.display_buff));
	//update_lcd();
	show_reset_screen();
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
	scr.display_buff[0] = 0; // " "
	scr.display_buff[1] &= BIT(3); // "bat"
	scr.display_buff[2] = BIT(4); // "ble"
	scr.display_buff[3] = LCD_SYM_a; // "a"
	scr.display_buff[4] = LCD_SYM_t; // "t"
	scr.display_buff[5] = LCD_SYM_o; // "o"
	scr.blink_flg = 0xf2;
	update_lcd();
}

void show_err_sensors(void) {
	scr.display_buff[0] = LCD_SYM_E, // "E"
	scr.display_buff[1] &= BIT(3); // "bat"
	scr.display_buff[1] |= LCD_SYM_E; // "E"
	scr.display_buff[2] &= BIT(4); // "ble"
	scr.display_buff[3] = 0; // " "
	scr.display_buff[4] = LCD_SYM_E; // "E"
	scr.display_buff[5] = LCD_SYM_E; // "E"
}

void show_reset_screen(void) {
	scr.display_buff[0] = 0; // " "
	scr.display_buff[1] &= BIT(3); // "bat"
	scr.display_buff[2] &= BIT(4); // "ble"
	scr.display_buff[3] = LCD_SYM_o; // "o"
	scr.display_buff[4] = LCD_SYM_o; // "o"
	scr.display_buff[5] = LCD_SYM_o; // "o"
	scr.blink_flg = 0xf2;
	update_lcd();
}

#endif // BOARD == BOARD_LYWSD03MMC
