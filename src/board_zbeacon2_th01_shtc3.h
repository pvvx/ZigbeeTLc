/*
 * board_zbeacon2_th01_shtc3.h
 * ZBEACON-TH01 v2.0 PCB variant with SHTC3
 */
#ifndef _BOARD_ZBEACON2_TH01_SHTC3_H_
#define _BOARD_ZBEACON2_TH01_SHTC3_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZBEACON2_TH01_SHTC3)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_THS | SERVICE_LED)

/* TLSR8656F512ET32
 * ZBEACON-TH01 v2.0 PCB variant, also sold as TS0201_TZ3000_rdhukkmi
 * https://github.com/pvvx/ZigbeeTLc/issues/211
 * Differences from BOARD_ZBEACON2_TH01: SHTC3 sensor, SDA/SCL swapped, no TX pad
GPIO_A0 - SCL
GPIO_A7 - SWS
GPIO_B6 - LED
GPIO_B7 - KEY
GPIO_D4 - SDA
 */

#define BLE_MODEL_STR		"TH01"
#define BLE_MAN_STR		"ZBeacon"

#define ZCL_BASIC_MFG_NAME     {7,'Z','B','e','a','c','o','n'}
#define ZCL_BASIC_MODEL_ID     {8,'T','H','0','1','-','2','-','z'}

// Battery & RF Power
#define USE_BATTERY	BATTERY_2AAA

// DISPLAY
#define USE_DISPLAY	0

// BUTTON
#define BUTTON1		GPIO_PB7
#define BUTTON1_ON		0
#define PB7_FUNC		AS_GPIO
#define PB7_OUTPUT_ENABLE	0
#define PB7_INPUT_ENABLE	1
#define PULL_WAKEUP_SRC_PB7	PM_PIN_PULLUP_10K

// I2C Sensor
#define USE_I2C_DRV		I2C_DRV_SOFT
#define I2C_CLOCK		400000

// I2C - SCL
#define I2C_SCL		GPIO_PA0
#define PA0_FUNC		AS_GPIO
#define PA0_INPUT_ENABLE	1
#define PA0_OUTPUT_ENABLE	0
#define PA0_DATA_OUT		0
#define PULL_WAKEUP_SRC_PA0	PM_PIN_PULLUP_10K

// I2C - SDA
#define I2C_SDA		GPIO_PD4
#define PD4_FUNC		AS_GPIO
#define PD4_INPUT_ENABLE	1
#define PD4_OUTPUT_ENABLE	0
#define PD4_DATA_OUT		0
#define PULL_WAKEUP_SRC_PD4	PM_PIN_PULLUP_10K

// Sensor T&H
#define USE_SENSOR_CHT8305	0
#define USE_SENSOR_CHT8215	0
#define USE_SENSOR_AHT20_30	0
#define USE_SENSOR_SHT4X	0
#define USE_SENSOR_SHTC3	1
#define USE_SENSOR_SHT30	0

// LED
#define LED_ON			1
#define LED_OFF		0
#define GPIO_LED		GPIO_PB6
#define PB6_FUNC		AS_GPIO
#define PB6_OUTPUT_ENABLE	1
#define PB6_INPUT_ENABLE	1
#define PB6_DATA_OUT		LED_OFF

// VBAT
#define SHL_ADC_VBAT		B0P
#define GPIO_VBAT		GPIO_PB0
#define PB0_INPUT_ENABLE	0
#define PB0_DATA_OUT		1
#define PB0_OUTPUT_ENABLE	1
#define PB0_FUNC		AS_GPIO

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define DEBUG_INFO_TX_PIN GPIO_SWS
#endif

#endif // BOARD == BOARD_ZBEACON2_TH01_SHTC3
#endif /* _BOARD_ZBEACON2_TH01_SHTC3_H_ */
