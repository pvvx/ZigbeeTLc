/*
 * board_zbeacon_th01.h
 * Author: 
 */
#ifndef _BOARD_ZBEACON_TH01_H_
#define _BOARD_ZBEACON_TH01_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZBEACON_TH01)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_THS)

/* https://pvvx.github.io/ZBEACON-TH01/ | https://pvvx.github.io/ZBEACON-TH01/

TLSR8250F512ET32 512K Flash (BT3L Tuya module)

GPIO_A0 - SDA
GPIO_A7 - SWS
GPIO_B1 - TX
GPIO_B4 - SCL
GPIO_B5 - KEY
GPIO_B7 - RX
GPIO_C3 - LED

*/

#define BLE_MODEL_STR		"TH01"
#define BLE_MAN_STR			"ZBeacon"

#define ZCL_BASIC_MFG_NAME     {7,'Z','B','e','a','c','o','n'} // ZBeacon
#define ZCL_BASIC_MODEL_ID	   {6,'T','H','0','1','-','z'} // TH01-z

// Battery & RF Power
#define USE_BATTERY 	BATTERY_2AAA

// DISPLAY
#define	USE_DISPLAY			0

// BUTTON
#define BUTTON1             GPIO_PB5
#define BUTTON1_ON			0
#define BUTTON1_OFF			1
#define PB5_FUNC			AS_GPIO
#define PB5_OUTPUT_ENABLE	0
#define PB5_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PB5	PM_PIN_PULLUP_10K

// I2C Sensor
#define	USE_I2C_DRV			I2C_DRV_SOFT // Soft I2C
#define	I2C_CLOCK			400000

// I2C - SCL
#define I2C_SCL 			GPIO_PB4
#define PB4_FUNC			AS_GPIO
#define PB4_INPUT_ENABLE	1
#define PB4_OUTPUT_ENABLE	0
#define PB4_DATA_OUT		0
#define PULL_WAKEUP_SRC_PB4	PM_PIN_PULLUP_10K // Board has hardware pull-ups

// I2C - SDA
#define I2C_SDA 			GPIO_PA0
#define PA0_FUNC			AS_GPIO
#define PA0_INPUT_ENABLE	1
#define PA0_OUTPUT_ENABLE	0
#define PA0_DATA_OUT		0
#define PULL_WAKEUP_SRC_PA0	PM_PIN_PULLUP_10K // Board has hardware pull-ups

// Sensor T&H
#define USE_SENSOR_CHT8305		0
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		0
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		0
#define USE_SENSOR_SHT30		0


// LED
#define LED_ON				1
#define LED_OFF				0
#define GPIO_LED			GPIO_PC3
#define PC3_FUNC	  		AS_GPIO
#define PC3_OUTPUT_ENABLE	1
#define PC3_INPUT_ENABLE	1
#define PC3_DATA_OUT		LED_OFF

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

#endif // BOARD == BOARD_ZBEACON_TH01
#endif /* _BOARD_ZBEACON_TH01_H_ */
