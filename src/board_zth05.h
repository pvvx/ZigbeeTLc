/********************************************************************************************************
 * @file    board_zth05.h
 *
 * @brief   This is the header file for board_zth05
 *
 *******************************************************************************************************/
#ifndef _BOARD_ZTH05_H_
#define _BOARD_ZTH05_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZTH05)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_SCREEN | SERVICE_THS)

/* https://pvvx.github.io/TS0601_TZE204/

*TLSR8258

 GPIO_PA0 - free (Reed Switch, input)
 GPIO_PA1 - free
 GPIO_PA7 - SWS, (debug TX)
 GPIO_PB1 - free
 GPIO_PB4 - KEY
 GPIO_PB5 - free, (TRG)
 GPIO_PB6 - free
 GPIO_PB7 - free
 GPIO_PC0 - free
 GPIO_PC1 - free
 GPIO_PC2 - SDA
 GPIO_PC3 - SCL
 GPIO_PC4 - free
 GPIO_PD2 - free, (RDS)
 GPIO_PD3 - free
 GPIO_PD4 - free
 GPIO_PD7 - free

*ZTH05 LCD buffer:  byte.bit

         --0.4--         --1.4--            --2.4--          BAT
  |    |         |     |         |        |         |        3.5
  |   0.5       0.0   1.5       1.0      2.5       2.0
  |    |         |     |         |        |         |      o 3.6
 0.3     --0.1--         --1.1--            --2.1--          +--- 3.6
  |    |         |     |         |        |         |     3.6|
  |   0.6       0.2   1.6       1.2      2.6       2.2       ---- 3.7
  |    |         |     |         |        |         |     3.6|
         --0.7--         --1.7--     *      --2.7--          ---- 2.3
                                    1.3
                                        --4.4--         --5.4--
                                      |         |     |         |
          3.0      3.0               4.5       4.0   5.5       5.0
          / \      / \                |         |     |         |
    3.4(  ___  3.1 ___  )3.4            --4.1--         --5.1--
          3.1  / \ 3.1                |         |     |         |
               ___                   4.6       4.2   5.6       5.2     %
               3.0                    |         |     |         |     5.3
                                        --4.7--         --5.7--
                        oo 4.3
None: 3.2, 3.3
*/

#define BLE_MODEL_STR		"ZTH05"
#define BLE_MAN_STR			"Tuya"

#define ZCL_BASIC_MFG_NAME     {4,'T','u','y','a'} // Tuya
#define ZCL_BASIC_MODEL_ID	   {6,'T','H','0','5','-','z'} // ZTH05 

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2032

// DISPLAY
#define SHOW_SMILEY			1
#define	USE_DISPLAY			1
#define LCD_BUF_SIZE		6
#define LCD_CMP_BUF_SIZE (LCD_BUF_SIZE + 1)
#define LCD_INIT_DELAY()
#define USE_DISPLAY_OFF		1

// KEY, BUTTON
#define BUTTON1				GPIO_PB4
#define BUTTON1_ON			0
#define BUTTON1_OFF			1
#define PB4_INPUT_ENABLE	1
#define PB4_DATA_OUT		0
#define PB4_OUTPUT_ENABLE	0
#define PB4_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB4 PM_PIN_PULLUP_1M

// I2C Sensor
#define	USE_I2C_DRV			I2C_DRV_HARD
#define I2C_CLOCK			400000 // Hz
#define I2C_SCL 			GPIO_PC2
#define I2C_SDA 			GPIO_PC3
#define I2C_GROUP 			I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

// Sensor T&H
#define USE_SENSOR_CHT8305		1
#define USE_SENSOR_CHT8215		1
#define USE_SENSOR_AHT20_30		1
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		1
#define USE_SENSOR_SHT30		1

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

#endif // (BOARD == BOARD_ZTH05)
#endif // _BOARD_ZTH05_H_
