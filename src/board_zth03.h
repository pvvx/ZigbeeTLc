/********************************************************************************************************
 * @file    board_zth03.h
 *
 * @brief   This is the header file for board_zth03
 *
 *******************************************************************************************************/
#ifndef _BOARD_ZTH03_H_
#define _BOARD_ZTH03_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZTH03)

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define ZIGBEE_TUYA_OTA 	1

// https://pvvx.github.io/TS0201_TZ3000_TH03/
// TLSR8258
// GPIO_PA0 - free (Reed Switch, input)
// GPIO_PA1 - free
// GPIO_PA7 - SWS, (debug TX)
// GPIO_PB1 - SCL, used I2C LCD
// GPIO_PB4 - free, (TRG)
// GPIO_PB5 - free
// GPIO_PB6 - free
// GPIO_PB7 - SDA, used I2C LCD
// GPIO_PC0 - LED
// GPIO_PC1 - free
// GPIO_PC2 - SDA, used I2C Sensor
// GPIO_PC3 - SCL, used I2C Sensor
// GPIO_PC4 - free
// GPIO_PD2 - free
// GPIO_PD3 - free
// GPIO_PD4 - used KEY
// GPIO_PD7 - free

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2032

// KEY, BUTTON
#define BUTTON1				GPIO_PD4
#define PD4_INPUT_ENABLE	1
#define PD4_DATA_OUT		0
#define PD4_OUTPUT_ENABLE	0
#define PD4_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PD4 PM_PIN_PULLUP_1M

// LED
#define USE_BLINK_LED		1
#define LED_ON				1
#define LED_OFF				0
#define GPIO_LED			GPIO_PC0
#define PC0_FUNC	  		AS_GPIO
#define PC0_OUTPUT_ENABLE	1
#define PC0_INPUT_ENABLE	1
#define PC0_DATA_OUT		LED_OFF

// I2C Sensor
#define	USE_I2C_DRV			I2C_DRV_HARD
#define I2C_CLOCK			400000 // Hz
#define I2C_SCL 			GPIO_PC2
#define I2C_SDA 			GPIO_PC3
#define I2C_GROUP 			I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

// Sensor T&H
#define USE_SENSOR_CHT8305		1
#define USE_SENSOR_CHT8215		1
#define USE_SENSOR_AHT20_30		1
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		1
#define USE_SENSOR_SHT30		1

// DISPLAY
#define SHOW_SMILEY			1
#define	USE_DISPLAY			5
#define LCD_BUF_SIZE		6
#define LCD_INIT_DELAY()

// I2C LCD
#define I2C_SCL_LCD			GPIO_PB1
#define I2C_SDA_LCD			GPIO_PB7
#define PB1_INPUT_ENABLE	1
#define PB1_DATA_OUT		0
#define PB1_OUTPUT_ENABLE	0
#define PB1_FUNC			AS_GPIO
#define PB7_INPUT_ENABLE	1
#define PB7_DATA_OUT		0
#define PB7_OUTPUT_ENABLE	0
#define PB7_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB1	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PB7	PM_PIN_PULLUP_10K

// VBAT
#define SHL_ADC_VBAT		C5P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PC5 // missing pin on case TLSR8251F512ET24
#define PC5_INPUT_ENABLE	0
#define PC5_DATA_OUT		1
#define PC5_OUTPUT_ENABLE	1
#define PC5_FUNC			AS_GPIO

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
#endif // (BOARD == BOARD_ZTH03)
#endif // _BOARD_ZTH03_H_
