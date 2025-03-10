/********************************************************************************************************
 * @file    board_mho_cgg1n.h
 *
 * @brief   This is the header file for board_mho_cgg1n
 *
 *******************************************************************************************************/
#ifndef _BOARD_CGG1N_H_
#define _BOARD_CGG1N_H_

#include "version_cfg.h"

#if (BOARD == BOARD_CGG1N)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_SCREEN | SERVICE_THS)

/* https://pvvx.github.io/CGG1N

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

CGG1-2022 LCD buffer:  byte.bit

  @     ---------5.0-------------     O   :--4.2---
 7.6    |                       |    4.3  |
       ||    |   |   |   |   |  |        4.2
 BLE   ||   5.5 5.4 5.3 5.2 5.1 |         |
 9.6   ||    |   |   |   |   |  |         :--4.1---
        |                       |         |
        -------------------------         4.2
                                          |
                                          :--4.0---

 12.4 11.4--10.7--10.4  7.4---6.7---6.4       3.4---2.7---2.4
   |    |           |    |           |         |           |
 12.3 11.3        10.3  7.3         6.3       3.3         2.3
   |    |           |    |           |         |           |
 12.2 11.2--10.6--10.2  7.2---6.6---6.2       3.2---2.6-- 2.2
   |    |           |    |           |         |           |
 12.1 11.1        10.1  7.1         6.1       3.1         2.1
   |    |           |    |           |   4.5   |           |
 12.0 11.0--10.5--10.0  7.0---6.5---6.0   *   3.0---2.5---2.0

  ------------------------------4.4--------------------------

  14.4--13.7--13.4  9.4---8.7---8.4   %   1.4---0.7---0.4
    |           |    |           |   1.6   |           |
  14.3        13.3  9.3         8.3       1.3         0.3
    |           |    |           |         |           |
  14.2--13.6--13.2  9.2---8.6---8.2       1.2---0.6---0.2
    |           |    |           |         |           |
  14.1        13.1  9.1         8.1       1.1         0.1
    |           |    |           |   4.6   |           |
  14.0--13.5--13.0  9.0---8.5---8.0   *   1.0---0.5---0.0

*/

#define BLE_MODEL_STR		"CGG1N"
#define BLE_MAN_STR			"Qingping"

#define ZCL_BASIC_MFG_NAME     {8,'Q','i','n','g','p','i','n','g'} // Qingping
#define ZCL_BASIC_MODEL_ID	   {7,'C','G','G','1','N','-','z'} // CGG1N

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2032

// DISPLAY
#define SHOW_SMILEY			1
#define	USE_DISPLAY			7
#define USE_EPD				50 // while(task_lcd()) pm_wait_ms(50 ms);
#define LCD_BUF_SIZE		16
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

#endif // BOARD == BOARD_CGG1N
#endif // _BOARD_CGG1N_H_
