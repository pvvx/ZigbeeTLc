/*
 * board_mho_c122.h
 *
 *  Created on: 12 нояб. 2023 г.
 *      Author: pvvx
 */
#ifndef _BOARD_MJWSD06MMC_H_
#define _BOARD_MJWSD06MMC_H_

#include "version_cfg.h"

#if (BOARD == BOARD_MJWSD06MMC)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_SCREEN | SERVICE_THS)

/* https://pvvx.github.io/MJWSD06MMC

 * Original Flash markup:
  0x00000 Old Firmware bin
  0x20000 OTA New bin storage Area
  0x74000 Pair & Security info (CFG_ADR_BIND)?
  0x76000 MAC address (CFG_ADR_MAC)
  0x77000 Customize freq_offset adjust cap value (CUST_CAP_INFO_ADDR)
  0x78000 Mijia key-code (blt.3.1ks7k7p8ocg00, bindkey, ...)
  0x7D000 SerialStr: "V1.3F3.0-4-FY-LB-S-G-TM-"
  0x80000 End Flash (FLASH_SIZE)

//#define MI_HW_VER_FADDR 0x7D000 // Mi HW version (DEVICE_LYWSD03MMC)

 * TLSR8251F512ET24
 GPIO_PA5 - DM, free, pcb mark "A5" (KEY)
 GPIO_PA6 - DP, free, pcb mark "A6"
 GPIO_PA7 - SWS, free, pcb mark "S" (debug TX)
 GPIO_PB6 - free, pcb mark "PB6"
 GPIO_PB7 - free, pcb mark "R"
 GPIO_PC2 - SDA, used I2C, pcb mark "SDA"
 GPIO_PC3 - SCL, used I2C, pcb mark "SCK"
 GPIO_PC4 - Vbat (+V-R/RC-GND)
 GPIO_PD2 - KEY to GND
 GPIO_PD7 - free, pcb mark "T"

 *  MJWSD06MMC LCD buffer:  byte.bit

                              OO 4.3               /+ 2.5

           --0.4--         --4.4--            --3.4--          BAT
    |    |         |     |         |        |         |        2.0
    |   5.0       5.5   4.0       4.5      3.0       3.5
    |    |         |     |         |        |         |      o 2.2
   5.3     --5.1--         --4.1--            --3.1--          +--- 2.2
    |    |         |     |         |        |         |     2.2|
    |   5.2       5.6   4.2       4.6      3.2       3.6       ---- 2.1
    |    |         |     |         |        |         |     2.2|
           --5.7--         --4.7--     *      --3.7--          ---- 2.3
                                      3.3

                                     --1.7--         --0.7--
                                   |         |     |         |
       2.4      2.4               1.3       1.6   0.3       0.6
       / \      / \                |         |     |         |
 1.0(  ___  2.7 ___  )1.0            --1.2--         --0.2--
       2.7  / \ 2.7                |         |     |         |
            ___                   1.1       1.5   0.1       0.5     %
            2.4                    |         |     |         |     0.0
                                     --1.4--         --0.4--

  None: 2.6
*/

#define BLE_MODEL_STR		"MJWSD06MMC"
#define BLE_MAN_STR			"Xiaomi"

#define ZCL_BASIC_MFG_NAME	{6,'X','i','a','o','m','i'}
#define ZCL_BASIC_MODEL_ID	{12,'M','J','W','S','D','0','6','M','M','C','-','z'} // MJWSD06MMC-z

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2450

// DISPLAY
#define SHOW_SMILEY			1
#define	USE_DISPLAY			3
#define LCD_BUF_SIZE		6
#define LCD_CMP_BUF_SIZE	(LCD_BUF_SIZE + 1)
#define LCD_INIT_DELAY()	// pm_wait_ms(50)
#define USE_DISPLAY_CONNECT_SYMBOL		2 // =2 inverted
#define USE_DISPLAY_OFF		1

// BUTTON
#define BUTTON1             GPIO_PD2
#define BUTTON1_ON			0
#define BUTTON1_OFF			1
#define PD2_FUNC			AS_GPIO
#define PD2_OUTPUT_ENABLE	0
#define PD2_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PD2	PM_PIN_PULLUP_10K

// I2C Sensor
#define	USE_I2C_DRV			I2C_DRV_HARD
#define I2C_CLOCK			400000 // Hz
#define I2C_SCL 			GPIO_PC2
#define I2C_SDA 			GPIO_PC3
#define I2C_GROUP 			I2C_GPIO_GROUP_C2C3
//#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
//#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

// Sensor T&H
#define USE_SENSOR_CHT8305		0
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		0
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		0
#define USE_SENSOR_SHT30		0

// VBAT
#define SHL_ADC_VBAT		C5P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PC5 // missing pin on case TLSR825x
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

#endif // (BOARD == BOARD_MJWSD06MMC)
#endif /* _BOARD_MJWSD06MMC_H_ */

