/********************************************************************************************************
 * @file    board_cb3s.h
 *
 * @brief   This is the header file for board_cb3s
 *
 *******************************************************************************************************/
#ifndef _BOARD_ZG204ZL_H_
#define _BOARD_ZG204ZL_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZG204ZV)

//#define SWS_PRINTF_MODE	1

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_LED | SERVICE_OTA | SERVICE_THS | SERVICE_PIR | SERVICE_ILLUMI)

/* https://pvvx.github.io/ZG-204ZL-3.0/
 TLSR8253F512ET32 512K Flash
 GPIO_PA7 - SWS, free, (debug TX)
 GPIO_PB1 - TX
 GPIO_PC2 - LED (+Vbat)
 GPIO_PB5 - R lx
 GPIO_PB6 - R lx
 GPIO_PB7 - RX
 GPIO_PD4 - KEY (GND)
 GPIO_PC3 - SDA
 GPIO_PC4 - SCL
 GPIO_PD7 - XBR818 OUT
 GPIO_PA0 - XBR818 SCL
 GPIO_PA1 - XBR818 SDA
*/

#define BLE_MODEL_STR		"ZG-204ZV"
#define BLE_MAN_STR			"Sonoff"

#define ZCL_BASIC_MFG_NAME     {6,'S','o','n','o','f','f'} // Sonoff
#define ZCL_BASIC_MODEL_ID	   {10,'Z','G','-','2','0','4','Z','V','-','z'} // ZG-204ZV-z

// Battery & RF Power
#define USE_BATTERY 	BATTERY_2AAA

#define USE_REMOTE_ONOFF	1

// DISPLAY
#define	USE_DISPLAY			0

#define USE_SENSOR_CHT8305		0
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		1
#define USE_SENSOR_SHT4X		0
#define USE_SENSOR_SHTC3		0
#define USE_SENSOR_SHT30		0

// KEY, BUTTON
#define BUTTON1				GPIO_PD4
#define PD4_INPUT_ENABLE	1
#define PD4_DATA_OUT		0
#define PD4_OUTPUT_ENABLE	0
#define PD4_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PD4	PM_PIN_PULLUP_10K

// LED
#define USE_BLINK_LED		1
#define LED_ON				0
#define LED_OFF				(!LED_ON)
#define GPIO_LED			GPIO_PC2
#define PC2_INPUT_ENABLE	1
#define PC2_DATA_OUT		1
#define PC2_OUTPUT_ENABLE	LED_OFF
#define PC2_FUNC			AS_GPIO

// I2C
#define	USE_I2C_DRV			I2C_DRV_SOFT
#define I2C_CLOCK 			400000 // 400 kHz
#define I2C_SCL 			GPIO_PC4
#define PC4_INPUT_ENABLE	1
#define PC4_DATA_OUT		0
#define PC4_OUTPUT_ENABLE	0
#define PULL_WAKEUP_SRC_PC4	PM_PIN_PULLUP_10K

#define I2C_SDA 			GPIO_PC3
#define PC3_INPUT_ENABLE	1
#define PC3_DATA_OUT		0
#define PC3_OUTPUT_ENABLE	0
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

// PIR
#define DEF_OCCUPANCY_DELAY		35	// sec (PIR sensor)

#define PIR_ON				1
#define GPIO_PIR			GPIO_PD7
#define PD7_INPUT_ENABLE	1
#define PD7_DATA_OUT		0
#define PD7_OUTPUT_ENABLE	0
#define PD7_FUNC			AS_GPIO

#define USE_SENSOR_XBR818	1

#define XBR818_SCL			GPIO_PA0
#define PA0_INPUT_ENABLE	1
#define PA0_DATA_OUT		0
#define PA0_OUTPUT_ENABLE	0
#define PA0_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PA0 PM_PIN_PULLUP_10K

#define XBR818_SDA			GPIO_PA1
#define PA1_INPUT_ENABLE	1
#define PA1_DATA_OUT		0
#define PA1_OUTPUT_ENABLE	0
#define PA1_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PA1 PM_PIN_PULLUP_10K

#define XBR818_OUT			GPIO_PD7

// illuminance sensor
#define USE_SENSOR_LX		2 // =1 - ADC = Ur, =2 - ADC = Us
#define GPIO_ADC_PULL		PM_PIN_PULLUP_10K

#define DEF_MIN_LEVEL_ZLX		13000 // ILLUMINANCE_LEVEL_SENSING ~20 lx
#define READ_SENSOR_TIMER_SEC 	15 // default, second

#define GPIO_ADC_ILLUMI		GPIO_PB5
#define SHL_ADC_ILLUMI		B5P // see in adc.h ADC_InputPchTypeDef

#define GPIO_ILLUMI_ON		GPIO_PB6
#define ILLUMI_POEWR_ON		0
#define PB6_DATA_OUT		ILLUMI_POEWR_ON
#define PB6_OUTPUT_ENABLE	0
#define PB6_INPUT_ENABLE	1
#define PB6_FUNC			AS_GPIO

// VBAT
#define SHL_ADC_VBAT		1  // "B0P" in adc.h
#define GPIO_VBAT			GPIO_PB0 // missing pin on case TLSR8253F512ET32
#define PB0_INPUT_ENABLE	1
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


#endif // (BOARD == BOARD_ZG204ZL)
#endif // _BOARD_ZG204ZL_H_
