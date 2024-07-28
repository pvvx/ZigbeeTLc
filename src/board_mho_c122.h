/*
 * board_mho_c122.h
 *
 *  Created on: 12 нояб. 2023 г.
 *      Author: pvvx
 */
#ifndef _BOARD_MHO_C122_H_
#define _BOARD_MHO_C122_H_

#include "version_cfg.h"

#if (BOARD == BOARD_MHO_C122)

#define RF_TX_POWER_DEF		RF_POWER_INDEX_P1p99dBm

// TLSR8251F512ET24
// GPIO_PA5 - DM, free, (TRG)
// GPIO_PA6 - DP, free, (RDS)
// GPIO_PA7 - SWS, free, (debug TX)
// GPIO_PB6 - used LCD, set "1"
// GPIO_PB7 - free, (ADC1)
// GPIO_PC2 - SDA, used I2C
// GPIO_PC3 - SCL, used I2C
// GPIO_PC4 - free
// GPIO_PD2 - CS/PWM, free
// GPIO_PD7 - free

// BUTTON
#define BUTTON1               		GPIO_PA5  // reset test point
#define PA5_FUNC			  		AS_GPIO
#define PA5_OUTPUT_ENABLE	  		0
#define PA5_INPUT_ENABLE	  		1
#define	PULL_WAKEUP_SRC_PA5	  		PM_PIN_PULLUP_10K

// DISPLAY
#define SHOW_SMILEY			1
#define	USE_DISPLAY			3
#define LCD_BUF_SIZE		6
#define PULL_WAKEUP_SRC_PB6 PM_PIN_PULLUP_10K // LCD ?
#define LCD_INIT_DELAY()	pm_wait_ms(50)

// I2C
#define I2C_CLOCK			400000 // Hz
#define SENSOR_TYPE 		SENSOR_SHTC3_4X
#define USE_SENSOR_ID		1

#define I2C_SCL 			GPIO_PC2
#define I2C_SDA 			GPIO_PC3
#define I2C_GROUP 			I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

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

#endif // (BOARD == BOARD_MHO_C122)
#endif /* _BOARD_MHO_C122_H_ */

