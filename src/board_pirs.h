/*
 * board_pirs.h
 * Author: pvvx
 */
#ifndef _BOARD_PIRS_H_
#define _BOARD_PIRS_H_

#include "version_cfg.h"

#if (BOARD == BOARD_TS0202_PIR1)

#define ZIGBEE_TUYA_OTA 	1

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_PIR)

/*
TLSR8258 1024/512K Flash

GPIO_PA7 - SWS, free, (debug TX)
GPIO_PB4 - LED (to GND)
GPIO_PC0 - KEY (to GND)
GPIO_PD7 - PIR BS612 sens
GPIO_PA0 - PIR R
GPIO_PA1 - PIR R
GPIO_PD3 - PIR R
*/

#define BLE_MODEL_STR		"TS202PIR1"
#define BLE_MAN_STR			"Tuya"

#define ZCL_BASIC_MFG_NAME     {4,'T','u','y','a'} // Tuya
#define ZCL_BASIC_MODEL_ID	   {11,'T','S','2','0','2','P','I','R','1','-','z'} // TS202PIR1-z

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2450

// DISPLAY
#define	USE_DISPLAY			0

// BUTTON
#define BUTTON1             GPIO_PB1
#define BUTTON1_ON			0
#define BUTTON1_OFF			1
#define PB1_FUNC			AS_GPIO
#define PB1_OUTPUT_ENABLE	0
#define PB1_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PB1	PM_PIN_PULLUP_10K

// I2C
#define	USE_I2C_DRV			I2C_DRV_NONE

// LED
#define GPIO_LED			GPIO_PB4
#define LED_ON				1
#define LED_OFF				0
#define PB4_FUNC	  		AS_GPIO
#define PB4_OUTPUT_ENABLE	1
#define PB4_INPUT_ENABLE	1
#define PB4_DATA_OUT		LED_OFF

// PIR
#define GPIO_PIR			GPIO_PD7
#define PIR_ON				1
#define PIR_OFF				0
#define PD7_FUNC		AS_GPIO
#define PD7_OUTPUT_ENABLE	0
#define PD7_INPUT_ENABLE	1
#define PULL_WAKEUP_SRC_PD7	PM_PIN_PULLDOWN_100K

#define PA0_FUNC		AS_GPIO
#define PA0_DATA_OUT		0
#define PA0_OUTPUT_ENABLE	1
#define PA0_INPUT_ENABLE	0
#define PULL_WAKEUP_SRC_PA0	PM_PIN_PULLDOWN_100K

#define PA1_FUNC		AS_GPIO
#define PA1_DATA_OUT		0
#define PA1_OUTPUT_ENABLE	1
#define PA1_INPUT_ENABLE	0
#define PULL_WAKEUP_SRC_PA1	PM_PIN_PULLDOWN_100K

#define PD3_FUNC		AS_GPIO
#define PD3_DATA_OUT		0
#define PD3_OUTPUT_ENABLE	1
#define PD3_INPUT_ENABLE	0
#define PULL_WAKEUP_SRC_PD3	PM_PIN_PULLDOWN_100K

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

#elif (BOARD == BOARD_TS0202_PIR2)

/*
TLSR8258 1024/512K Flash

GPIO_PA7 - SWS, free, (debug TX)
GPIO_PB4 - LED (to GND)
GPIO_PC2 - PIR BS612 sens
GPIO_PC3 - KEY (to GND)
*/

#define BLE_MODEL_STR		"TS202PIR2"
#define BLE_MAN_STR			"Tuya"

#define ZCL_BASIC_MFG_NAME     {4,'T','u','y','a'} // Tuya
#define ZCL_BASIC_MODEL_ID	   {11,'T','S','2','0','2','P','I','R','2','-','z'} // TS202PIR2-z

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2450

// DISPLAY
#define	USE_DISPLAY			0

// BUTTON
#define BUTTON1             GPIO_PC3
#define BUTTON1_ON			0
#define BUTTON1_OFF			1
#define PC3_FUNC			AS_GPIO
#define PC3_OUTPUT_ENABLE	0
#define PC3_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

// I2C
#define	USE_I2C_DRV			I2C_DRV_NONE

// LED
#define GPIO_LED			GPIO_PB4
#define LED_ON				1
#define LED_OFF				0
#define PB4_FUNC	  		AS_GPIO
#define PB4_OUTPUT_ENABLE	1
#define PB4_INPUT_ENABLE	1
#define PB4_DATA_OUT		LED_OFF

// PIR
#define GPIO_PIR			GPIO_PC2
#define PIR_ON				1
#define PIR_OFF				0
#define PC2_FUNC		AS_GPIO
#define PC2_OUTPUT_ENABLE	0
#define PC2_INPUT_ENABLE	1
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLDOWN_100K

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

#endif // BOARD == BOARD_TS0202_PIRx
#endif /* _BOARD_PIRS_H_ */
