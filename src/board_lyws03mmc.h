/********************************************************************************************************
 * @file    board_lyws03mmc.h
 *
 * @brief   This is the header file for board_8258_lywsd03mmc
 *
 *******************************************************************************************************/
#ifndef _BOARD_LYWS03MMC_H_
#define _BOARD_LYWS03MMC_H_

#include "version_cfg.h"

#if (BOARD == BOARD_LYWSD03MMC)
/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define RF_TX_POWER_DEF		RF_POWER_INDEX_P1p99dBm


// BUTTON
#define BUTTON1				GPIO_PA5  // reset test point
#define PA5_FUNC			AS_GPIO
#define PA5_OUTPUT_ENABLE	0
#define PA5_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PA5	PM_PIN_PULLUP_10K

// DISPLAY
#define SHOW_SMILEY			1
#define	USE_DISPLAY			1
#define LCD_BUF_SIZE		6
#define PULL_WAKEUP_SRC_PB6 PM_PIN_PULLUP_10K // LCD
#define PULL_WAKEUP_SRC_PD7	PM_PIN_PULLUP_1M // UART TX (B1.6)
#define LCD_INIT_DELAY()	pm_wait_ms(50)


// I2C
#define I2C_CLOCK			400000 // Hz
#define SENSOR_TYPE 		SENSOR_SHTC3_4X
#define USE_SENSOR_ID		0

#define I2C_SCL 			GPIO_PC2
#define I2C_SDA 			GPIO_PC3
#define I2C_GROUP 			I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K


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
#endif // (BOARD == BOARD_LYWSD03MMC)
#endif // _BOARD_LYWS03MMC_H_
