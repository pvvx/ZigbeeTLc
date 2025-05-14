#ifndef _LCD_H_
#define _LCD_H_

#if USE_DISPLAY

/*
 * Used defined:
 *
 * LCD_BUF_SIZE		n
 * [LCD_CMP_BUF_SIZE] n + 1
 *
 * SHOW_SMILEY	0/1
 *
 * [USE_DISPLAY_BATTERY_LEVEL]
 * [USE_DISPLAY_SMALL_NUMBER_X10]
 * [USE_DISPLAY_OFF]
 * [USE_DISPLAY_CONNECT_SYMBOL]
 *
 *
 */

#ifndef _SCR_CODE_SEC_
#define _SCR_CODE_SEC_ // _attribute_ram_code_
#endif

#ifndef RAM
#define RAM // _attribute_data_retention_
#endif

#define ZIGBEE_BATT_LEVEL	1	// =1 - battery_level: 0..200, =0 - battery_level: 0..100

// LCD controller I2C address
#define B14_I2C_ADDR		0x3C
#define B16_I2C_ADDR		0	 // UART
#define B19_I2C_ADDR		0x3E // BU9792AFUV
#define BU9792AFUV_I2C_ADDR		0x3E // BU9792AFUV
#define BL55028_I2C_ADDR		0x3E // BL55028

typedef enum {
	TEMP_SYMBOL_NONE = 0,	// "  "
	TEMP_SYMBOL_C = 0xA0,	// "°C"
	TEMP_SYMBOL_E = 0xE0,	// "°E"
	TEMP_SYMBOL_F = 0x60	// "°F"
} TEMP_SYMBOL_e;

void show_temp_symbol(u8 symbol);

#ifdef USE_EPD

typedef struct  {
	u8 display_off; // !g_zcl_thermostatUICfgAttrs.display_off
	u8 init;
	u8 stage;
	u8 updated;
	u16 refresh_cnt;
	u8 display_buff[LCD_BUF_SIZE];
	u8 display_cmp_buff[LCD_BUF_SIZE];
} scr_data_t;

int task_lcd(void);

#define update_lcd() task_lcd()

#else

#ifndef LCD_CMP_BUF_SIZE
#define LCD_CMP_BUF_SIZE LCD_BUF_SIZE
#endif

typedef struct  {
	u8 display_off; // !g_zcl_thermostatUICfgAttrs.display_off
	u8 i2c_address;
	u8 blink_flg;
	u8 display_buff[LCD_BUF_SIZE];
	u8 display_cmp_buff[LCD_CMP_BUF_SIZE];
} scr_data_t;

void update_lcd(void);

#endif

extern scr_data_t scr;

void init_lcd(void);
#ifdef USE_DISPLAY_BATTERY_LEVEL
void show_battery_symbol(bool state, u8 battery_level);
#else
void show_battery_symbol(bool state);
#endif
void show_big_number_x10(s16 number, u8 symbol);

#ifdef USE_DISPLAY_SMALL_NUMBER_X10
void show_small_number_x10(s16 number, bool percent);
#else
void show_small_number(s16 number, bool percent);
#endif

void show_ble_symbol(bool state);
#ifdef USE_DISPLAY_CONNECT_SYMBOL
void show_connected_symbol(bool state);
#if USE_DISPLAY_CONNECT_SYMBOL == 2 // =2 inverted
#define show_light(a) show_connected_symbol(!(a))
#else
#define show_light(a) show_connected_symbol(a)
#endif
#else
#define show_connected_symbol(a) // none
#define show_light(a) show_ble_symbol(a)
#endif

#if SHOW_SMILEY

#define SMILE_NONE  0 		// "     "
#define SMILE_HAPPY 5 		// "(^-^)" happy
#define SMILE_SAD   6 		// "(-^-)" sad

void show_smiley(u8 state);

#endif // SHOW_SMILEY

void show_ble_ota(void);
void show_err_sensors(void);
void show_reset_screen(void);

#ifdef USE_DISPLAY_OFF
// TODO: сравнить уровень потребления питания с двумя командами 0xd0 + 0xea
void display_off(void);
#else
#define display_off() // не отключается
#endif

#endif // USE_DISPLAY
#endif // _LCD_H_
