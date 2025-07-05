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

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_SCREEN | SERVICE_THS)

/* TLSR8251F512ET24

GPIO_PA5 - DM, free, pcb mark "reset" (KEY)
GPIO_PA6 - DP, free, pcb mark "P8" (RDS)
GPIO_PA7 - SWS, free, pcb mark "P14", (debug TX)
GPIO_PB6 - used LCD, set "1"
GPIO_PB7 - free, pcb mark "B1" (ADC1)
GPIO_PC2 - SDA, used I2C, pcb mark "P12"
GPIO_PC3 - SCL, used I2C, pcb mark "P15"
GPIO_PC4 - free, pcb mark "P9" (ADC2)
GPIO_PD2 - CS/PWM, free (TRG)
GPIO_PD7 - free [B1.4], UART TX LCD [B1.6], pcb mark "P7"

 *  LYWSD03MMC LCD buffer:  byte.bit

         --5.4--         --4.4--            --3.4--          BLE
  |    |         |     |         |        |         |        2.4
  |   5.5       5.0   4.5       4.0      3.5       3.0  
  |    |         |     |         |        |         |      o 2.5
 5.3     --5.1--         --4.1--            --3.1--          +--- 2.5
  |    |         |     |         |        |         |     2.5|
  |   5.6       5.2   4.6       4.2      3.6       3.2       ---- 2.6
  |    |         |     |         |        |         |     2.5|
         --5.7--         --4.7--     *      --3.7--          ---- 2.7
                                    4.3
                                        --1.4--         --0.4--
                                      |         |     |         |
          2.0      2.0               1.5       1.0   0.5       0.0
          / \      / \                |         |     |         |
    2.2(  ___  2.1 ___  )2.2            --1.1--         --0.1--
          2.1  / \ 2.1                |         |     |         |
               ___                   1.6       1.2   0.6       0.2     %
               2.0                    |         |     |         |     0.3
                                        --1.7--         --0.7--
                           BAT 1.3
*/


#define BLE_MODEL_STR		"LYWSD03MMC"
#define BLE_MAN_STR			"miaomiaoce.com"
#define ZCL_BASIC_MFG_NAME	{6,'X','i','a','o','m','i'}
#define ZCL_BASIC_MODEL_ID	{12,'L','Y','W','S','D','0','3','M','M','C','-','z'}

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2032

// DISPLAY
#define SHOW_SMILEY			1
#define	USE_DISPLAY			1
#define LCD_BUF_SIZE		6
#define PULL_WAKEUP_SRC_PB6 PM_PIN_PULLUP_10K // LCD
#define PULL_WAKEUP_SRC_PD7	PM_PIN_PULLUP_1M // UART TX (B1.6)
#define LCD_INIT_DELAY()	//pm_wait_ms(50)
#define USE_DISPLAY_OFF		1

// BUTTON
#define BUTTON1				GPIO_PA5  // reset test point
#define BUTTON1_ON			0
#define BUTTON1_OFF			1
#define PA5_FUNC			AS_GPIO
#define PA5_OUTPUT_ENABLE	0
#define PA5_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PA5	PM_PIN_PULLUP_10K

// I2C Sensor & LCD
#define	USE_I2C_DRV			I2C_DRV_HARD
#define I2C_CLOCK			400000 // Hz
#define I2C_SCL 			GPIO_PC2
#define I2C_SDA 			GPIO_PC3
#define I2C_GROUP 			I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

// Sensor T&H
#define USE_SENSOR_CHT8305		0
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		0
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		1
#define USE_SENSOR_SHT30		0

#define SHL_ADC_VBAT		C5P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PC5 // missing pin on case TLSR8251F512ET24
#define PC5_INPUT_ENABLE	0
#define PC5_DATA_OUT		1
#define PC5_OUTPUT_ENABLE	1
#define PC5_FUNC			AS_GPIO

#define GPIO_LCD_URX 		UART_TX_PD7  // UART RX LCD old B1.6
#define GPIO_LCD_UTX 		UART_RX_PB7  // UART TX LCD old B1.6
#define GPIO_LCD_CLK 		GPIO_PD7  //(not change!) SPI-CLK LCD new B1.6
#define GPIO_LCD_SDI 		GPIO_PB7  //(not change!) SPI-SDI LCD new B1.6

#define PD7_INPUT_ENABLE	1
#define PD7_DATA_OUT		1
#define PD7_OUTPUT_ENABLE	1
#define PD7_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PD7	PM_PIN_PULLUP_1M // UART TX (B1.6)

#define PB7_INPUT_ENABLE	1
#define PB7_DATA_OUT		1
#define PB7_OUTPUT_ENABLE	0
#define PB7_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB7	PM_PIN_PULLUP_1M // SPI CLK (B1.6 new)


// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif

#endif // (BOARD == BOARD_LYWSD03MMC)
#endif // _BOARD_LYWS03MMC_H_
