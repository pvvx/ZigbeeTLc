/********************************************************************************************************
 * @file    board_mho_c401n.h
 *
 * @brief   This is the header file for board_mho_c401n
 *
 *******************************************************************************************************/
#ifndef _BOARD_CGG1_H_
#define _BOARD_CGG1_H_

#include "version_cfg.h"

#if (BOARD == BOARD_CGG1)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_SCREEN | SERVICE_THS)

/* https://pvvx.github.io/CGG1

TLSR8253F512ET32

GPIO_PA0 - used EPD_RST
GPIO_PA1 - used EPD_SHD
GPIO_PA7 - SWS, free, (debug TX)
GPIO_PB1 - free
GPIO_PB4 - free
GPIO_PB5 - free (ADC1)
GPIO_PB6 - free (ADC2)
GPIO_PB7 - used EPD_SDA
GPIO_PC0 - SDA, used I2C
GPIO_PC1 - SCL, used I2C
GPIO_PC2 - TX, free, (Trigger, Output)
GPIO_PC3 - RX, free, (RDS)
GPIO_PC4 - used KEY
GPIO_PD2 - used EPD_CSB
GPIO_PD3 - free
GPIO_PD4 - used EPD_BUSY
GPIO_PD7 - used EPD_SCL

EPD: https://pvvx.github.io/CGG1
*/

#define BLE_MODEL_STR		"CGG1"
#define BLE_MAN_STR			"Qingping"

#define ZCL_BASIC_MFG_NAME     {8,'Q','i','n','g','p','i','n','g'} // Qingping
#define ZCL_BASIC_MODEL_ID	   {6,'C','G','G','1','-','z'} // CGG1

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2430

// DISPLAY
#define SHOW_SMILEY			1
#define	USE_DISPLAY			1
#define USE_EPD				50 // while(task_lcd()) pm_wait_ms(50 ms);
#define LCD_BUF_SIZE		18
#define LCD_INIT_DELAY()	// pm_wait_ms(50)
#define USE_DISPLAY_CONNECT_SYMBOL		1
#define USE_DISPLAY_BATTERY_LEVEL		1
#define USE_DISPLAY_SMALL_NUMBER_X10	1

// DISPLAY GPIO
#define EPD_SHD				GPIO_PA1 // should be high
#define PULL_WAKEUP_SRC_PA1 PM_PIN_PULLUP_10K

#define EPD_BUSY			GPIO_PD4
#define PULL_WAKEUP_SRC_PD4 PM_PIN_PULLUP_1M
#define PD4_INPUT_ENABLE	1
#define PD4_FUNC			AS_GPIO

#define EPD_RST				GPIO_PA0
#define PULL_WAKEUP_SRC_PA0 PM_PIN_PULLUP_1M
#define PA0_INPUT_ENABLE	1
#define PA0_DATA_OUT		1
#define PA0_OUTPUT_ENABLE	1
#define PA0_FUNC			AS_GPIO

#define EPD_CSB				GPIO_PD2
#define PULL_WAKEUP_SRC_PD2 PM_PIN_PULLUP_1M
#define PD2_INPUT_ENABLE	1
#define PD2_DATA_OUT		1
#define PD2_OUTPUT_ENABLE	1
#define PD2_FUNC			AS_GPIO

#define EPD_SDA				GPIO_PB7
#define PULL_WAKEUP_SRC_PB7 PM_PIN_PULLDOWN_100K // PM_PIN_PULLUP_1M
#define PB7_INPUT_ENABLE	1
#define PB7_DATA_OUT		1
#define PB7_OUTPUT_ENABLE	1
#define PB7_FUNC			AS_GPIO

#define EPD_SCL				GPIO_PD7
#define PULL_WAKEUP_SRC_PD7 PM_PIN_PULLDOWN_100K // PM_PIN_PULLUP_1M
#define PD7_INPUT_ENABLE	1
#define PD7_DATA_OUT		0
#define PD7_OUTPUT_ENABLE	1
#define PD7_FUNC			AS_GPIO

// Sensor T&H
#define USE_SENSOR_CHT8305		0
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		0
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		1
#define USE_SENSOR_SHT30		1

// VBAT
#define SHL_ADC_VBAT		1  // "B0P" in adc.h
#define GPIO_VBAT			GPIO_PB0 // missing pin on case TLSR8253F512ET32
#define PB0_INPUT_ENABLE	1
#define PB0_DATA_OUT		1
#define PB0_OUTPUT_ENABLE	1
#define PB0_FUNC			AS_GPIO

// I2C Sensors
#define	USE_I2C_DRV			I2C_DRV_HARD
#define I2C_CLOCK 			400000 // 400 kHz
#define I2C_SCL 			GPIO_PC0
#define I2C_SDA 			GPIO_PC1
#define I2C_GROUP 			I2C_GPIO_GROUP_C0C1
#define PULL_WAKEUP_SRC_PC0	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC1	PM_PIN_PULLUP_10K


// BUTTON PC4
#define BUTTON1				GPIO_PC4
#define BUTTON1_ON			0
#define BUTTON1_OFF			1
#define PC4_INPUT_ENABLE	1
#define PC4_DATA_OUT		0
#define PC4_OUTPUT_ENABLE	0
#define PC4_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC4 PM_PIN_PULLUP_1M

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif

#endif // BOARD == BOARD_CGG1
#endif // _BOARD_CGG1_H_
