/********************************************************************************************************
 * @file    app_cfg.h
 *
 * @brief   This is the header file for app_cfg
 *
 *******************************************************************************************************/
#ifndef _APP_CFG_H_
#define _APP_CFG_H_

/**********************************************************************
 * Version configuration
 */
#include "version_cfg.h"

/**********************************************************************
 * Product Information
 */
/* Debug mode config */
#define	DEBUG_ENABLE					0 // lcd = DeviceSysException

/* Flash protect module */
/* Only the firmware area will be locked, the NV data area will not be locked.
 * For details, please refer to drv_flash.c file.
 */
#define FLASH_PROTECT_ENABLE			0

#define	UART_PRINTF_MODE				0
#define USB_PRINTF_MODE         		0

/* HCI interface */
#define ZBHCI_BLE						0  // not used !

/* PM */
#define PM_ENABLE						1  // always = 1

/* PA */
#define PA_ENABLE						0  // not used !

/**********************************************************************
 * Clock & Power
 */
#define CLOCK_SYS_CLOCK_HZ  		24000000 //48000000

/**********************************************************************
 * I2C driver type
 */
#define I2C_DRV_NONE	0
#define I2C_DRV_HARD	1
#define I2C_DRV_SOFT	2

/**********************************************************************
 * Product services
 * Supported services by the device (bits)
 */
#define SERVICE_OTA			0x00000001	// OTA all enable!
#define SERVICE_OTA_EXT		0x00000002	// Compatible BigOTA/ZigbeeOTA
#define SERVICE_PINCODE 	0x00000004	// support pin-code
#define SERVICE_BINDKEY 	0x00000008	// support encryption beacon (bindkey)
#define SERVICE_HISTORY 	0x00000010	// flash logger enable
#define SERVICE_SCREEN		0x00000020	// screen
#define SERVICE_LE_LR		0x00000040	// support extension advertise + LE Long Range
#define SERVICE_THS			0x00000080	// T & H sensor
#define SERVICE_RDS			0x00000100	// wake up when the reed switch + pulse counter
#define SERVICE_KEY			0x00000200	// key "connect"
#define SERVICE_OUTS		0x00000400	// GPIO output
#define SERVICE_INS			0x00000800	// GPIO input
#define SERVICE_TIME_ADJUST 0x00001000	// time correction enabled
#define SERVICE_HARD_CLOCK	0x00002000	// RTC enabled
#define SERVICE_TH_TRG		0x00004000	// use TH trigger out
#define SERVICE_LED			0x00008000	// use led
#define SERVICE_MI_KEYS		0x00010000	// use mi keys
#define SERVICE_PRESSURE	0x00020000	// pressure sensor
#define SERVICE_18B20		0x00040000	// use sensor(s) MY18B20
#define SERVICE_IUS			0x00080000	// use I and U sensor (INA226)
#define SERVICE_PLM			0x00100000	// use PWM-RH and NTC
#define SERVICE_BUTTON		0x00200000	// брелок-кнопка
#define SERVICE_FINDMY		0x00400000	// FindMy
#define SERVICE_SCANTIM		0x00800000	// Scan Time (develop, test only!)
#define SERVICE_ZIGBEE		0x01000000	// ZB-version
#define SERVICE_PIR			0x02000000	// use PIR sensor
#define SERVICE_EXTENDED	0x80000000  //


/**********************************************************************
 *  Battery type
 */
#define BATTERY_CR2032		0
#define BATTERY_CR2430		1
#define BATTERY_CR2450		2
#define BATTERY_2AAA		3


/* Board include */
#if defined(BOARD)
#include "board_cgg1.h"
#include "board_cgg1n.h"
#include "board_lyws03mmc.h"
#include "board_cgdk2.h"
#include "board_mho_c122.h"
#include "board_mho_c401.h"
#include "board_mho_c401n.h"
#include "board_ts0201_tz3000.h"
#include "board_th03z.h"
#include "board_zth01_02.h"
#include "board_zth03.h"
#include "board_zth05.h"
#include "board_lktmzl02.h"
#include "board_cb3s.h"
#include "board_zyzth02.h"
#include "board_zyzth02pro.h"
#include "board_zg_227z.h"
#include "board_pirs.h"
#include "board_mjwsd06mmc.h"
#else
#error "Define BOARD!"
#endif

/**********************************************************************
 * Battery & RF Power
 */
#ifndef USE_BATTERY
#define USE_BATTERY 	BATTERY_CR2032
#endif

#if USE_BATTERY == BATTERY_CR2032

#define ZB_TX_POWER_IDX_DEF				RF_POWER_INDEX_P1p99dBm	// TX 8 mA
#define	BLE_TX_POWER_DEF				RF_POWER_P1p99dBm		// TX 8 mA

#elif USE_BATTERY == BATTERY_CR2430

#define ZB_TX_POWER_IDX_DEF				RF_POWER_INDEX_P2p39dBm
#define	BLE_TX_POWER_DEF				RF_POWER_P2p39dBm

#elif USE_BATTERY == BATTERY_CR2450

#define ZB_TX_POWER_IDX_DEF				RF_POWER_INDEX_P2p61dBm
#define	BLE_TX_POWER_DEF				RF_POWER_P2p61dBm

#elif USE_BATTERY == BATTERY_2AAA

#define ZB_TX_POWER_IDX_DEF				RF_POWER_INDEX_P3p01dBm
#define	BLE_TX_POWER_DEF				RF_POWER_P3p01dBm

#endif

#if USE_BLE
#define	ZB_DEFAULT_TX_POWER_IDX		BLE_TX_POWER_DEF
#define	BLE_DEFAULT_TX_POWER		BLE_TX_POWER_DEF
#else
#define	ZB_DEFAULT_TX_POWER_IDX		ZB_TX_POWER_IDX_DEF
#define	BLE_DEFAULT_TX_POWER		ZB_TX_POWER_IDX_DEF
#endif

/* Voltage detect module */
#define VOLTAGE_DETECT_ENABLE		0 // always = 0!
#define VOLTAGE_DETECT_ADC_PIN 		GPIO_VBAT

/**********************************************************************
 * Sensor configuration
 */
#ifndef USE_SENSOR_TH
#define USE_SENSOR_TH  (USE_SENSOR_CHT8305 || USE_SENSOR_CHT8215 || USE_SENSOR_AHT20_30 || USE_SENSOR_SHT4X || USE_SENSOR_SHTC3 || USE_SENSOR_SHT30)
#endif


#define DEF_OCCUPANCY_DELAY		60 // sec

#if USE_SENSOR_TH
#define READ_SENSOR_TIMER_MIN_SEC 	3 // second
#define READ_SENSOR_TIMER_MAX_SEC 	30 // second
#define READ_SENSOR_TIMER_SEC 		10 // default, second
#define DEFAULT_POLL_RATE			(g_zcl_thermostatUICfgAttrs.measure_interval * (4 * POLL_RATE_QUARTERSECONDS)) //   (READ_SENSOR_TIMER_SEC * (4 * POLL_RATE_QUARTERSECONDS)) // msecond
#else
#define READ_SENSOR_TIMER_MIN_SEC 	3 // second
#define READ_SENSOR_TIMER_SEC 		30 // default, second
#define DEFAULT_POLL_RATE			(READ_SENSOR_TIMER_SEC * (4 * POLL_RATE_QUARTERSECONDS)) //   (READ_SENSOR_TIMER_SEC * (4 * POLL_RATE_QUARTERSECONDS)) // msecond
#endif
#define READ_SENSOR_TIMER_MS 		DEFAULT_POLL_RATE // (READ_SENSOR_TIMER_SEC*1000) // msecond

/**********************************************************************
 * NVM configuration
 */
// ID compatible NV: (0x3D000000 | (APP_RELEASE<<16) | (APP_BUILD<<8) | DEVICE)
// Used NV_MODULE_APP Flash: 0x7a000 - 0x7c000
#define USE_NV_APP	(0x3D012400 | BOARD)	// ID, not change!

typedef enum{
	NV_ITEM_APP_DEV_VER	= 0x60,
	NV_ITEM_APP_DEV_NAME,
	NV_ITEM_APP_MAN_NAME,
	NV_ITEM_APP_THERMOSTAT_UI_CFG,
	NV_ITEM_APP_PIR_CFG,
} nv_item_app_t;

/**********************************************************************
 * ZCL cluster support setting
 */
#define ZCL_ON_OFF_SUPPORT				0 // =0 (!)
#define ZCL_LEVEL_CTRL_SUPPORT			0 // =0 (!)
#define ZCL_LIGHT_COLOR_CONTROL_SUPPORT	0 // =0 (!)
#define ZCL_POWER_CFG_SUPPORT						1
//#define ZCL_IAS_ZONE_SUPPORT						1
#if (DEV_SERVICES & SERVICE_THS)
#define ZCL_TEMPERATURE_MEASUREMENT_SUPPORT			1
#define ZCL_RELATIVE_HUMIDITY_SUPPORT   			1
#define ZCL_THERMOSTAT_UI_CFG_SUPPORT				1
#endif
#if (DEV_SERVICES & SERVICE_PIR)
#define ZCL_OCCUPANCY_SENSING_SUPPORT				1
#endif
#define ZCL_POLL_CTRL_SUPPORT						1
#define ZCL_GROUP_SUPPORT							0
#define ZCL_OTA_SUPPORT								1
#define TOUCHLINK_SUPPORT							0
#define FIND_AND_BIND_SUPPORT						0
#if TOUCHLINK_SUPPORT
#define ZCL_ZLL_COMMISSIONING_SUPPORT				1
#endif
#define REJOIN_FAILURE_TIMER						1
#define USE_CHG_NAME								1

// for consistency
#if ZCL_RELATIVE_HUMIDITY_SUPPORT
#define ZCL_RELATIVE_HUMIDITY
#define ZCL_RELATIVE_HUMIDITY_MEASUREMENT
#endif
#if ZCL_THERMOSTAT_UI_CFG_SUPPORT
#define ZCL_THERMOSTAT_UI_CFG				1
#endif
/**********************************************************************
 * BLE configuration
 */
#define APP_SECURITY_ENABLE      			0
#define APP_DIRECT_ADV_ENABLE				1
#define BLE_APP_PM_ENABLE					PM_ENABLE
#define PM_DEEPSLEEP_RETENTION_ENABLE		PM_ENABLE

#define USE_DEVICE_INFO_CHR_UUID			1
#define USE_FLASH_SERIAL_UID				1
#define USE_BLE_OTA							ZCL_OTA_SUPPORT

/**********************************************************************
 * Modules configuration
 */

/* Configure startup */
#define STARTUP_IN_BLE				1	// always =1 (if BLE)!

/* Watch dog module */
#define MODULE_WATCHDOG_ENABLE						0

/* UART module */
#define	MODULE_UART_ENABLE							0

#if (ZBHCI_USB_PRINT || ZBHCI_USB_CDC || ZBHCI_USB_HID || ZBHCI_UART)
	#define ZBHCI_EN								1
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

#endif // _APP_CFG_H_
