/********************************************************************************************************
 * @file    board_zth03.h
 *
 * @brief   This is the header file for board_zth03
 *
 *******************************************************************************************************/
#ifndef _BOARD_LKTMZL02_H_
#define _BOARD_LKTMZL02_H_

#include "version_cfg.h"

#if (BOARD == BOARD_LKTMZL02)

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define RF_TX_POWER_DEF RF_POWER_INDEX_P3p01dBm //  2xAAA, if CR2032 -> RF_POWER_INDEX_P1p99dBm

// TLSR8258
// GPIO_PA0 - free (Reed Switch, input)
// GPIO_PA1 - free
// GPIO_PA7 - SWS, (debug TX)
// GPIO_PB1 - free
// GPIO_PB4 - LED - "1" On
// GPIO_PB5 - free, (TRG)
// GPIO_PB6 - free
// GPIO_PB7 - free
// GPIO_PC0 - KEY
// GPIO_PC1 - SDA1, used I2C LCD
// GPIO_PC2 - SDA, used I2C Sensor
// GPIO_PC3 - SCL, used I2C Sensor
// GPIO_PC4 - SCL1, used I2C LCD
// GPIO_PD2 - free
// GPIO_PD3 - free
// GPIO_PD4 - free
// GPIO_PD7 - free


// KEY, BUTTON
#define BUTTON1				GPIO_PC0
#define PC0_INPUT_ENABLE	1
#define PC0_DATA_OUT		0
#define PC0_OUTPUT_ENABLE	0
#define PC0_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC0 PM_PIN_PULLUP_1M

// LED
#define USE_BLINK_LED		1
#define LED_ON				1
#define LED_OFF				0
#define GPIO_LED			GPIO_PB4
#define PB4_FUNC	  		AS_GPIO
#define PB4_OUTPUT_ENABLE	1
#define PB4_INPUT_ENABLE	1
#define PB4_DATA_OUT		LED_OFF

// I2C Sensor
#define I2C_CLOCK			400000 // Hz
#define SENSOR_TYPE 		SENSOR_AHT2X3X
#define USE_SENSOR_ID		0

#define I2C_SCL 			GPIO_PC2
#define I2C_SDA 			GPIO_PC3
#define I2C_GROUP 			I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

// DISPLAY
#define SHOW_SMILEY			0
#define	USE_DISPLAY			6
#define LCD_BUF_SIZE		7
#define LCD_INIT_DELAY()

// I2C LCD
#define I2C_SCL_LCD			GPIO_PC4
#define I2C_SDA_LCD			GPIO_PC1
#define PC1_INPUT_ENABLE	1
#define PC1_DATA_OUT		0
#define PC1_OUTPUT_ENABLE	0
#define PC1_FUNC			AS_GPIO
#define PC4_INPUT_ENABLE	1
#define PC4_DATA_OUT		0
#define PC4_OUTPUT_ENABLE	0
#define PC4_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC1	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC4	PM_PIN_PULLUP_10K

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
#endif // (BOARD == BOARD_LKTMZL02)
#endif // _BOARD_LKTMZL02_H_
