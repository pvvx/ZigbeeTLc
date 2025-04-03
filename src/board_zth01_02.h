/*
 * board_mho_zth01_02.h
 * Author: pvvx
 */
#ifndef _BOARD_ZTH01_02_H_
#define _BOARD_ZTH01_02_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZTH01 || BOARD == BOARD_ZTH02)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_THS)

/* https://pvvx.github.io/TS0601_TZE200_zth01

TLSR825x 1M Flash

GPIO_PB1 - TX
GPIO_PB4 - KEY
GPIO_PB5 - LED
GPIO_PB7 - RX
GPIO_PC2 - SDA
GPIO_PC3 - SCL
*/

#define BLE_MAN_STR			"Tuya"

#if (BOARD == BOARD_ZTH01)
 
#define BLE_MODEL_STR		"ZTH01"
#define ZCL_BASIC_MFG_NAME     {4,'T','u','y','a'} // Tuya
#define ZCL_BASIC_MODEL_ID	   {7,'Z','T','H','0','1','-','z'} // ZTH01

#elif (BOARD == BOARD_ZTH02)

#define BLE_MODEL_STR		"ZTH02"
#define ZCL_BASIC_MFG_NAME     {4,'T','u','y','a'} // Tuya
#define ZCL_BASIC_MODEL_ID	   {7,'Z','T','H','0','2','-','z'} // ZTH02

#else
#error "BOARD ?"
#endif

// Battery & RF Power
#define USE_BATTERY 	BATTERY_2AAA

// DISPLAY
#define	USE_DISPLAY			0

// VBAT
#define SHL_ADC_VBAT		B0P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PB0 // missing pin on case TLSR825x
#define PB0_INPUT_ENABLE	0
#define PB0_DATA_OUT		1
#define PB0_OUTPUT_ENABLE	1
#define PB0_FUNC			AS_GPIO

// BUTTON
#define BUTTON1             GPIO_PB4
#define BUTTON1_ON			0
#define BUTTON1_OFF			1
#define PB4_FUNC			AS_GPIO
#define PB4_OUTPUT_ENABLE	0
#define PB4_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PB4	PM_PIN_PULLUP_1M

// LED
#define LED_ON				1
#define LED_OFF				0
#define GPIO_LED			GPIO_PB5
#define PB5_FUNC	  		AS_GPIO
#define PB5_OUTPUT_ENABLE	1
#define PB5_INPUT_ENABLE	1
#define PB5_DATA_OUT		LED_OFF

// I2C Sensor
#define	USE_I2C_DRV			I2C_DRV_HARD
#define I2C_CLOCK			400000 // Hz
#define I2C_SCL 			GPIO_PC2
#define I2C_SDA 			GPIO_PC3
#define I2C_GROUP 			I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

// Sensor T&H
#define USE_SENSOR_CHT8305		0
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		1
#define USE_SENSOR_SHT4X		0
#define USE_SENSOR_SHTC3		0
#define USE_SENSOR_SHT30		0

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif

#endif // BOARD == BOARD_ZTH0X
#endif /* _BOARD_ZTH01_02_H_ */
