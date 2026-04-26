/********************************************************************************************************
 * @file    board_cb3s.h
 *
 * @brief   This is the header file for board_cb3s
 *
 *******************************************************************************************************/
#ifndef _BOARD_ZG204ZL_H_
#define _BOARD_ZG204ZL_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZG204ZL)

//#define SWS_PRINTF_MODE	1

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_PIR | SERVICE_ILLUMI)

/* https://pvvx.github.io/ZG-204ZL-3.0/

TLSR8253

GPIO_PA1 - KEY
GPIO_PA7 - SWS, (debug TX)
GPIO_PB1 - TX
GPIO_PB7 - RX
GPIO_PC4 - illuminance Sense
GPIO_PD3 - illuminance Sense Power
GPIO_PD4 - PIR
GPIO_PD7 - LED to +Vbat
*/

#define BLE_MODEL_STR		"ZG-204ZL"
#define BLE_MAN_STR			"Sonoff"

#define ZCL_BASIC_MFG_NAME     {6,'S','o','n','o','f','f'} // Sonoff
#define ZCL_BASIC_MODEL_ID	   {10,'Z','G','-','2','0','4','Z','L','-','z'} // ZG-204ZL-z

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2450

#define USE_ONOFF	1

// DISPLAY
#define	USE_DISPLAY			0

// KEY, BUTTON
#define BUTTON1				GPIO_PA1
#define BUTTON1_ON			0
#define PA1_INPUT_ENABLE	1
#define PA1_DATA_OUT		0
#define PA1_OUTPUT_ENABLE	0
#define PA1_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PA1 PM_PIN_PULLUP_10K

// LED
#define USE_BLINK_LED		1
#define LED_ON				0
#define LED_OFF				1
#define GPIO_LED			GPIO_PD7
#define PD7_FUNC	  		AS_GPIO
#define PD7_OUTPUT_ENABLE	1
#define PD7_INPUT_ENABLE	1
#define PD7_DATA_OUT		LED_OFF

// I2C
#define	USE_I2C_DRV			I2C_DRV_NONE

// PIR
#define DEF_OCCUPANCY_DELAY		35	// sec (PIR sensor)

#define GPIO_PIR			GPIO_PD4
#define PIR_ON				1
#define PD4_FUNC		AS_GPIO
#define PD4_OUTPUT_ENABLE	0
#define PD4_INPUT_ENABLE	1
//#define PULL_WAKEUP_SRC_PD4	PM_PIN_PULLDOWN_100K

// illuminance sensor
#define USE_SENSOR_LX		1
#define DEF_MIN_LEVEL_ZLX		13000 // ILLUMINANCE_LEVEL_SENSING ~20 lx
#define READ_SENSOR_TIMER_SEC 	20 // default, second

#define GPIO_ADC_ILLUMI		GPIO_PC4
#define SHL_ADC_ILLUMI		C4P // see in adc.h ADC_InputPchTypeDef
#define PC4_FUNC			AS_GPIO
#define PC4_OUTPUT_ENABLE	0
#define PC4_INPUT_ENABLE	1
//#define PULL_WAKEUP_SRC_PC4	PM_PIN_PULLDOWN_100K

#define GPIO_ILLUMI_ON		GPIO_PD3
#define ILLUMI_POEWR_ON		1
#define PD3_DATA_OUT		(!ILLUMI_POEWR_ON)
#define PD3_OUTPUT_ENABLE	0
#define PD3_INPUT_ENABLE	1
#define PD3_FUNC			AS_GPIO

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


#endif // (BOARD == BOARD_ZG204ZL)
#endif // _BOARD_ZG204ZL_H_
