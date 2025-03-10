/*
 * board_mho_c122.h
 *
 *  Created on: 12 нояб. 2023 г.
 *      Author: pvvx
 */
#ifndef _BOARD_MHO_C122_H_
#define _BOARD_MHO_C122_H_

#include "version_cfg.h"

#if (BOARD == BOARD_MHO_C122)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_SCREEN | SERVICE_THS)

/* https://pvvx.github.io/MHO_C122

TLSR8251F512ET24

GPIO_PA5 - DM, free, (TRG)
GPIO_PA6 - DP, free, (RDS)
GPIO_PA7 - SWS, free, (debug TX)
GPIO_PB6 - used LCD, set "1"
GPIO_PB7 - free, (ADC1)
GPIO_PC2 - SDA, used I2C
GPIO_PC3 - SCL, used I2C
GPIO_PC4 - free
GPIO_PD2 - CS/PWM, free
GPIO_PD7 - free

 *  MHO-C122 LCD buffer:  byte.bit

         --0.4--         --1.4--            --2.4--
  |    |         |     |         |        |         |
  |   0.0       0.5   1.0       1.5      2.0       2.5
  |    |         |     |         |        |         |      o 3.0
 0.3     --0.1--         --1.1--            --2.1--          +--- 3.0
  |    |         |     |         |        |         |     3.0|
  |   0.2       0.6   1.2       1.6      2.2       2.6       ---- 3.1
  |    |         |     |         |        |         |     3.0|
         --0.7--         --1.7--     *      --2.7--          ---- 3.2
                                    2.3
           --5.3--         --4.3--              1.3       1.3            
 (|)     |         |     |         |            / \       / \      
 3.6    5.6       5.2   4.6       4.2     1.3(  ___  3.1  ___  )1.3
         |         |     |         |            1.3 /3.6\ 1.3      
 BLE       --5.5--         --4.5--                  _____
 4.7     |         |     |         |                \3.5/
        5.4       5.1   4.4       4.1     	          
 BAT     |         |     |         |                  %
 5.7       --5.0--         --4.0--                   3.4

None: 3.3, 3.7 ?
*/

#define BLE_MODEL_STR		"MHO-C122"
#define BLE_MAN_STR			"miaomiaoce.com"

#define ZCL_BASIC_MFG_NAME     {10,'M','i','a','o','M','i','a','o','C','e'} // MiaoMiaoCe
#define ZCL_BASIC_MODEL_ID	   {10,'M','H','O','-','C','1','2','2','-','z'} // MHO-C122

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2032

// DISPLAY
#define SHOW_SMILEY			1
#define	USE_DISPLAY			3
#define LCD_BUF_SIZE		6
#define PULL_WAKEUP_SRC_PB6 PM_PIN_PULLUP_10K // LCD ?
#define LCD_INIT_DELAY()	// pm_wait_ms(50)
#define USE_DISPLAY_CONNECT_SYMBOL		1

// BUTTON
#define BUTTON1               		GPIO_PA5  // reset test point
#define BUTTON1_ON			0
#define BUTTON1_OFF			1
#define PA5_FUNC			  		AS_GPIO
#define PA5_OUTPUT_ENABLE	  		0
#define PA5_INPUT_ENABLE	  		1
#define	PULL_WAKEUP_SRC_PA5	  		PM_PIN_PULLUP_10K

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
#define USE_SENSOR_AHT20_30		0
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		1
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

#endif // (BOARD == BOARD_MHO_C122)
#endif /* _BOARD_MHO_C122_H_ */

