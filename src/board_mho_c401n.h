/********************************************************************************************************
 * @file    board_mho_c401n.h
 *
 * @brief   This is the header file for board_mho_c401n
 *
 *******************************************************************************************************/
#ifndef _BOARD_MHO_C401_H_
#define _BOARD_MHO_C401_H_

#include "version_cfg.h"

#if (BOARD == BOARD_MHO_C401N)

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define RF_TX_POWER_DEF		RF_POWER_INDEX_P1p99dBm

// TLSR8251F512ET24
// GPIO_PA5 - used EPD_BUSY
// GPIO_PA6 - used EPD_CSB
// GPIO_PA7 - SWS, free, (debug TX)
// GPIO_PB6 - used KEY, pcb mark "P5" (TRG)
// GPIO_PB7 - used EPD_RST2
// GPIO_PC2 - SDA, used I2C
// GPIO_PC3 - SCL, used I2C
// GPIO_PC4 - used EPD_SCL
// GPIO_PD2 - used EPD_SDA
// GPIO_PD7 - used EPD_RST

// DISPLAY
#define SHOW_SMILEY			1
#define	USE_DISPLAY			4
#define USE_EPD				50 // while(task_lcd()) pm_wait_ms(50 ms);
#define LCD_BUF_SIZE		16
#define LCD_INIT_DELAY()

#define EPD_RST2			GPIO_PB7 // should be high
#define PULL_WAKEUP_SRC_PB7 PM_PIN_PULLUP_1M

#define EPD_RST 			GPIO_PD7 // should be high
#define PULL_WAKEUP_SRC_PD7 PM_PIN_PULLUP_1M
#define PD7_INPUT_ENABLE	1
#define PD7_DATA_OUT		1
#define PD7_OUTPUT_ENABLE	1
#define PD7_FUNC			AS_GPIO

#define EPD_BUSY			GPIO_PA5
#define PULL_WAKEUP_SRC_PA5 PM_PIN_PULLUP_1M
#define PA5_INPUT_ENABLE	1
#define PA5_FUNC			AS_GPIO

#define EPD_CSB				GPIO_PA6
#define PULL_WAKEUP_SRC_PA6 PM_PIN_PULLUP_1M
#define PA6_INPUT_ENABLE	1
#define PA6_DATA_OUT		1
#define PA6_OUTPUT_ENABLE	1
#define PA6_FUNC			AS_GPIO

#define EPD_SDA				GPIO_PD2
#define PULL_WAKEUP_SRC_PD2 PM_PIN_PULLDOWN_100K
#define PD2_INPUT_ENABLE	1
#define PD2_DATA_OUT		1
#define PD2_OUTPUT_ENABLE	1
#define PD2_FUNC			AS_GPIO

#define EPD_SCL				GPIO_PC4
#define PULL_WAKEUP_SRC_PC4 PM_PIN_PULLDOWN_100K // PM_PIN_PULLUP_1M
#define PC4_INPUT_ENABLE	1
#define PC4_DATA_OUT		0
#define PC4_OUTPUT_ENABLE	1
#define PC4_FUNC			AS_GPIO

// BUTTON
#define BUTTON1 			GPIO_PB6	// Reed Switch, input, pcb mark "P5"
#define PB6_INPUT_ENABLE	1
#define PB6_DATA_OUT		0
#define PB6_OUTPUT_ENABLE	0
#define PB6_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB6 PM_PIN_PULLUP_1M

// I2C
#define I2C_CLOCK			100000 // Hz
#define SENSOR_TYPE 		SENSOR_SHTC3_4X
#define USE_SENSOR_ID		0

#define I2C_SCL 			GPIO_PC2
#define I2C_SDA 			GPIO_PC3
#define I2C_GROUP 			I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

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
#endif // BOARD == BOARD_MHO_C401N
#endif // _BOARD_MHO_C401_H_
