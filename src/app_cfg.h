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

/* BDB */
#define TOUCHLINK_SUPPORT				0
#define FIND_AND_BIND_SUPPORT			0

#define CLOCK_SYS_CLOCK_HZ  		24000000 //48000000

#define SENSOR_SHTXX	1
#define SENSOR_CHT8305	2

/* Board include */
#if defined(BOARD)
//#if (BOARD == BOARD_LYWSD03MMC)
	#include "board_lyws03mmc.h"
//#elif (BOARD == BOARD_CGDK2)
	#include "board_cgdk2.h"
//#elif BOARD == BOARD_MHO_C122
	#include "board_mho_c122.h"
//#elif BOARD == BOARD_MHO_C401N
#include "board_mho_c401n.h"
//#elif BOARD == BOARD_TS0201_TZ3000
#include "board_ts0201_tz3000.h"
//#else
//#error "Define BOARD!"
//#endif
#else
#error "Define BOARD!"
#endif

#define VOLTAGE_DETECT_ADC_PIN GPIO_VBAT

#define READ_SENSOR_TIMER_SEC 	10 // second
#define READ_SENSOR_TIMER_MS 	(READ_SENSOR_TIMER_SEC*1000) // msecond

/* Voltage detect module */
/* If VOLTAGE_DETECT_ENABLE is set,
 * 1) if MCU_CORE_826x is defined, the DRV_ADC_VBAT_MODE mode is used by default,
 * and there is no need to configure the detection IO port;
 * 2) if MCU_CORE_8258 or MCU_CORE_8278 is defined, the DRV_ADC_VBAT_MODE mode is used by default,
 * we need to configure the detection IO port, and the IO must be in a floating state.
 * 3) if MCU_CORE_B91 is defined, the DRV_ADC_BASE_MODE mode is used by default,
 * we need to configure the detection IO port, and the IO must be connected to the target under test,
 * such as VCC.
 */
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
#define ZCL_THERMOSTAT_UI_CFG_SUPPORT				USE_DISPLAY
//#define ZCL_POLL_CTRL_SUPPORT						1
#define ZCL_OTA_SUPPORT								1
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

#define DEFAULT_POLL_RATE					(10 * (4 * POLL_RATE_QUARTERSECONDS))

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
