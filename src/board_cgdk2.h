/********************************************************************************************************
 * @file    board_cgdk2.h
 *
 * @brief   This is the header file for board_cgdk2
 *
 *******************************************************************************************************/
#ifndef _BOARD_CGDK2_H_
#define _BOARD_CGDK2_H_

#include "version_cfg.h"

#if (BOARD == BOARD_CGDK2)

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define RF_TX_POWER_DEF RF_POWER_INDEX_P1p99dBm

// TLSR8253F512ET32
// GPIO_PA0 - free, UART_RX, pcb mark "TP3", (Reed Switch, input)
// GPIO_PA1 - free
// GPIO_PA7 - SWS, free, (debug TX), pcb mark "TP17"
// GPIO_PB1 - free, UART_TX, pcb mark "TP1", (TRG)
// GPIO_PB4 - free
// GPIO_PB5 - free
// GPIO_PB6 - free, (ADC2)
// GPIO_PB7 - free, (ADC1)
// GPIO_PC0 - SDA, used I2C
// GPIO_PC1 - SCL, used I2C
// GPIO_PC2 - free
// GPIO_PC3 - free
// GPIO_PC4 - used KEY (RDS)
// GPIO_PD2 - free
// GPIO_PD3 - free
// GPIO_PD4 - free
// GPIO_PD7 - free

// KEY, BUTTON
#define BUTTON1				GPIO_PC4
#define PC4_INPUT_ENABLE	1
#define PC4_DATA_OUT		0
#define PC4_OUTPUT_ENABLE	0
#define PC4_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC4 PM_PIN_PULLUP_1M

// DISPLAY
#define SHOW_SMILEY			0
#define	USE_DISPLAY			2
#define LCD_BUF_SIZE		18
#define LCD_INIT_DELAY()

// I2C
#define I2C_CLOCK			100000 // Hz
#define SENSOR_TYPE 		SENSOR_SHTC3_4X
#define USE_SENSOR_ID		1

#define I2C_SCL 			GPIO_PC0
#define I2C_SDA 			GPIO_PC1
#define I2C_GROUP 			I2C_GPIO_GROUP_C0C1
#define PULL_WAKEUP_SRC_PC0	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC1	PM_PIN_PULLUP_10K

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
#endif // (BOARD == BOARD_CGDK2)
#endif // _BOARD_CGDK2_H_
