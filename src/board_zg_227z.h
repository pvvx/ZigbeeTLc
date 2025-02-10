/*
 * board_zg_227z.h
 *
 *  Created on: 10 февр. 2025 г.
 *      Author: pvvx
 */
/*
 * board_zg_227z.h
 *
 *      Author: pvvx
 */
#ifndef _BOARD_ZG_227Z_H_
#define _BOARD_ZG_227Z_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZG_227Z)

#define ZIGBEE_TUYA_OTA 	1

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2450

// TLSR8253 512K Flash
// GPIO_PA7 - SWS, free, (debug TX)
// GPIO_PB1 - TX
// GPIO_PC2 - LED
// GPIO_PB7 - RX
// GPIO_PD4 - KEY
// GPIO_PC3 - SDA
// GPIO_PC4 - SCL

// BUTTON
#define BUTTON1             GPIO_PD4
#define PD4_FUNC			AS_GPIO
#define PD4_OUTPUT_ENABLE	0
#define PD4_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PD4	PM_PIN_PULLUP_10K

// I2C Sensor
#define	USE_I2C_DRV			I2C_DRV_SOFT // Soft I2C
#define	I2C_CLOCK			400000

// I2C - SCL
#define I2C_SCL 			GPIO_PC4
#define PC4_FUNC			AS_GPIO
#define PC4_INPUT_ENABLE	1
#define PC4_OUTPUT_ENABLE	0
#define PC4_DATA_OUT		0
#define PULL_WAKEUP_SRC_PC4	PM_PIN_PULLUP_10K // Board has hardware pull-ups

// I2C - SDA
#define I2C_SDA 			GPIO_PC3
#define PC3_FUNC			AS_GPIO
#define PC3_INPUT_ENABLE	1
#define PC3_OUTPUT_ENABLE	0
#define PC3_DATA_OUT		0
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K // Board has hardware pull-ups

// Sensor T&H
#define USE_SENSOR_CHT8305		0
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		1
#define USE_SENSOR_SHT4X		0
#define USE_SENSOR_SHTC3		0
#define USE_SENSOR_SHT30		0

// DISPLAY
#define	USE_DISPLAY			0

// LED
#define LED_ON				0
#define LED_OFF				1
#define GPIO_LED			GPIO_PC2
#define PC2_FUNC	  		AS_GPIO
#define PC2_OUTPUT_ENABLE	1
#define PC2_INPUT_ENABLE	1
#define PC2_DATA_OUT		LED_OFF

// VBAT
#define SHL_ADC_VBAT		C5P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PC5 // missing pin on case TLSR825x
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

#endif // BOARD == BOARD_ZG_227Z
#endif /* _BOARD_ZG_227Z_H_ */
