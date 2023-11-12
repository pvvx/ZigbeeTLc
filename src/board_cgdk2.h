/********************************************************************************************************
 * @file    board_8258_03mmc.h
 *
 * @brief   This is the header file for board_8258_cgdk2
 *
 *******************************************************************************************************/

#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

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


/*
#define GPIO_TRG			GPIO_PB1
#define PB1_INPUT_ENABLE	1
#define PB1_DATA_OUT		0
#define PB1_OUTPUT_ENABLE	0
#define PB1_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB1	PM_PIN_PULLDOWN_100K
*/

#define I2C_CLOCK			100000 // Hz

#define LCD_BUF_SIZE	18

#define I2C_SCL 	GPIO_PC0
#define I2C_SDA 	GPIO_PC1
#define I2C_GROUP 	I2C_GPIO_GROUP_C0C1
#define PULL_WAKEUP_SRC_PC0	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC1	PM_PIN_PULLUP_10K

#define USE_SENSOR_ID		1

#define SHL_ADC_VBAT		B0P // "B0P" see in adc.h ADC_InputPchTypeDef
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


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
