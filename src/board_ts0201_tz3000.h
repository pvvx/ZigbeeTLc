/*
 * board_ts0201_tz3000.h
 *
 *      Author: pvvx
 */
#ifndef _BOARD_TS0201_TZ3000_H_
#define _BOARD_TS0201_TZ3000_H_

#include "version_cfg.h"

#if (BOARD == BOARD_TS0201_TZ3000)

#define ZIGBEE_TUYA_OTA 	1

#define RF_TX_POWER_DEF		RF_POWER_INDEX_P3p01dBm

// TLSR825x 1M Flash
// GPIO_PA7 - SWS, free, (debug TX)
// GPIO_PB4 - LED (to GND)
// GPIO_PC0 - KEY (to GND)
// GPIO_PC2 - SDA, used I2C
// GPIO_PC3 - SCL, used I2C
// GPIO_PD7 - ALERT (CHT8305)

// BUTTON
#define BUTTON1             GPIO_PC0
#define PC0_FUNC			AS_GPIO
#define PC0_OUTPUT_ENABLE	0
#define PC0_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PC0	PM_PIN_PULLUP_10K

// I2C
#define I2C_CLOCK	400000 // Hz
#define SENSOR_TYPE SENSOR_CHT8305
#define USE_SENSOR_ID	0

#define I2C_SCL 	GPIO_PC2
#define I2C_SDA 	GPIO_PC3
#define I2C_GROUP 	I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

// DISPLAY
#define	USE_DISPLAY			0

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

#endif // BOARD == BOARD_TS0201_TZ3000
#endif /* _BOARD_TS0201_TZ3000_H_ */
