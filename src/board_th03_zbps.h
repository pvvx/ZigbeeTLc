/********************************************************************************************************
 * @file    board_th03_zbps.h
 *
 * @brief   Tuya TH03 Zigbee LCD thermo-hygrometer, PCB "TH03_ZBPS1.0"
 *          (reports itself as "Zbeacon"/"TH01" with stock firmware, OTA 1286-0202-10033607).
 *
 *          Hardware map from TheMysticle (BLE port for ATC_MiThermometer):
 *          https://github.com/TheMysticle/ATC_MiThermometer/blob/th03-zbps1.0-support/TH03_Hardware_Info.md
 *          https://github.com/pvvx/ATC_MiThermometer/discussions/791
 *
 *******************************************************************************************************/
#ifndef _BOARD_TH03_ZBPS_H_
#define _BOARD_TH03_ZBPS_H_

#include "version_cfg.h"

#if (BOARD == BOARD_TH03_ZBPS)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_SCREEN | SERVICE_THS)

/*
TLSR8656F512ET32 (TLSR825x)

GPIO_PA0 - SDA, used I2C Sensor (GXHTV4 = SHT4x clone)
GPIO_PA1 - SCL, used I2C LCD (VKL060)
GPIO_PA7 - SWS
GPIO_PB0 - Sensor power ("1") + VBAT ADC
GPIO_PC0 - DO NOT DRIVE (TheMysticle: driving or configuring it as I/O crashes the board)
GPIO_PC1 - DO NOT DRIVE (same as PC0)
GPIO_PD3 - KEY (to GND)
GPIO_PD4 - SCL, used I2C Sensor
GPIO_PD7 - SDA, used I2C LCD

P2 header (top to bottom): GND, SWS, +3.3V

LCD: VKL060 (BL55028 compatible commands), I2C addr 0x3E, display RAM starts at 0x0B,
needs a fast soft-I2C clock (2 us). Segment map is the same as TH03 (board_zth03.h).
*/

#define BLE_MODEL_STR		"TH03"
#define BLE_MAN_STR			"ZBeacon"

#define ZCL_BASIC_MFG_NAME     {7,'Z','B','e','a','c','o','n'} // ZBeacon
#define ZCL_BASIC_MODEL_ID	   {11,'T','H','0','3','-','Z','B','P','S','-','z'} // TH03-ZBPS-z

// Battery & RF Power
#define USE_BATTERY 	BATTERY_CR2032

// DISPLAY
#define SHOW_SMILEY			1
#define	USE_DISPLAY			1
#define LCD_BUF_SIZE		6
#define LCD_CMP_BUF_SIZE	7
#define LCD_INIT_DELAY()
#define USE_DISPLAY_OFF		1
#define USE_DISPLAY_CONNECT_SYMBOL 2
#define LCD_VKL060			1 // TH03 segment map on a VKL060 controller (see lcd_zth03.c)

// KEY, BUTTON
#define BUTTON1				GPIO_PD3
#define BUTTON1_ON			0
#define PD3_INPUT_ENABLE	1
#define PD3_DATA_OUT		0
#define PD3_OUTPUT_ENABLE	0
#define PD3_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PD3 PM_PIN_PULLUP_10K

// LED: none on this PCB (and PC0/PC1 must not be used, see above)

// I2C Sensor (same pins as BOARD_ZBEACON2_TH01)
#define	USE_I2C_DRV			I2C_DRV_SOFT // Soft I2C
#define	I2C_CLOCK			400000

// I2C - SCL
#define I2C_SCL 			GPIO_PD4
#define PD4_FUNC			AS_GPIO
#define PD4_INPUT_ENABLE	1
#define PD4_OUTPUT_ENABLE	0
#define PD4_DATA_OUT		0
#define PULL_WAKEUP_SRC_PD4	PM_PIN_PULLUP_10K

// I2C - SDA
#define I2C_SDA 			GPIO_PA0
#define PA0_FUNC			AS_GPIO
#define PA0_INPUT_ENABLE	1
#define PA0_OUTPUT_ENABLE	0
#define PA0_DATA_OUT		0
#define PULL_WAKEUP_SRC_PA0	PM_PIN_PULLUP_10K

// Sensor T&H
#define USE_SENSOR_CHT8305		0
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		0
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		0
#define USE_SENSOR_SHT30		0

// I2C LCD (VKL060): PA1 = SCL, PD7 = SDA
#define I2C_SCL_LCD			GPIO_PA1
#define I2C_SDA_LCD			GPIO_PD7
#define PA1_INPUT_ENABLE	1
#define PA1_DATA_OUT		0
#define PA1_OUTPUT_ENABLE	0
#define PA1_FUNC			AS_GPIO
#define PD7_INPUT_ENABLE	1
#define PD7_DATA_OUT		0
#define PD7_OUTPUT_ENABLE	0
#define PD7_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PA1	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PD7	PM_PIN_PULLUP_10K

// VBAT + sensor power
#define SHL_ADC_VBAT		B0P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PB0
#define PB0_INPUT_ENABLE	0
#define PB0_DATA_OUT		1
#define PB0_OUTPUT_ENABLE	1
#define PB0_FUNC			AS_GPIO

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif

#endif // (BOARD == BOARD_TH03_ZBPS)
#endif // _BOARD_TH03_ZBPS_H_
