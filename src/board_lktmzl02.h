/********************************************************************************************************
 * @file    board_zth03.h
 *
 * @brief   This is the header file for board_zth03
 *
 *******************************************************************************************************/
#ifndef _BOARD_LKTMZL02_H_
#define _BOARD_LKTMZL02_H_

#include "version_cfg.h"

#if (BOARD == BOARD_LKTMZL02)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_SCREEN | SERVICE_THS)

/* https://pvvx.github.io/LKTMZL02

TLSR8258

GPIO_PA0 - free (Reed Switch, input)
GPIO_PA1 - free
GPIO_PA7 - SWS, (debug TX)
GPIO_PB1 - free
GPIO_PB4 - LED - "1" On
GPIO_PB5 - free, (TRG)
GPIO_PB6 - free
GPIO_PB7 - free
GPIO_PC0 - KEY
GPIO_PC1 - SDA1, used I2C LCD
GPIO_PC2 - SDA, used I2C Sensor
GPIO_PC3 - SCL, used I2C Sensor
GPIO_PC4 - SCL1, used I2C LCD
GPIO_PD2 - free
GPIO_PD3 - free
GPIO_PD4 - free
GPIO_PD7 - free

LCD LKTMZL02 real buffer:  byte.bit

                                ------------|
 0.2 0.1 0.0					0.5 0.6	0.4 |
  .   i   |                      |   |   |  |0.7
                                ------------|

              --1.4--         --2.4--            --3.0--
       |    |         |     |         |        |         |
       |   1.5       1.1   2.5       2.1      3.1       4.5
       |    |         |     |         |        |         |      o 3.5
-1.0- 0.3     --1.2--         --2.2--            --4.6--          +--- 3.5
       |    |         |     |         |        |         |     3.5|
       |   1.6       1.3   2.6       2.3      3.2       4.7       ---- 3.4
       |    |         |     |         |        |         |     3.5|
              --1.7--         --2.7--     *      --3.3--          ---- 3.6
                                         3.7

          --4.0--         --5.0--
        |         |     |         |
       4.1       5.5   5.1       6.5
        |         |     |         |
          --5.6--         --6.6--
        |         |     |         |
       4.2       5.7   5.2       6.7     %
        |         |     |         |     6.4
          --4.3--         --5.3--


  None: 2.0, 4.4, 5.4, 6.0..6.3

* Work buffer:  byte.bit

                                ------------|
 0.2 0.1 0.0					0.5 0.6	0.4 |
  .   i   |                      |   |   |  |0.7
                                ------------|

              --1.4--         --2.4--            --4.4--
       |    |         |     |         |        |         |
       |   1.5       1.1   2.5       2.1      4.5       4.1
       |    |         |     |         |        |         |      o 3.1
-1.0- 0.3     --1.2--         --2.2--            --4.2--          +--- 3.1
       |    |         |     |         |        |         |     3.1|
       |   1.6       1.3   2.6       2.3      4.6       4.3       ---- 3.0
       |    |         |     |         |        |         |     3.1|
              --1.7--         --2.7--     *      --4.7--          ---- 3.2
                                         3.3

          --5.4--         --6.4--
        |         |     |         |
       5.5       5.1   6.5       6.1
        |         |     |         |
          --5.2--         --6.2--
        |         |     |         |
       5.6       5.3   6.6       6.3     %
        |         |     |         |     6.0
          --5.7--         --6.7--


  None: 2.0, 3.4..3.7, 4.0, 5.0
*/

#define BLE_MODEL_STR		"LKTMZLZ02"
#define BLE_MAN_STR			"Tuya"

#define ZCL_BASIC_MFG_NAME     {4,'T','u','y','a'} // Tuya
#define ZCL_BASIC_MODEL_ID	   {10,'L','K','T','M','Z','L','0','2','-','z'} // LKTMZL02

// Battery & RF Power
#define USE_BATTERY 	BATTERY_2AAA

// DISPLAY
#define SHOW_SMILEY			0
#define	USE_DISPLAY			2
#define LCD_BUF_SIZE		18
#define LCD_INIT_DELAY()
#define USE_DISPLAY_BATTERY_LEVEL		1
#define USE_DISPLAY_OFF					1
#define USE_DISPLAY_CONNECT_SYMBOL		2 // =2 inverted

// KEY, BUTTON
#define BUTTON1				GPIO_PC0
#define BUTTON1_ON 			0
#define BUTTON1_OFF 		1
#define PC0_INPUT_ENABLE	1
#define PC0_DATA_OUT		0
#define PC0_OUTPUT_ENABLE	0
#define PC0_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC0 PM_PIN_PULLUP_1M

// LED
#define USE_BLINK_LED		1
#define LED_ON				1
#define LED_OFF				0
#define GPIO_LED			GPIO_PB4
#define PB4_FUNC	  		AS_GPIO
#define PB4_OUTPUT_ENABLE	1
#define PB4_INPUT_ENABLE	1
#define PB4_DATA_OUT		LED_OFF

// I2C Sensor
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
#define USE_SENSOR_AHT20_30		1
#define USE_SENSOR_SHT4X		0
#define USE_SENSOR_SHTC3		0
#define USE_SENSOR_SHT30		0

// I2C LCD
#define I2C_SCL_LCD			GPIO_PC4
#define I2C_SDA_LCD			GPIO_PC1
#define PC1_INPUT_ENABLE	1
#define PC1_DATA_OUT		0
#define PC1_OUTPUT_ENABLE	0
#define PC1_FUNC			AS_GPIO
#define PC4_INPUT_ENABLE	1
#define PC4_DATA_OUT		0
#define PC4_OUTPUT_ENABLE	0
#define PC4_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC1	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC4	PM_PIN_PULLUP_10K

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

#endif // (BOARD == BOARD_LKTMZL02)
#endif // _BOARD_LKTMZL02_H_
