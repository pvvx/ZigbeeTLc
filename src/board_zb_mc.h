/*
 * board_zbeacon_th01.h
 * Author: 
 */
#ifndef _BOARD_ZB_MC_H_
#define _BOARD_ZB_MC_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZB_MC)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_THS)

/*
ZTU Tuya module
GPIO_A7 - SWS
GPIO_B1 - TX
GPIO_B4 - LED, On: "1"
GPIO_C0 - SCL
GPIO_C3 - ALERT
GPIO_B7 - RX
GPIO_D3 - KEY, On: "0"
GPIO_D7 - SDA
*/

#define BLE_MODEL_STR		"MC"
#define BLE_MAN_STR			"ZBeacon"

#define ZCL_BASIC_MFG_NAME     {7,'Z','B','e','a','c','o','n'} // ZBeacon
#define ZCL_BASIC_MODEL_ID	   {4,'M','C','-','z'} // MC-z

// Battery & RF Power
#define USE_BATTERY 	BATTERY_2AAA

// DISPLAY
#define	USE_DISPLAY			0

// BUTTON
#define BUTTON1             GPIO_PD3
#define BUTTON1_ON			0
#define BUTTON1_OFF			1
#define PD3_FUNC			AS_GPIO
#define PD3_OUTPUT_ENABLE	0
#define PD3_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PD3	PM_PIN_PULLUP_10K

// I2C Sensor
#define	USE_I2C_DRV			I2C_DRV_SOFT // Soft I2C
#define	I2C_CLOCK			400000

// I2C - SCL
#define I2C_SCL 			GPIO_PC0
#define PC0_FUNC			AS_GPIO
#define PC0_INPUT_ENABLE	1
#define PC0_OUTPUT_ENABLE	0
#define PC0_DATA_OUT		0
#define PULL_WAKEUP_SRC_PC0	PM_PIN_PULLUP_10K // Board has hardware pull-ups

// I2C - SDA
#define I2C_SDA 			GPIO_PD7
#define PD7_FUNC			AS_GPIO
#define PD7_INPUT_ENABLE	1
#define PD7_OUTPUT_ENABLE	0
#define PD7_DATA_OUT		0
#define PULL_WAKEUP_SRC_PD7	PM_PIN_PULLUP_10K // Board has hardware pull-ups

// Sensor T&H
#define USE_SENSOR_CHT8305		1
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		1
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		0
#define USE_SENSOR_SHT30		1

// LED
#define LED_ON				1
#define LED_OFF				0
#define GPIO_LED			GPIO_PB4
#define PB4_FUNC	  		AS_GPIO
#define PB4_OUTPUT_ENABLE	1
#define PB4_INPUT_ENABLE	1
#define PB4_DATA_OUT		LED_OFF

// VBAT
#define SHL_ADC_VBAT		B0P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PB0 // missing pin on case TLSR825x
#define PB0_INPUT_ENABLE	0
#define PB0_DATA_OUT		1
#define PB0_OUTPUT_ENABLE	1
#define PB0_FUNC			AS_GPIO

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif

#endif // BOARD == BOARD_ZB_MC
#endif /* _BOARD_ZB_MC_H_ */

