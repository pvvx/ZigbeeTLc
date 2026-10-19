/********************************************************************************************************
 * @file    board_zg204zs.h
 *
 * @brief   This is the header file for Tuya / HOBEIAN ZG-204ZS and ZG-204ZM
 *          Dual PIR + Skyrelay RC2417 24GHz mmWave Presence Sensor
 *
 *******************************************************************************************************/
#ifndef _BOARD_ZG204ZS_H_
#define _BOARD_ZG204ZS_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZG204ZS)

//#define SWS_PRINTF_MODE	1

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_PIR | SERVICE_ILLUMI | SERVICE_LED)

/* Pin assignments for ZG-204ZS V5.2 / ZG-204ZM:
   TLSR8258F512ET32
   GPIO_PA1 - KEY / Button 1 (Active LOW, 10k pullup)
   GPIO_PA7 - SWS (debug TX / flash)
   GPIO_PB1 - Radar UART TX (MCU TX -> Radar R pin, 115200 8N1)
   GPIO_PB5 - Radar Power Gate (A19T P-MOSFET gate, Active LOW: 0 = Radar ON, 1 = Radar OFF)
   GPIO_PB6 - Radar OUT (Digital presence detection input, Active HIGH, 100k pulldown)
   GPIO_PB7 - Radar UART RX (MCU RX <- Radar T pin, 115200 8N1)
   GPIO_PC1 - Illuminance Sensor Power switch (Active HIGH)
   GPIO_PC4 - Illuminance Sensor ADC input (C4P)
   GPIO_PD4 - PIR sensor motion input (Active HIGH)
   GPIO_PD7 - LED to +Vbat (Active LOW)
*/

#define BLE_MODEL_STR		"ZG-204ZS"
#define BLE_MAN_STR			"Sonoff"

#define ZCL_BASIC_MFG_NAME     {6,'S','o','n','o','f','f'} // Sonoff
#define ZCL_BASIC_MODEL_ID	   {10,'Z','G','-','2','0','4','Z','S','-','z'} // ZG-204ZS-z

// Battery & RF Power
#define USE_BATTERY 	BATTERY_2AAA

#define USE_REMOTE_ONOFF	1

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
#define DEF_OCCUPANCY_DELAY		35	// sec
#define DEF_OCCUPANCY_THRES		5	// radar sensitivity (1..10)

// RC2417 Default Zone Thresholds
#define DEF_RC2417_ZONE1_M_THRES	14	// Zone 1 (0-2m) Motion threshold (0..99)
#define DEF_RC2417_ZONE1_S_THRES	2	// Zone 1 (0-2m) Still threshold (0..99) - sensitive to breathing/sitting
#define DEF_RC2417_ZONE2_M_THRES	10	// Zone 2 (2-5m) Motion threshold (0..99)
#define DEF_RC2417_ZONE2_S_THRES	4	// Zone 2 (2-5m) Still threshold (0..99)
#define DEF_RC2417_ZONE3_M_THRES	10	// Zone 3 (5-10m) Motion threshold (0..99)
#define DEF_RC2417_ZONE3_S_THRES	4	// Zone 3 (5-10m) Still threshold (0..99)
#define DEF_RC2417_TRITH			1	// Consecutive trigger scans (1..10) - 1 for instant response

#define GPIO_PIR			GPIO_PD4
#define PIR_ON				1
#define PD4_FUNC			AS_GPIO
#define PD4_OUTPUT_ENABLE	0
#define PD4_INPUT_ENABLE	1
//#define PULL_WAKEUP_SRC_PD4	PM_PIN_PULLDOWN_100K

// RC2417 mmWave Radar
#define USE_SENSOR_RC2417	1

// Radar Power Gate: PB5 driving A19T P-MOSFET gate (0 = ON, 1 = OFF)
#define RC2417_PWR_PIN		GPIO_PB5
#define PB5_FUNC			AS_GPIO
#define PB5_INPUT_ENABLE	0
#define PB5_OUTPUT_ENABLE	1
#define PB5_DATA_OUT		1 // Radar OFF by default
#define PULL_WAKEUP_SRC_PB5	PM_PIN_PULLUP_10K

// Radar Digital OUT: PB6 (1 = Presence detected, 0 = Clear)
#define RC2417_OUT_PIN		GPIO_PB6
#define PB6_FUNC			AS_GPIO
#define PB6_OUTPUT_ENABLE	0
#define PB6_INPUT_ENABLE	1
#define PULL_WAKEUP_SRC_PB6	PM_PIN_PULLDOWN_100K

// Radar UART Pins: PB1 (TX) and PB7 (RX)
#define RC2417_UART_TX_PIN	GPIO_PB1
#define PB1_FUNC			AS_GPIO
#define PB1_OUTPUT_ENABLE	1
#define PB1_INPUT_ENABLE	1
#define PB1_DATA_OUT		1
#define PULL_WAKEUP_SRC_PB1	PM_PIN_PULLUP_10K

#define RC2417_UART_RX_PIN	GPIO_PB7
#define PB7_FUNC			AS_GPIO
#define PB7_OUTPUT_ENABLE	0
#define PB7_INPUT_ENABLE	1
#define PULL_WAKEUP_SRC_PB7	PM_PIN_PULLUP_10K

// illuminance sensor
#define USE_SENSOR_LX		1 // =1 - ADC = Ur, =2 - ADC = Us, =3 ZG-223Z
//#define GPIO_ADC_PULL		PM_PIN_PULLUP_10K // external pulldown on PCB

#define ADC_LX_ZERO_DEF		0
#define ADC_LX_COEF_DEF		3000

#define DEF_MIN_LEVEL_ZLX		13000 // ILLUMINANCE_LEVEL_SENSING ~20 lx
#define READ_SENSOR_TIMER_SEC 	20 // default, second

#define GPIO_ILLUMI_ADC		GPIO_PC4
#define SHL_ADC_ILLUMI		C4P // see in adc.h ADC_InputPchTypeDef
#define PC4_FUNC			AS_GPIO
#define PC4_OUTPUT_ENABLE	0
#define PC4_INPUT_ENABLE	1

#define GPIO_ILLUMI_ON		GPIO_PC1
#define ILLUMI_POWER_ON		1
#define PC1_DATA_OUT		(!ILLUMI_POWER_ON)
#define PC1_OUTPUT_ENABLE	0
#define PC1_INPUT_ENABLE	1
#define PC1_FUNC			AS_GPIO

// VBAT
#define SHL_ADC_VBAT		C5P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PC5
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

#endif // (BOARD == BOARD_ZG204ZS)
#endif // _BOARD_ZG204ZS_H_
