/********************************************************************************************************
 * @file    board_mho_c401n.h
 *
 * @brief   This is the header file for board_mho_c401n
 *
 *******************************************************************************************************/
#ifndef _BOARD_MHO_C401N_H_
#define _BOARD_MHO_C401N_H_

#include "version_cfg.h"

#if (BOARD == BOARD_MHO_C401N)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_SCREEN | SERVICE_THS)

/* https://pvvx.github.io/MHO_C401N

TLSR8251F512ET24

GPIO_PA5 - used EPD_BUSY
GPIO_PA6 - used EPD_CSB
GPIO_PA7 - SWS, free, (debug TX)
GPIO_PB6 - used KEY, pcb mark "P5" (TRG)
GPIO_PB7 - used EPD_RST2
GPIO_PC2 - SDA, used I2C
GPIO_PC3 - SCL, used I2C
GPIO_PC4 - used EPD_SCL
GPIO_PD2 - used EPD_SDA
GPIO_PD7 - used EPD_RST

* MHO-C401-2022 LCD buffer:  byte.bit

  7.4  8.4--8.7-- .  10.4--10.7--- .       12.4--12.7--- .     BAT 14.6
   |    |         |    |           |         |           |
  7.4  8.3       9.2 10.3        11.2      12.3        13.2   o|--- 14.4
   |    |         |    |           |         |           |     |  --14.2
  7.4  8.2--8.6--9.1 10.2--10.6--11.1      12.2--12.6--13.1    |  --14.0
   |    |         |    |           |         |           |
  7.4  8.1       9.0 10.1        11.0      12.1        13.0
   |    |         |    |           |  11.4   |           |
  7.4  8.0--8.5-- '  10.0--10.5--- '    *  12.0--12.5--- '


       1.4--1.7-- .   3.4--3.7-- .
        |         |    |         |     *7.2 ^6.6      ^6.6
 BLE   1.3       2.2  3.3       4.2         U6.5      U6.5
 0.4    |         |    |         |               ^6.0
       1.2--1.6--2.1  3.2--3.6--4.1    (7.0 `5.2 -5.6  )7.0 ;6.2
  @     |         |    |         |               U5.4
 0.0   1.1       2.0  3.1       4.0
        |         |    |         |               %5.0
       1.0--1.5-- '   3.0--3.5-- '

Background: 15=0xff ?
None: 0.1..0.3, 0.5..0.7, 2.3..2.7, 4.3..4.7, 5.1, 5,3, 5.5, 5.7, 6.1, 6.3, 6.7, 7.1, 7.3, 7.5..7.7, 9.3..9.7, 11.3, 11.5..11.7, 13.3..13.7, 14.1, 14.3, 14.5
*/

#define BLE_MODEL_STR		"MHO-C401N"
#define BLE_MAN_STR			"miaomiaoce.com"

#define ZCL_BASIC_MFG_NAME     {10,'M','i','a','o','M','i','a','o','C','e'} // MiaoMiaoCe
#define ZCL_BASIC_MODEL_ID	   {11,'M','H','O','-','C','4','0','1','N','-','z'} // MHO-C401N

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2032

// DISPLAY
#define SHOW_SMILEY			1
#define	USE_DISPLAY			4
#define USE_EPD				50 // while(task_lcd()) pm_wait_ms(50 ms);
#define LCD_BUF_SIZE		16
#define LCD_INIT_DELAY()
#define USE_DISPLAY_CONNECT_SYMBOL		2 // =2 inverted

// DISPLAY GPIO
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
#define BUTTON1_ON			0
#define BUTTON1_OFF			1
#define PB6_INPUT_ENABLE	1
#define PB6_DATA_OUT		0
#define PB6_OUTPUT_ENABLE	0
#define PB6_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB6 PM_PIN_PULLUP_1M

// I2C Sensor
#define	USE_I2C_DRV			I2C_DRV_HARD
#define I2C_CLOCK			100000 // Hz
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

#endif // BOARD == BOARD_MHO_C401N
#endif // _BOARD_MHO_C401N_H_
