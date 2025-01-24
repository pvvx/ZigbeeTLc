/********************************************************************************************************
 * @file    board_cb3s.h
 *
 * @brief   This is the header file for board_zth03
 *
 *******************************************************************************************************/
#ifndef _BOARD_CB3S_H_
#define _BOARD_CB3S_H_

#include "version_cfg.h"

#if (BOARD == BOARD_CB3S)

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define RF_TX_POWER_DEF RF_POWER_INDEX_P3p01dBm

// TLSR8258
// GPIO_PA0 - free
// GPIO_PA1 - free
// GPIO_PA7 - SWS, (debug TX)
// GPIO_PB1 - TX
// GPIO_PB4 - free
// GPIO_PB5 - free
// GPIO_PB6 - free
// GPIO_PB7 - RX
// GPIO_PC0 - free
// GPIO_PC1 - free
// GPIO_PC2 - KEY
// GPIO_PC3 - free
// GPIO_PC4 - LED
// GPIO_PD2 - free
// GPIO_PD3 - free
// GPIO_PD4 - free
// GPIO_PD7 - free


// KEY, BUTTON
#define BUTTON1				GPIO_PC2
#define PC2_INPUT_ENABLE	1
#define PC2_DATA_OUT		0
#define PC2_OUTPUT_ENABLE	0
#define PC2_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PD4 PM_PIN_UP_DOWN_FLOAT

// LED
#define USE_BLINK_LED		1
#define LED_ON				1
#define LED_OFF				0
#define GPIO_LED			GPIO_PC4
#define PC4_FUNC	  		AS_GPIO
#define PC4_OUTPUT_ENABLE	1
#define PC4_INPUT_ENABLE	1
#define PC4_DATA_OUT		LED_OFF

// DISPLAY
#define SHOW_SMILEY			0
#define	USE_DISPLAY			0

// I2C
#define	USE_I2C_DRV			I2C_DRV_NONE

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

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
#endif // (BOARD == BOARD_CB3S)
#endif // _BOARD_CB3S_H_
