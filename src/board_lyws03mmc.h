/********************************************************************************************************
 * @file    board_8258_03mmc.h
 *
 * @brief   This is the header file for board_8258_lywsd03mmc
 *
 *******************************************************************************************************/

#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define SHOW_SMILEY

// BUTTON
#define BUTTON1               		GPIO_PA5  // reset test point
#define PA5_FUNC			  		AS_GPIO
#define PA5_OUTPUT_ENABLE	  		0
#define PA5_INPUT_ENABLE	  		1
#define	PULL_WAKEUP_SRC_PA5	  		PM_PIN_PULLUP_10K

/*
#define BUTTON2               		GPIO_PA6  // P8 test point
#define PA6_FUNC			  		AS_GPIO
#define PA6_OUTPUT_ENABLE	  		0
#define PA6_INPUT_ENABLE	  		1
#define	PULL_WAKEUP_SRC_PA6	  		PM_PIN_PULLUP_1M
*/

#define	PULL_WAKEUP_SRC_PB6	  		PM_PIN_PULLUP_10K

#define I2C_CLOCK					400000 // Hz
#define LCD_BUF_SIZE	6

#define I2C_SCL 	GPIO_PC2
#define I2C_SDA 	GPIO_PC3
#define I2C_GROUP 	I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

#define PULL_WAKEUP_SRC_PB6 PM_PIN_PULLUP_10K // LCD

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
