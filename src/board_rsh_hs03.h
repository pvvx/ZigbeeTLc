/********************************************************************************************************
 * @file    board_rsh_hs03.h
 *
 * @brief   This is the header file for board_rsh_hs03
 *
 *******************************************************************************************************/
#ifndef _BOARD_RSH_HS03_H_
#define _BOARD_RSH_HS03_H_

#include "version_cfg.h"

#if (BOARD == BOARD_RSH_HS03)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_SCREEN | SERVICE_THS)

/*
  This file is for Zigbee T&H Sensors with "RSH-HS03-V2.0" designator on PCB
  Zigbee model/manufacturer TY0201_TZ3000_bjawzodf

  There are 2 HW version of this sensor: with MRT08S SO-8 chip soldered on PCB
  and version without this chip but with 2 zero ohm resistors near PA0, PA1.

  HARDWARE MODIFICATION IS REQUIRED TO RUN THIS FIRMWARE ON BOARDS WITH MRT08S,
  see https://github.com/pvvx/ZigbeeTLc/issues/107#issuecomment-2227385821

TLSR8258

GPIO_PA0 - free or soft I2C SCL on boards without MRT08S chip
GPIO_PA1 - free or soft I2C SDA on boards without MRT08S chip
GPIO_PA7 - SWS
GPIO_PB4 - KEY
GPIO_PC2 - UART TX on boards with MRT08S chip, debug header TX, used for HW I2C SDA after MRT08S removal
GPIO_PC3 - UART RX on boards with MRT08S chip, debug header RX, used for HW I2C SCL after MRT08S removal

 *  RSH-HS03-V2.0 LCD buffer:  byte.bit
 *

 RSSI indicator     Battery level indicator

                    ---------6.1-----------
              |     |                     |
          |   |     |  |   |   |   |   |  ||
      |   |   |     | 7.7 7.6 7.5 7.4 6.0 ||
  |   |   |   |     |  |   |   |   |   |  ||
 3.7 3.3 4.3 6.3    |                     |
                    ---------6.1-----------



                --4.7--         --5.7--            --6.7--
         |    |         |     |         |        |         |
         |   3.2       4.6   4.2       5.6      5.3       6.6
         |    |         |     |         |        |         |      o 6.2
--3.6-- 7.3     --3.1--         --4.1--            --5.2--          +--- 6.2
         |    |         |     |         |        |         |     6.2|
  TEMP   |   3.0       4.5   4.0       5.5      5.1       6.5       ---- 7.1
  ICON   |    |         |     |         |        |         |     6.2|
  3.5           --4.4--         --5.4--     *      --6.4--          ---- 7.2
                                           5.0

  ---------------------------------3.4------------------------------------

               ---0.3---      ---1.3---           ---2.3---      %
  HUMID       |         |    |         |         |         |    1.7
  ICON       0.6       0.2  1.6       1.2       2.7       2.2
  0.7         |         |    |         |         |         |
               ---0.5---      ---1.5---           ---2.6---
              |         |    |         |         |         |
             0.4       0.1  1.4       1.1       2.5       2.1
              |         |    |         |   2.4   |         |
               ---0.0---      ---1.0---     *     ---2.0---


*/

#define BLE_MODEL_STR		"TY0201"
#define BLE_MAN_STR			"Tuya"

#define ZCL_BASIC_MFG_NAME	{4,'T','u','y','a'} // Tuya
#define ZCL_BASIC_MODEL_ID	{8,'T','Y','0','2','0','1','-','z'} // TY0201-z

// Battery & RF Power
#define USE_BATTERY 	BATTERY_2AAA

// DISPLAY
#define SHOW_SMILEY			0
#define	USE_DISPLAY			1
#define LCD_BUF_SIZE		8
#define LCD_CMP_BUF_SIZE    9  // Extra byte for Set Display RAM command
#define LCD_INIT_DELAY()
#define USE_DISPLAY_BATTERY_LEVEL		1
#define USE_DISPLAY_SMALL_NUMBER_X10	1
#define USE_DISPLAY_OFF					1

// KEY BUTTON
#define BUTTON1				GPIO_PB4
#define PB4_INPUT_ENABLE	1
#define PB4_DATA_OUT		0
#define PB4_OUTPUT_ENABLE	0
#define PB4_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB4 PM_PIN_PULLUP_1M

// I2C T&H sensor & Display
// use I2C_DRV_HARD for boards, which originally had MRT08S soldered
#define	USE_I2C_DRV			I2C_DRV_SOFT

#if USE_I2C_DRV == I2C_DRV_HARD
#define I2C_CLOCK			400000 // Hz
#define I2C_SCL				GPIO_PC3
#define I2C_SDA				GPIO_PC2
#define I2C_GROUP			I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K
#else // I2C_DRV_SOFT
#define I2C_CLOCK			400000 // Hz

#define I2C_SCL				GPIO_PA0
#define PA0_FUNC			AS_GPIO
#define PA0_INPUT_ENABLE	1
#define PA0_OUTPUT_ENABLE	0
#define PA0_DATA_OUT		0
#define PULL_WAKEUP_SRC_PA0	PM_PIN_PULLUP_10K

#define I2C_SDA				GPIO_PA1
#define PA1_FUNC			AS_GPIO
#define PA1_INPUT_ENABLE	1
#define PA1_OUTPUT_ENABLE	0
#define PA1_DATA_OUT		0
#define PULL_WAKEUP_SRC_PA1	PM_PIN_PULLUP_10K
#endif

// Sensor T&H
#define USE_SENSOR_CHT8305		0
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		1
#define USE_SENSOR_SHT4X		0
#define USE_SENSOR_SHTC3		0
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

#endif // (BOARD == BOARD_RSH_HS03)
#endif // _BOARD_RSH_HS03_H_
