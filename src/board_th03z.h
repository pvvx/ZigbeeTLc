/*
 * board_mho_th03z.h
 * Author: pvvx
 */
#ifndef _BOARD_TH03Z_H_
#define _BOARD_TH03Z_H_

#include "version_cfg.h"

#if (BOARD == BOARD_TH03Z)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_THS)

/* https://pvvx.github.io/TH03Z/

TLSR825x 1M Flash

GPIO_PA7 - SWS, free, (debug TX)
GPIO_PB1 - TX
GPIO_PB4 - LED
GPIO_PB7 - RX
GPIO_PC0 - KEY
GPIO_PC2 - SDA
GPIO_PC3 - SCL
GPIO_PD7 - Alert
*/

#define BLE_MODEL_STR		"TH03Z"
#define BLE_MAN_STR			"Tuya"

#define ZCL_BASIC_MFG_NAME     {4,'T','u','y','a'} // Tuya
#define ZCL_BASIC_MODEL_ID	   {7,'T','H','0','3','Z','-','z'} // TH03Z


// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2450

// DISPLAY
#define	USE_DISPLAY			0

// BUTTON
#define BUTTON1             GPIO_PC0
#define BUTTON1_ON			0
#define BUTTON1_OFF			1
#define PC0_FUNC			AS_GPIO
#define PC0_OUTPUT_ENABLE	0
#define PC0_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PC0	PM_PIN_PULLUP_10K

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
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		0
#define USE_SENSOR_SHT4X		0
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

#endif // BOARD == BOARD_TH03Z
#endif /* _BOARD_TH03Z_H_ */
