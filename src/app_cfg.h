/********************************************************************************************************
 * @file    app_cfg.h
 *
 * @brief   This is the header file for app_cfg
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
#ifndef _APP_CFG_H_
#define _APP_CFG_H_

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif


/**********************************************************************
 * Version configuration
 */
#include "version_cfg.h"

/**********************************************************************
 * Product Information
 */
/* Debug mode config */
#define	DEBUG_ENABLE					0 // lcd = DeviceSysException

#define	UART_PRINTF_MODE				0
#define USB_PRINTF_MODE         		0

/* PM */
#define PM_ENABLE						1

/* PA */
#define PA_ENABLE						0

#define CLOCK_SYS_CLOCK_HZ  		24000000 //48000000

#define I2C_DRV_NONE	0
#define I2C_DRV_HARD	1
#define I2C_DRV_SOFT	2

#define BATTERY_CR2032		0
#define BATTERY_CR2430		1
#define BATTERY_CR2450		2
#define BATTERY_2AAA		3

/* Board include */
#if defined(BOARD)
#include "board_lyws03mmc.h"
#include "board_cgdk2.h"
#include "board_mho_c122.h"
#include "board_mho_c401.h"
#include "board_mho_c401n.h"
#include "board_ts0201_tz3000.h"
#include "board_th03z.h"
#include "board_zth01_02.h"
#include "board_zth03.h"
#include "board_lktmzl02.h"
#include "board_cb3s.h"
#include "board_zyzth02.h"
#include "board_zg_227z.h"
#else
#error "Define BOARD!"
#endif

// Battery & RF Power
#ifndef USE_BATTERY
#define USE_BATTERY 	BATTERY_CR2032
#endif
#ifndef RF_TX_POWER_DEF
#if USE_BATTERY == BATTERY_CR2032
#define RF_TX_POWER_DEF		RF_POWER_INDEX_P1p99dBm
#elif USE_BATTERY == BATTERY_CR2430
#define RF_TX_POWER_DEF		RF_POWER_INDEX_P2p39dBm
#elif USE_BATTERY == BATTERY_CR2450
#define RF_TX_POWER_DEF		RF_POWER_INDEX_P2p61dBm
#elif USE_BATTERY == BATTERY_2AAA
#define RF_TX_POWER_DEF		RF_POWER_INDEX_P3p01dBm
#else
#error "Set BATTERY!"
#endif
#endif

#ifndef USE_SENSOR_TH
#define USE_SENSOR_TH  (USE_SENSOR_CHT8305 || USE_SENSOR_CHT8215 || USE_SENSOR_AHT20_30 || USE_SENSOR_SHT4X || USE_SENSOR_SHTC3 || USE_SENSOR_SHT30)
#endif

#ifndef ZIGBEE_TUYA_OTA
#define ZIGBEE_TUYA_OTA 	0
#endif

#define USE_CHG_NAME	1

#define VOLTAGE_DETECT_ADC_PIN GPIO_VBAT

#define READ_SENSOR_TIMER_MIN_SEC 	3 // second
#define READ_SENSOR_TIMER_SEC 		10 // default, second
#define DEFAULT_POLL_RATE			(g_zcl_thermostatUICfgAttrs.measure_interval * (4 * POLL_RATE_QUARTERSECONDS)) //   (READ_SENSOR_TIMER_SEC * (4 * POLL_RATE_QUARTERSECONDS)) // msecond
#define READ_SENSOR_TIMER_MS 		DEFAULT_POLL_RATE // (READ_SENSOR_TIMER_SEC*1000) // msecond

/* Voltage detect module */
#define VOLTAGE_DETECT_ENABLE						0

/* Watch dog module */
#define MODULE_WATCHDOG_ENABLE						0

/* UART module */
#define	MODULE_UART_ENABLE							0

#if (ZBHCI_USB_PRINT || ZBHCI_USB_CDC || ZBHCI_USB_HID || ZBHCI_UART)
	#define ZBHCI_EN								1
#endif


/**********************************************************************
 * ZCL cluster support setting
 */
#define ZCL_POWER_CFG_SUPPORT						1
//#define ZCL_IAS_ZONE_SUPPORT						1
#define ZCL_TEMPERATURE_MEASUREMENT_SUPPORT			1
#define ZCL_RELATIVE_HUMIDITY_SUPPORT   			1
#define ZCL_THERMOSTAT_UI_CFG_SUPPORT				1
#define ZCL_POLL_CTRL_SUPPORT						1
#define ZCL_GROUP_SUPPORT							0
#define ZCL_OTA_SUPPORT								1
#define TOUCHLINK_SUPPORT							0
#define FIND_AND_BIND_SUPPORT						0
#if TOUCHLINK_SUPPORT
#define ZCL_ZLL_COMMISSIONING_SUPPORT				1
#endif
#define REJOIN_FAILURE_TIMER						1

// for consistency
#if ZCL_RELATIVE_HUMIDITY_SUPPORT
#define ZCL_RELATIVE_HUMIDITY
#define ZCL_RELATIVE_HUMIDITY_MEASUREMENT
#endif
#if ZCL_THERMOSTAT_UI_CFG_SUPPORT
#define ZCL_THERMOSTAT_UI_CFG
#define NV_ITEM_ZCL_THERMOSTAT_UI_CFG       (NV_ITEM_APP_GP_TRANS_TABLE + 1)    // see sdk/proj/drivers/drv_nv.h
#endif


/**********************************************************************
 * Stack configuration
 */
#include "includes/zb_config.h"
#include "stack_cfg.h"


/**********************************************************************
 * EV configuration
 */
typedef enum{
	EV_POLL_ED_DETECT,
	EV_POLL_HCI,
    EV_POLL_IDLE,
	EV_POLL_MAX,
}ev_poll_e;

/**********************************************************************
 * Sensor configuration
 */

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

#endif // _APP_CFG_H_
