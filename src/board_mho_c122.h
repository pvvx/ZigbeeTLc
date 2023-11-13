/*
 * board_mho_c122.h
 *
 *  Created on: 12 нояб. 2023 г.
 *      Author: pvvx
 */
#pragma once

#ifndef BOARD_MHO_C122_H_
#define BOARD_MHO_C122_H_
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

#define SHL_ADC_VBAT	   B0P  // "B0P" in adc.h
#define GPIO_VBAT		GPIO_PB0 // missing pin on case TLSR8251F512ET24
#define PB0_INPUT_ENABLE	1
#define PB0_DATA_OUT		1
#define PB0_OUTPUT_ENABLE	1
#define PB0_FUNC			AS_GPIO


#define I2C_CLOCK	100000 // Hz
#define I2C_SCL 	GPIO_PC2
#define I2C_SDA 	GPIO_PC3
#define I2C_GROUP 	I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

#define SHOW_SMILEY
#define LCD_BUF_SIZE	6
#define PULL_WAKEUP_SRC_PB6 PM_PIN_PULLUP_10K // LCD

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif

#endif /* BOARD_MHO_C122_H_ */
