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

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_SCREEN | SERVICE_THS)

/* https://pvvx.github.io/CGDK2

TLSR8253F512ET32

GPIO_PA0 - free, UART_RX, pcb mark "TP3", (Reed Switch, input)
GPIO_PA1 - free
GPIO_PA7 - SWS, free, (debug TX), pcb mark "TP17"
GPIO_PB1 - free, UART_TX, pcb mark "TP1", (TRG)
GPIO_PB4 - free
GPIO_PB5 - free
GPIO_PB6 - free, (ADC2)
GPIO_PB7 - free, (ADC1)
GPIO_PC0 - SDA, used I2C
GPIO_PC1 - SCL, used I2C
GPIO_PC2 - free
GPIO_PC3 - free
GPIO_PC4 - used KEY (RDS)
GPIO_PD2 - free
GPIO_PD3 - free
GPIO_PD4 - free
GPIO_PD7 - free

CGDK2 LCD buffer:  byte.bit

       ---------1.5-------------      O 17.6  :--17.6---
       |                       |              |
 BLE  ||    |   |   |   |   |  |             17.6
 1.4  ||   1.6 1.7 1.3 1.2 1.1 |              |
      ||    |   |   |   |   |  |              :--17.5---
       |                       |              |
       -------------------------             17.6
                                              |
                                              :--17.4---

   |   2.7---2.3---5.7  3.3---4.7---5.6       0.7---0.6--17.3
   |    |           |    |           |         |           |
   |   2.6         3.7  3.2         4.3       0.3        17.2
   |    |           |    |           |         |           |
  1.0  2.5---2.2---3.6  3.1---4.6---4.2       0.2---0.5--17.1
   |    |           |    |           |         |           |
   |   2.4         3.5  3.0         4.1       0.1        17.0
   |    |           |    |           |   5.4   |           |
   |   2.0---2.1---3.4  4.4---4.5---4.0   *   0.0---0.4---5.5

  ------------------------------5.4--------------------------

   5.3---6.7---8.3  7.7---7.3---8.2   %   9.7---9.3--17.7
    |           |    |           |   8.1   |           |
   5.2         6.3  7.6         8.7       9.6        10.7
    |           |    |           |         |           |
   5.1---6.6---6.2  7.5---7.2---8.6       9.5---9.2--10.6
    |           |    |           |         |           |
   5.0         6.1  7.4         8.5       9.4        10.5
    |           |    |           |   8.0   |           |
   6.4---6.5---6.0  7.0---7.1---8.4   *   9.0---9.1--10.4


none: 10.0-10.3, 11.0-16.7
*/

#define BLE_MODEL_STR		"CGDK2"
#define BLE_MAN_STR			"Qingping"

#define ZCL_BASIC_MFG_NAME     {8,'Q','i','n','g','p','i','n','g'} // Qingping
#define ZCL_BASIC_MODEL_ID	   {7,'C','G','D','K','2','-','z'} // CGDK2

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2430

// DISPLAY
#define SHOW_SMILEY			0
#define	USE_DISPLAY			1
#define LCD_BUF_SIZE		18
#define LCD_INIT_DELAY()
#define USE_DISPLAY_BATTERY_LEVEL		1
#define USE_DISPLAY_SMALL_NUMBER_X10	1
#define USE_DISPLAY_OFF					1
#define USE_DISPLAY_CONNECT_SYMBOL		2 // =2 inverted

// KEY, BUTTON
#define BUTTON1				GPIO_PC4
#define PC4_INPUT_ENABLE	1
#define PC4_DATA_OUT		0
#define PC4_OUTPUT_ENABLE	0
#define PC4_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC4 PM_PIN_PULLUP_1M

// I2C Sensors & Display
#define	USE_I2C_DRV			I2C_DRV_HARD
#define I2C_CLOCK			100000 // Hz
#define I2C_SCL 			GPIO_PC0
#define I2C_SDA 			GPIO_PC1
#define I2C_GROUP 			I2C_GPIO_GROUP_C0C1
#define PULL_WAKEUP_SRC_PC0	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC1	PM_PIN_PULLUP_10K

// Sensor T&H
#define USE_SENSOR_CHT8305		0
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		0
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		1
#define USE_SENSOR_SHT30		0

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

#endif // (BOARD == BOARD_CGDK2)
#endif // _BOARD_CGDK2_H_
