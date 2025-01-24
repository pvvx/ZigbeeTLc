/*
 * board_ts0201_tz3000.h
 *
 *      Author: pvvx
 */
#ifndef _BOARD_ZYZTH02_H_
#define _BOARD_ZYZTH02_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZYZTH02)

#define ZIGBEE_TUYA_OTA 	1

#define RF_TX_POWER_DEF		RF_POWER_INDEX_P3p01dBm

// TLSR825x 1M Flash
// GPIO_PA7 - SWS, free, (debug TX)
// GPIO_PB1 - TX
// GPIO_PB4 - LED (to GND)
// GPIO_PB7 - RX
// GPIO_PC0 - KEY (to GND)
// GPIO_PC2 - SDA, used I2C
// GPIO_PC3 - SCL, used I2C
// GPIO_PD7 - ALERT (CHT8305)

// BUTTON
#define BUTTON1             GPIO_PB4
#define PB4_FUNC			AS_GPIO
#define PB4_OUTPUT_ENABLE	0
#define PB4_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PB4	PM_PIN_PULLUP_1M // Board has hardware pull-ups

// I2C
// Warning: SCL and SDA markings are swapped on the PCB
//#define I2C_GROUP 	I2C_GPIO_GROUP_C2C3
#define	USE_I2C_DRV			I2C_DRV_SOFT // Soft I2C
#define	I2C_CLOCK			400000

// I2C - SCL
#define I2C_SCL 			GPIO_PC3
#define PC3_FUNC			AS_GPIO
#define PC3_INPUT_ENABLE	1
#define PC3_OUTPUT_ENABLE	0
#define PC3_DATA_OUT		0
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K // Board has hardware pull-ups

// I2C - SDA
#define I2C_SDA 			GPIO_PD2
#define PD2_FUNC			AS_GPIO
#define PD2_INPUT_ENABLE	1
#define PD2_OUTPUT_ENABLE	0
#define PD2_DATA_OUT		0
#define PULL_WAKEUP_SRC_PD2	PM_PIN_PULLUP_10K // Board has hardware pull-ups

// Sensor T&H
#define USE_SENSOR_CHT8305		0
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		0
#define USE_SENSOR_SHT4X		0
#define USE_SENSOR_SHTC3		0
#define USE_SENSOR_SHT30		1

// DISPLAY
#define	USE_DISPLAY			0

// LED
//#define USE_BLINK_LED		1 // Debug
#define LED_ON				1
#define LED_OFF				0
#define GPIO_LED			GPIO_PC2
#define PC2_FUNC	  		AS_GPIO
#define PC2_OUTPUT_ENABLE	1
#define PC2_INPUT_ENABLE	1
#define PC2_DATA_OUT		LED_OFF

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

#endif // BOARD == BOARD_ZYZTH02
#endif /* _BOARD_ZYZTH02_H_ */
