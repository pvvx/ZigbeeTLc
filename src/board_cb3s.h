/********************************************************************************************************
 * @file    board_cb3s.h
 *
 * @brief   This is the header file for board_cb3s
 *
 *******************************************************************************************************/
#ifndef _BOARD_CB3S_H_
#define _BOARD_CB3S_H_

#include "version_cfg.h"

#if (BOARD == BOARD_CB3S)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_THS)

/* https://pvvx.github.io/TS0041_TZ3000_fa9mlvja/

TLSR8258

GPIO_PA0 - free
GPIO_PA1 - free
GPIO_PA7 - SWS, (debug TX)
GPIO_PB1 - TX
GPIO_PB4 - free
GPIO_PB5 - free
GPIO_PB6 - free
GPIO_PB7 - RX
GPIO_PC0 - free
GPIO_PC1 - free
GPIO_PC2 - KEY
GPIO_PC3 - free
GPIO_PC4 - LED
GPIO_PD2 - free
GPIO_PD3 - free
GPIO_PD4 - free
GPIO_PD7 - free
*/

#define BLE_MODEL_STR		"CB3S"
#define BLE_MAN_STR			"Tuya"

#define ZCL_BASIC_MFG_NAME     {4,'T','u','y','a'} // Tuya
#define ZCL_BASIC_MODEL_ID	   {6,'C','B','3','S','-','z'} // CB3S-z

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2032

// DISPLAY
#define	USE_DISPLAY			0

// KEY, BUTTON
#define BUTTON1				GPIO_PC2
#define PC2_INPUT_ENABLE	1
#define PC2_DATA_OUT		0
#define PC2_OUTPUT_ENABLE	0
#define PC2_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PD4 PM_PIN_UP_DOWN_FLOAT

// LED
#define USE_BLINK_LED		1
#define LED_ON				1
#define LED_OFF				0
#define GPIO_LED			GPIO_PC4
#define PC4_FUNC	  		AS_GPIO
#define PC4_OUTPUT_ENABLE	1
#define PC4_INPUT_ENABLE	1
#define PC4_DATA_OUT		LED_OFF

// I2C Sensors
#define	USE_I2C_DRV			I2C_DRV_NONE

// VBAT
#define SHL_ADC_VBAT		C5P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PC5 // missing pin on case TLSR8251F512ET24
#define PC5_INPUT_ENABLE	0
#define PC5_DATA_OUT		1
#define PC5_OUTPUT_ENABLE	1
#define PC5_FUNC			AS_GPIO

// Sensor T&H
#define USE_SENSOR_CHT8305		1
#define USE_SENSOR_CHT8215		1
#define USE_SENSOR_AHT20_30		1
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		1
#define USE_SENSOR_SHT30		1

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif

#endif // (BOARD == BOARD_CB3S)
#endif // _BOARD_CB3S_H_
