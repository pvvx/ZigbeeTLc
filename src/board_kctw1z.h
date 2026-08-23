/********************************************************************************************************
 * @file    board_kctw1z.h
 *
 * @brief   Tuya KCTW1Z (_TZ3000_itnrsufe, TS0201) = Moes ZSS-KB-TH
 *          Zigbee Temperature/Humidity Sensor with LCD, CR2032
 *
 * ZTU module pinout:
 *   ZTU pin 3  -> PC0 : BUTTON (active LOW -> GND)
 *   ZTU pin 8  -> PC2 : I2C SCL (SHT40 @ 0x44)
 *   ZTU pin 9  -> PC3 : I2C SDA (SHT40 @ 0x44)
 *   ZTU C1     -> LCD SCL (VINKA VKL144B)
 *   ZTU D2     -> LCD SDA (VINKA VKL144B)
 *   ZTU pin 13 -> GND
 *   ZTU pin 14 -> VCC
 *   ZTU pin 15 -> PB1 : UART TX (ISP)
 *   ZTU pin 16 -> PB7 : UART RX (ISP)
 *   ZTU pin 18 -> RST : RESET (ISP)
 *   ZTU pin 4  -> SWS : SWire programming (ISP)
 *
 * LED: NOT present. Identify via Zigbee icon on LCD.
 *
 *******************************************************************************************************/
#ifndef _BOARD_KCTW1Z_H_
#define _BOARD_KCTW1Z_H_

#include "version_cfg.h"

#if (BOARD == BOARD_KCTW1Z)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_SCREEN | SERVICE_THS)

/*
LCD KCTW1Z segment map (determined experimentally).

Addressing: segments are numbered from 1 ("steps"). kctw1z_set_step() maps a
step to display_buff[(step-1)/8] bit ((step-1)%8). The controller RAM holds 18
bytes (datasheet 8.2, steps 1..144); the stock refresh transmits the first 12
(steps 1..96). The known glass map uses steps 1..64, so bytes 8..11 stay zero.
Per-digit step lists and symbol steps live in lcd_kctw1z.c:
kctw1z_temp_left/right/frac_digit[], kctw1z_humi_left/right_digit[],
show_temp_symbol(), show_ble_symbol(), show_battery_symbol().

Segment layout:

   -  [T1][T2] . [T3]  [degC/F]        [BT] [Zigbee]
                                       .  |  |  |
                                      dot low mid high

      --T1.a--         --T2.a--            --T3.a--
    |         |     |         |        |         |
   T1.f      T1.b   T2.f     T2.b     T3.f      T3.b
    |         |     |         |        |         |
      --T1.g--         --T2.g--   .      --T3.g--
    |         |     |         | (dot24) |         |
   T1.e      T1.c   T2.e     T2.c     T3.e      T3.c
    |         |     |         |        |         |
      --T1.d--         --T2.d--          --T3.d--


      --H1.a--         --H2.a--
    |         |     |         |
   H1.f      H1.b   H2.f     H2.b        [battery]
    |         |     |         |
      --H1.g--         --H2.g--             %%
    |         |     |         |
   H1.e      H1.c   H2.e     H2.c
    |         |     |         |
      --H1.d--         --H2.d--

Step assignments (a,b,c,d,e,f,g order):
  T1 (temp tens) : 4,15,13,1,2,3,14
  T2 (temp ones) : 12,23,21,9,10,11,22
  T3 (temp .1)   : 48,43,41,45,46,47,42
  H1 (humi tens) : 20,31,29,17,18,19,30
  H2 (humi ones) : 28,39,37,25,26,27,38
  minus: 5   temp dot: 24   humi divider: 32   percent: 33
  unmapped: 6 (always cleared, no known segment on this glass)
  temp unit: C=44+64, F=63+64, degree corner=64
  Bluetooth: 53   Zigbee: 54
  signal: min=50, middle=61, max=49, dot=62
  battery: outline+fill1(bottom)=51, fill2=52, fill3=56, fill4(top)=55

*/

#define BLE_MODEL_STR		"KCTW1Z"
#define BLE_MAN_STR			"Tuya"

#define ZCL_BASIC_MFG_NAME     {4,'T','u','y','a'}
#define ZCL_BASIC_MODEL_ID	   {8,'K','C','T','W','1','Z','-','z'}

#define USE_BATTERY 	BATTERY_CR2032

#define SHOW_SMILEY			0
#define	USE_DISPLAY			2
#define LCD_BUF_SIZE		18
#define LCD_INIT_DELAY()
#define USE_DISPLAY_BATTERY_LEVEL		1
#define USE_DISPLAY_OFF					1
#define USE_DISPLAY_CONNECT_SYMBOL		1

#define BUTTON1				GPIO_PC0
#define BUTTON1_ON 			0
#define BUTTON1_OFF 		1
#define PC0_INPUT_ENABLE	1
#define PC0_DATA_OUT		0
#define PC0_OUTPUT_ENABLE	0
#define PC0_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC0 PM_PIN_PULLUP_1M

// no LED on this board - leave USE_BLINK_LED undefined
//#define USE_BLINK_LED		0

// HW I2C sensor bus, same as BOARD_TS0201_TZ3000 (pins driven by I2C init)
#define	USE_I2C_DRV			I2C_DRV_HARD
#define I2C_CLOCK			100000 // Hz
#define I2C_SCL 			GPIO_PC2
#define I2C_SDA 			GPIO_PC3
#define I2C_GROUP			I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

#define I2C_SCL_LCD			GPIO_PC1
#define I2C_SDA_LCD			GPIO_PD2

#define PC1_INPUT_ENABLE	1
#define PC1_DATA_OUT		0
#define PC1_OUTPUT_ENABLE	0
#define PC1_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC1	PM_PIN_PULLUP_10K
#define PD2_INPUT_ENABLE	1
#define PD2_DATA_OUT		0
#define PD2_OUTPUT_ENABLE	0
#define PD2_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PD2	PM_PIN_PULLUP_10K

// Tuya ships different T/RH chips in this family - auto-detection
#define USE_SENSOR_CHT8305		1
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		1
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		0
#define USE_SENSOR_SHT30		1

#define SHL_ADC_VBAT		C5P
#define GPIO_VBAT			GPIO_PC5
#define PC5_INPUT_ENABLE	0
#define PC5_DATA_OUT		1
#define PC5_OUTPUT_ENABLE	1
#define PC5_FUNC			AS_GPIO

#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS
#endif

#endif // (BOARD == BOARD_KCTW1Z)
#endif // _BOARD_KCTW1Z_H_
