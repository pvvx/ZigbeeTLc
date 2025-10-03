/*
 * board_zg303z.h
 * Author: pvvx
 */
#ifndef _BOARD_ZG303Z_H_
#define _BOARD_ZG303Z_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZG303Z)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_THS | SERVICE_PLM)

/*TLSR8258
 GPIO_PA0 - LED
 GPIO_PA7 - J1 pin3, SWS
 GPIO_PB1 - J1 pin4, TX
 GPIO_PB4 - PWM
 GPIO_PB5 - RH_IN
 GPIO_PB7 - J1 pin5, RX
 GPIO_PC0 - I2C_SDA
 GPIO_PC1 - I2C_SCK
 GPIO_PD4 - KEY
*/

#define BLE_MODEL_STR		"ZG-303Z"
#define BLE_MAN_STR			"Tuya"

#define ZCL_BASIC_MFG_NAME     {6,'S','o','n','o','f','f'} // Sonoff
#define ZCL_BASIC_MODEL_ID	   {9,'Z','G','-','3','0','3','Z','-','z'} // ZY-ZG-303Z-z

// Battery & RF Power
#define USE_BATTERY 	BATTERY_2AAA

// DISPLAY
#define	USE_DISPLAY			0

// BUTTON
#define BUTTON1             GPIO_PD4
#define BUTTON1_ON			0
#define BUTTON1_OFF			1
#define PD4_FUNC			AS_GPIO
#define PD4_OUTPUT_ENABLE	0
#define PD4_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PD4	PM_PIN_PULLUP_10K // Board has hardware pull-ups

// I2C Sensor
// Warning: SCL and SDA markings are swapped on the PCB
#define I2C_GROUP 			I2C_GPIO_GROUP_C0C1
#define	USE_I2C_DRV			I2C_DRV_HARD
#define	I2C_CLOCK			400000 // 400 kHz

// Sensor T&H
#define USE_SENSOR_CHT8305		0
#define USE_SENSOR_CHT8215		1
#define USE_SENSOR_AHT20_30		1
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		0
#define USE_SENSOR_SHT30		1

// LED
//#define USE_BLINK_LED		1 // Debug
#define LED_ON				0
#define LED_OFF				1
#define GPIO_LED			GPIO_PA0
#define PA0_FUNC	  		AS_GPIO
#define PA0_OUTPUT_ENABLE	1
#define PA0_INPUT_ENABLE	1
#define PA0_DATA_OUT		LED_OFF

// VBAT
#define SHL_ADC_VBAT		C5P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PC5 // missing pin on case TLSR825x
#define PC5_INPUT_ENABLE	0
#define PC5_DATA_OUT		1
#define PC5_OUTPUT_ENABLE	1
#define PC5_FUNC			AS_GPIO

#define USE_AVERAGE_RH_SHL	2
// PWM
#define PWM_PIN			GPIO_PB4
#define AS_PWMx			AS_PWM4
#define PWM_ID			PWM4_ID
// ADC
#define GPIO_RHI		GPIO_PB5
#define CHNL_RHI		6 //B5P

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif

#endif // BOARD == BOARD_ZG303Z
#endif /* _BOARD_ZG303Z_H_ */
