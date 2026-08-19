/********************************************************************************************************
 * @file    board_zg223z.h
 *
 * @brief   This is the header file for board_cb3s
 *
 *******************************************************************************************************/
#ifndef _BOARD_ZG223Z_H_
#define _BOARD_ZG223Z_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZG223Z)


#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_THS | SERVICE_ILLUMI | SERVICE_RNDS | SERVICE_LED)

/* https://pvvx.github.io/ZG-223Z/

TLSR8253 512K

PC0 +LXsensor
PC1 LED to R to +BAT
PC2 Button to GND
PC3 CB2401 pin 6: RXEN
PC4 CB2401 pin 5: TXEN
PB4 R 10k to PB5
PB5 R 10k -> Raindrop-sensor to GND
PB6 -LXsensor + R 50k to GND
*/

#define BLE_MODEL_STR		"ZG-223Z"
#define BLE_MAN_STR			"HOBELAN"

#define ZCL_BASIC_MFG_NAME     {7,'H','O','B','E','L','A','N'} // HOBELAN
#define ZCL_BASIC_MODEL_ID	   {9,'Z','G','-','2','2','3','Z','-','z'} // ZG-223Z

#define USE_REMOTE_ONOFF		1
#define NOT_INPYT_ONOFF			1

#define USE_SOC_TEMP_SENSOR		1
#define READ_SENSOR_TIMER_SEC 	30 // default, second
#define ZCL_DIHUMIDIFICATION_CONTROL_SUPPORT	1

/* PA */
#define PA_ENABLE           1
#define PA_RX               GPIO_PC3
#define PA_TX               GPIO_PC4
#define PC3_DATA_OUT		0
#define PC3_OUTPUT_ENABLE	1
#define PC4_DATA_OUT		0
#define PC4_OUTPUT_ENABLE	1
#define PULL_WAKEUP_SRC_PC4 PM_PIN_PULLDOWN_100K
#define PULL_WAKEUP_SRC_PC3 PM_PIN_PULLDOWN_100K

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR123A

// DISPLAY
#define	USE_DISPLAY			0

// KEY, BUTTON
#define BUTTON1				GPIO_PC2
#define BUTTON1_ON			0
#define PC2_INPUT_ENABLE	1
#define PC2_DATA_OUT		0
#define PC2_OUTPUT_ENABLE	0
#define PC2_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC2 PM_PIN_PULLUP_10K

// LED
#define USE_BLINK_LED		1
#define LED_ON				0
#define LED_OFF				1
#define GPIO_LED			GPIO_PC1
#define PC1_FUNC	  		AS_GPIO
#define PC1_OUTPUT_ENABLE	1
#define PC1_INPUT_ENABLE	1
#define PC1_DATA_OUT		LED_OFF

// I2C
#define	USE_I2C_DRV			I2C_DRV_NONE

// Raindrop sensor
#define USE_SENSOR_RND		1 // =1 - ADC = Ur, =2 - ADC = Us
#define DEF_MIN_LEVEL_RND	13000 // RND (RH) = 0

#define GPIO_RNDS			GPIO_PB4
#define SHL_ADC_RND			B5P // see in adc.h ADC_InputPchTypeDef
#define PB4_FUNC			AS_GPIO
#define PB4_OUTPUT_ENABLE	0
#define PB4_INPUT_ENABLE	1
#define PULL_WAKEUP_SRC_PB4	PM_PIN_PULLUP_1M

// Resistor to Raindrop sensor - not used!
/*
#define GPIO_RNDR			GPIO_PB5
#define RND_ON				1
#define PB5_FUNC			AS_GPIO
#define PB5_OUTPUT_ENABLE	0
#define PB5_INPUT_ENABLE	1
*/

// illuminance sensor
#define USE_SENSOR_LX		3 // =1 - ADC = Ur, =2 - ADC = Us, =3 ZG-223Z

#define ADC_LX_ZERO_DEF	0
#define ADC_LX_COEF_DEF	5000  // = max 5000 lx

#define DEF_MIN_LEVEL_ZLX	13000 // ILLUMINANCE_LEVEL_SENSING ~20 lx

#define GPIO_ILLUMI_ADC		GPIO_PB6
#define SHL_ADC_ILLUMI		B6P // see in adc.h ADC_InputPchTypeDef
#define PB6_FUNC			AS_GPIO
#define PB6_OUTPUT_ENABLE	0
#define PB6_INPUT_ENABLE	1
//#define PULL_WAKEUP_SRC_PC4	PM_PIN_PULLDOWN_100K

#define GPIO_ILLUMI_ON		GPIO_PC0
#define ILLUMI_POWER_ON		1
#define PC0_DATA_OUT		(!ILLUMI_POWER_ON)
#define PC0_OUTPUT_ENABLE	0
#define PC0_INPUT_ENABLE	1
#define PC0_FUNC			AS_GPIO

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


#endif // (BOARD == BOARD_ZG223Z)
#endif // _BOARD_ZG223Z_H_
