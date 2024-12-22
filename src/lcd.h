#ifndef _LCD_H_
#define _LCD_H_

// LCD controller I2C address
#define B14_I2C_ADDR		0x3C
#define B16_I2C_ADDR		0	 // UART
#define B19_I2C_ADDR		0x3E // BU9792AFUV

u8 i2c_address_lcd;

void init_lcd(void);
void show_battery_symbol(bool state);
void show_big_number_x10(s16 number, u8 symbol);
#if BOARD == BOARD_CGDK2
void show_small_number_x10(s16 number, bool percent);
#else
void show_small_number(s16 number, bool percent);
#endif
#if (BOARD == BOARD_MHO_C401N) || (BOARD == BOARD_MHO_C401)
void show_connected_symbol(bool state);
#define update_lcd() task_lcd()
u8 stage_lcd;
#else
void update_lcd();
#endif
void show_smiley(u8 state);
void show_ble_symbol(bool state);
void show_blink_screen(void);

#ifdef USE_EPD
int task_lcd(void);
#endif

#endif // _LCD_H_
