/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zcl_include.h"
#include "zcl_relative_humidity.h"
#include "zcl_thermostat_ui_cfg.h"
#include "device.h"
#include "lcd.h"
#if USE_BLE
#include "zigbee_ble_switch.h"
#endif
#if USE_TRIGGER
#include "trigger.h"
#endif

/**********************************************************************
 * LOCAL CONSTANTS
 */

#if !defined(ZCL_BASIC_MFG_NAME) || !defined(ZCL_BASIC_MODEL_ID)
#error "defined ZCL_BASIC_MODEL_ID & ZCL_BASIC_MFG_NAME !"
#endif

#ifndef ZCL_BASIC_SW_BUILD_ID

#define ZCL_BASIC_SW_BUILD_ID	{9 \
								,'0'+(STACK_RELEASE>>4) \
								,'0'+(STACK_RELEASE & 0xf) \
								,'0'+(STACK_BUILD>>4) \
								,'0'+(STACK_BUILD & 0xf) \
								,'-' \
								,'0'+(APP_RELEASE>>4) \
								,'0'+(APP_RELEASE & 0xf) \
								,'0'+(APP_BUILD>>4) \
								,'0'+(APP_BUILD & 0xf) \
								}

#endif

#ifndef ZCL_BASIC_DATE_CODE
#define ZCL_BASIC_DATE_CODE     {8,'0','0','0','0','0','0','0','0'}
#endif

/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * GLOBAL VARIABLES
 */
/**
 *  @brief Definition for Incoming cluster / Sever Cluster
 */
const u16 sensorDevice_inClusterList[] =
{
	ZCL_CLUSTER_GEN_BASIC,
	ZCL_CLUSTER_GEN_POWER_CFG,
	ZCL_CLUSTER_GEN_IDENTIFY,
#ifdef ZCL_POLL_CTRL
	ZCL_CLUSTER_GEN_POLL_CONTROL,
#endif
#ifdef ZCL_THERMOSTAT_UI_CFG
	ZCL_CLUSTER_HAVC_USER_INTERFACE_CONFIG,
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT
	ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT,
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_MEASUREMENT
    ZCL_CLUSTER_MS_RELATIVE_HUMIDITY,
#endif
#ifdef ZCL_OCCUPANCY_SENSING
	ZCL_CLUSTER_MS_OCCUPANCY_SENSING,
#endif
#ifdef ZCL_IAS_ZONE
	ZCL_CLUSTER_SS_IAS_ZONE,
#endif
};

#if (DEV_SERVICES & SERVICE_PLM)
const u16 sensorDevice_inClusterList2[] =
{
    ZCL_CLUSTER_MS_RELATIVE_HUMIDITY
};
#endif

/**
 *  @brief Definition for Outgoing cluster / Client Cluster
 */
const u16 sensorDevice_outClusterList[] =
{
#ifdef ZCL_GROUP
	ZCL_CLUSTER_GEN_GROUPS,
#endif
#ifdef ZCL_ON_OFF
	ZCL_CLUSTER_GEN_ON_OFF,
#endif
#ifdef ZCL_OTA
    ZCL_CLUSTER_OTA,
#endif
};

/**
 *  @brief Definition for Server cluster number and Client cluster number
 */
#define sensorDevice_IN_CLUSTER_NUM		(sizeof(sensorDevice_inClusterList)/sizeof(sensorDevice_inClusterList[0]))
#define sensorDevice_OUT_CLUSTER_NUM	(sizeof(sensorDevice_outClusterList)/sizeof(sensorDevice_outClusterList[0]))
#if (DEV_SERVICES & SERVICE_PLM)
#define sensorDevice_IN_CLUSTER_NUM2		(sizeof(sensorDevice_inClusterList2)/sizeof(sensorDevice_inClusterList2[0]))
#endif

/**
 *  @brief Definition for simple description for HA profile
 */
const af_simple_descriptor_t sensorDevice_simpleDesc =
{
	HA_PROFILE_ID,                      	/* Application profile identifier */
	HA_DEV_TEMPERATURE_SENSOR,              /* Application device identifier */
	SENSOR_DEVICE_ENDPOINT,         		/* Endpoint */
	1,										/* Application device version */
	0,										/* Reserved */
	sensorDevice_IN_CLUSTER_NUM,           	/* Application input cluster count */
	sensorDevice_OUT_CLUSTER_NUM,          	/* Application output cluster count */
	(u16 *)sensorDevice_inClusterList,    	/* Application input cluster list */
	(u16 *)sensorDevice_outClusterList,   	/* Application output cluster list */
};

#if (DEV_SERVICES & SERVICE_PLM)
const af_simple_descriptor_t sensorDevice_simpleDesc2 =
{
	HA_PROFILE_ID,                      	/* Application profile identifier */
	HA_DEV_TEMPERATURE_SENSOR,              		/* Application device identifier */
	SENSOR_DEVICE_ENDPOINT2,         		/* Endpoint */
	1,										/* Application device version */
	0,										/* Reserved */
	sensorDevice_IN_CLUSTER_NUM2,           	/* Application input cluster count */
	0,//	sensorDevice_OUT_CLUSTER_NUM2,          	/* Application output cluster count */
	(u16 *)sensorDevice_inClusterList2,    	/* Application input cluster list */
	0 //	(u16 *)sensorDevice_outClusterList1,   	/* Application output cluster list */
};
#endif

/* Basic */
zcl_basicAttr_t g_zcl_basicAttrs =
{
	.zclVersion 	= 0x03,
	.appVersion 	= APP_BUILD,
	.stackVersion 	= (STACK_RELEASE|STACK_BUILD),
#if BOARD == BOARD_LYWSD03MMC
	.hwVersion		= 0x00,
#else
	.hwVersion		= BOARD,
#endif
	.manuName		= ZCL_BASIC_MFG_NAME,
	.modelId		= ZCL_BASIC_MODEL_ID,
#ifdef ZCL_BASIC_SW_BUILD_ID
	.swBuildId		= ZCL_BASIC_SW_BUILD_ID,
#endif
#ifdef ZCL_BASIC_DATE_CODE
	.dateCode 		= ZCL_BASIC_DATE_CODE,
#endif
	.powerSource	= POWER_SOURCE_BATTERY,
	.deviceEnable	= TRUE,
};

const zclAttrInfo_t basic_attrTbl[] =
{
	{ ZCL_ATTRID_BASIC_ZCL_VER,      		ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ,  						(u8*)&g_zcl_basicAttrs.zclVersion},
	{ ZCL_ATTRID_BASIC_APP_VER,      		ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ,  						(u8*)&g_zcl_basicAttrs.appVersion},
	{ ZCL_ATTRID_BASIC_STACK_VER,    		ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ,  						(u8*)&g_zcl_basicAttrs.stackVersion},
	{ ZCL_ATTRID_BASIC_HW_VER,       		ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ,  						(u8*)&g_zcl_basicAttrs.hwVersion},
#if USE_CHG_NAME
	{ ZCL_ATTRID_BASIC_MFR_NAME,     		ZCL_DATA_TYPE_CHAR_STR, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE,	(u8*)g_zcl_basicAttrs.manuName},
	{ ZCL_ATTRID_BASIC_MODEL_ID,     		ZCL_DATA_TYPE_CHAR_STR, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE,	(u8*)g_zcl_basicAttrs.modelId},
#else
	{ ZCL_ATTRID_BASIC_MFR_NAME,     		ZCL_DATA_TYPE_CHAR_STR, ACCESS_CONTROL_READ,  						(u8*)g_zcl_basicAttrs.manuName},
	{ ZCL_ATTRID_BASIC_MODEL_ID,     		ZCL_DATA_TYPE_CHAR_STR, ACCESS_CONTROL_READ,						(u8*)g_zcl_basicAttrs.modelId},
#endif
	{ ZCL_ATTRID_BASIC_POWER_SOURCE, 		ZCL_DATA_TYPE_ENUM8,    ACCESS_CONTROL_READ,  						(u8*)&g_zcl_basicAttrs.powerSource},
	{ ZCL_ATTRID_BASIC_DEV_ENABLED,  		ZCL_DATA_TYPE_BOOLEAN,  ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_basicAttrs.deviceEnable},
	{ ZCL_ATTRID_BASIC_SW_BUILD_ID,  		ZCL_DATA_TYPE_CHAR_STR, ACCESS_CONTROL_READ,  						(u8*)&g_zcl_basicAttrs.swBuildId},
	{ ZCL_ATTRID_BASIC_DATE_CODE,           ZCL_DATA_TYPE_CHAR_STR, ACCESS_CONTROL_READ,                        (u8*)g_zcl_basicAttrs.dateCode},

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

#define ZCL_BASIC_ATTR_NUM 				sizeof(basic_attrTbl) / sizeof(zclAttrInfo_t)


/* Identify */
zcl_identifyAttr_t g_zcl_identifyAttrs =
{
	.identifyTime	= 0x0000,
};

const zclAttrInfo_t identify_attrTbl[] =
{
	{ ZCL_ATTRID_IDENTIFY_TIME,  			ZCL_DATA_TYPE_UINT16,   ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_identifyAttrs.identifyTime },

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

#define ZCL_IDENTIFY_ATTR_NUM			sizeof(identify_attrTbl) / sizeof(zclAttrInfo_t)

/* power */
zcl_powerAttr_t g_zcl_powerAttrs =
{
    .batteryVoltage    = 30, //in 100 mV units, 0xff - unknown
    .batteryPercentage = 200 //in 0,5% units, 0xff - unknown
};

const zclAttrInfo_t powerCfg_attrTbl[] =
{
	{ ZCL_ATTRID_BATTERY_VOLTAGE,      		   ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE,	(u8*)&g_zcl_powerAttrs.batteryVoltage},
	{ ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING, ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_powerAttrs.batteryPercentage},

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

#define	ZCL_POWER_CFG_ATTR_NUM		 sizeof(powerCfg_attrTbl) / sizeof(zclAttrInfo_t)


#ifdef ZCL_IAS_ZONE
/* IAS Zone */
zcl_iasZoneAttr_t g_zcl_iasZoneAttrs =
{
	.zoneState		= ZONE_STATE_NOT_ENROLLED,
	.zoneType		= ZONE_TYPE_CONTACT_SWITCH,
	.zoneStatus		= 0x00,
	.iasCieAddr		= {0x00},
	.zoneId 		= ZCL_ZONE_ID_INVALID,
};

const zclAttrInfo_t iasZone_attrTbl[] =
{
	{ ZCL_ATTRID_ZONE_STATE,   ZCL_DATA_TYPE_ENUM8,     ACCESS_CONTROL_READ,  						(u8*)&g_zcl_iasZoneAttrs.zoneState },
	{ ZCL_ATTRID_ZONE_TYPE,    ZCL_DATA_TYPE_ENUM16,    ACCESS_CONTROL_READ,  						(u8*)&g_zcl_iasZoneAttrs.zoneType },
	{ ZCL_ATTRID_ZONE_STATUS,  ZCL_DATA_TYPE_BITMAP16,  ACCESS_CONTROL_READ,  						(u8*)&g_zcl_iasZoneAttrs.zoneStatus },
	{ ZCL_ATTRID_IAS_CIE_ADDR, ZCL_DATA_TYPE_IEEE_ADDR, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)g_zcl_iasZoneAttrs.iasCieAddr },
	{ ZCL_ATTRID_ZONE_ID,	   ZCL_DATA_TYPE_UINT8,     ACCESS_CONTROL_READ,  						(u8*)&g_zcl_iasZoneAttrs.zoneId},

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, ZCL_DATA_TYPE_UINT16,  ACCESS_CONTROL_READ,  				(u8*)&zcl_attr_global_clusterRevision},
};

#define	ZCL_IASZONE_ATTR_NUM		 sizeof(iasZone_attrTbl) / sizeof(zclAttrInfo_t)
#endif



#ifdef ZCL_TEMPERATURE_MEASUREMENT
zcl_temperatureAttr_t g_zcl_temperatureAttrs =
{
	.measuredValue	= 0x8000,
	.minValue 		= -5000,
	.maxValue		= 17500,
	.tolerance		= 0,
};

const zclAttrInfo_t temperature_measurement_attrTbl[] =
{
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MEASUREDVALUE,       	ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_temperatureAttrs.measuredValue },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MINMEASUREDVALUE,      ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_temperatureAttrs.minValue },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MAXMEASUREDVALUE,      ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_temperatureAttrs.maxValue },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_TOLERANCE,       		ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_temperatureAttrs.tolerance },

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

#define	ZCL_TEMPERATURE_MEASUREMENT_ATTR_NUM		 sizeof(temperature_measurement_attrTbl) / sizeof(zclAttrInfo_t)
#endif


#ifdef ZCL_RELATIVE_HUMIDITY
zcl_relHumidityAttr_t g_zcl_relHumidityAttrs =
{
	.measuredValue	= 0xffff,
	.minValue 		= 0,
	.maxValue		= 9999,
	.tolerance		= 0,
};

const zclAttrInfo_t relative_humdity_attrTbl[] =
{
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MEASUREDVALUE,       ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_relHumidityAttrs.measuredValue },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MINMEASUREDVALUE,    ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_relHumidityAttrs.minValue },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MAXMEASUREDVALUE,    ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_relHumidityAttrs.maxValue },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_TOLERANCE,      		ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_relHumidityAttrs.tolerance },

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

#define	ZCL_RELATIVE_HUMIDITY_ATTR_NUM		 sizeof(relative_humdity_attrTbl) / sizeof(zclAttrInfo_t)

#if (DEV_SERVICES & SERVICE_PLM)

zcl_MoistureAttr_t g_zcl_MoistureAttrs =
{
	.measuredValue	= 0xffff,
	.minValue 		= 0,
	.maxValue		= 9999,
	.tolerance		= 0,
};

const zclAttrInfo_t relative_humdity_attrTbl2[] =
{
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MEASUREDVALUE,       ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_MoistureAttrs.measuredValue },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MINMEASUREDVALUE,    ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_MoistureAttrs.minValue },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MAXMEASUREDVALUE,    ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_MoistureAttrs.maxValue },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_TOLERANCE,      		ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_MoistureAttrs.tolerance },

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};
#endif

#define	ZCL_RELATIVE_HUMIDITY_ATTR_NUM2		 sizeof(relative_humdity_attrTbl2) / sizeof(zclAttrInfo_t)

#endif // ZCL_RELATIVE_HUMIDITY

#ifdef ZCL_THERMOSTAT_UI_CFG
zcl_thermostatUICfgAttr_t g_zcl_thermostatUICfgAttrs;
const zcl_thermostatUICfgAttr_t g_zcl_thermostatUICfgDefault = {
		.temp_offset = 0,
		.humi_offset = 0,
		.measure_interval = READ_SENSOR_TIMER_SEC,
#if	USE_DISPLAY
		.TemperatureDisplayMode = 0,
#if SHOW_SMILEY
		.showSmiley = 0,
		.temp_comfort_min = 2000,  // +20.00 C  параметры ГОСТ, СНиП
		.temp_comfort_max = 2500,  // +25.00 C
		.humi_comfort_min = 4000,  // 40.00 %
		.humi_comfort_max = 6000  // 60.00 %
#endif
#endif
};

const zclAttrInfo_t thermostat_ui_cfg_attrTbl[] =
{
#if	USE_DISPLAY
	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_TEMPERATUREDISPLAYMODE,       ZCL_DATA_TYPE_ENUM8,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_thermostatUICfgAttrs.TemperatureDisplayMode },
#endif
#if SHOW_SMILEY
	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_SCHEDULEPROGRAMMINGVISIBILITY,   ZCL_DATA_TYPE_ENUM8,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_thermostatUICfgAttrs.showSmiley },
#endif
	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_OFFSET_TEMP,   ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_thermostatUICfgAttrs.temp_offset },
	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_OFFSET_HUMI,   ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_thermostatUICfgAttrs.humi_offset },
#if	SHOW_SMILEY
	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_COMFORT_MIN_T,   ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_thermostatUICfgAttrs.temp_comfort_min },
	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_COMFORT_MAX_T,   ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_thermostatUICfgAttrs.temp_comfort_max },

	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_COMFORT_MIN_H,   ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_thermostatUICfgAttrs.humi_comfort_min },
	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_COMFORT_MAX_H,   ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_thermostatUICfgAttrs.humi_comfort_max },
#endif
#if	USE_DISPLAY
	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_DISPLAY_OFF,   ZCL_DATA_TYPE_ENUM8,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_thermostatUICfgAttrs.display_off },
#endif
	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_MEASURE_INTERVAL,   ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_thermostatUICfgAttrs.measure_interval },

#if USE_TRIGGER
	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_TRIGGER_TRH_T,   ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&trg.temp_threshold },
	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_TRIGGER_HST_T,   ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&trg.temp_hysteresis },
	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_TRIGGER_TRH_H,   ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&trg.humi_threshold },
	{ ZCL_THERMOSTAT_UI_CFG_ATTRID_TRIGGER_HST_H,   ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&trg.humi_hysteresis },
#endif
	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

#define	ZCL_THERMOSTAT_UI_CFG_ATTR_NUM		 sizeof(thermostat_ui_cfg_attrTbl) / sizeof(zclAttrInfo_t)
#endif // ZCL_THERMOSTAT_UI_CFG

#ifdef ZCL_OCCUPANCY_SENSING
zcl_occupancyAttr_t zcl_occupAttr = {
	.occupancy	= 0,
	.sensor_type	= 0,
	.delay	= DEF_OCCUPANCY_DELAY
};

/* Attribute record list */
const zclAttrInfo_t occupancy_attrTbl[] = {
	{ ZCL_ATTRID_OCCUPANCY,  ZCL_DATA_TYPE_BITMAP8,   ACCESS_CONTROL_READ,  (u8*)&zcl_occupAttr.occupancy},
	{ ZCL_ATTRID_OCCUPANCY_SENSOR_TYPE,  ZCL_DATA_TYPE_ENUM8,   ACCESS_CONTROL_READ,  (u8*)&zcl_occupAttr.sensor_type},
	{ ZCL_ATTRID_PIR_OCCU2UNOCCU_DELAY,  ZCL_DATA_TYPE_UINT16,   ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE,  (u8*)&zcl_occupAttr.delay},
	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, ZCL_DATA_TYPE_UINT16,  ACCESS_CONTROL_READ,  (u8*)&zcl_attr_global_clusterRevision},
};

#define	ZCL_OCCUPANCY_ATTR_NUM sizeof(occupancy_attrTbl) / sizeof(zclAttrInfo_t)
//const u8 zcl_occupancy_attrNum = ( sizeof(occupancy_attrTbl) / sizeof(zclAttrInfo_t) );

#endif // ZCL_OCCUPANCY_SENSING

#ifdef ZCL_POLL_CTRL
/* Poll Control */
zcl_pollCtrlAttr_t g_zcl_pollCtrlAttrs =
{
		.chkInInterval			= 3600*4, // 3600 sec, 1 hr
		.longPollInterval		= READ_SENSOR_TIMER_SEC*4,  //  10 sec
		.shortPollInterval		= 2, 	// 2 qs
		.fastPollTimeout		= READ_SENSOR_TIMER_SEC*4,  // 10 sec
		.chkInIntervalMin		= 0, // 2*READ_SENSOR_TIMER_SEC*4, // 20 sec
		.longPollIntervalMin	= READ_SENSOR_TIMER_SEC*4, // 10 sec
		.fastPollTimeoutMax		= READ_SENSOR_TIMER_SEC*4 // 10 sec
};

const zclAttrInfo_t pollCtrl_attrTbl[] =
{
	{ ZCL_ATTRID_CHK_IN_INTERVAL,  		ZCL_DATA_TYPE_UINT32, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_pollCtrlAttrs.chkInInterval },
	{ ZCL_ATTRID_LONG_POLL_INTERVAL, 	ZCL_DATA_TYPE_UINT32, ACCESS_CONTROL_READ, 						  (u8*)&g_zcl_pollCtrlAttrs.longPollInterval },
	{ ZCL_ATTRID_SHORT_POLL_INTERVAL, 	ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, 						  (u8*)&g_zcl_pollCtrlAttrs.shortPollInterval },
	{ ZCL_ATTRID_FAST_POLL_TIMEOUT, 	ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_pollCtrlAttrs.fastPollTimeout },
	{ ZCL_ATTRID_CHK_IN_INTERVAL_MIN, 	ZCL_DATA_TYPE_UINT32, ACCESS_CONTROL_READ, 						  (u8*)&g_zcl_pollCtrlAttrs.chkInIntervalMin},
	{ ZCL_ATTRID_LONG_POLL_INTERVAL_MIN,ZCL_DATA_TYPE_UINT32, ACCESS_CONTROL_READ, 						  (u8*)&g_zcl_pollCtrlAttrs.longPollIntervalMin },
	{ ZCL_ATTRID_FAST_POLL_TIMEOUT_MAX, ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, 						  (u8*)&g_zcl_pollCtrlAttrs.fastPollTimeoutMax},

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, ZCL_DATA_TYPE_UINT16,  ACCESS_CONTROL_READ,  					  (u8*)&zcl_attr_global_clusterRevision},
};

#define	ZCL_POLLCTRL_ATTR_NUM		 sizeof(pollCtrl_attrTbl) / sizeof(zclAttrInfo_t)
#endif

/**
 *  @brief Definition for simple contact sensor ZCL specific cluster
 */
const zcl_specClusterInfo_t g_sensorDeviceClusterList[] =
{
	{ZCL_CLUSTER_GEN_BASIC,			MANUFACTURER_CODE_NONE, ZCL_BASIC_ATTR_NUM, 	basic_attrTbl,  	zcl_basic_register,		sensorDevice_basicCb},
#ifdef ZCL_POWER_CFG
	{ZCL_CLUSTER_GEN_POWER_CFG,		MANUFACTURER_CODE_NONE,	ZCL_POWER_CFG_ATTR_NUM,	powerCfg_attrTbl,	zcl_powerCfg_register,	sensorDevice_powerCfgCb},
#endif
	{ZCL_CLUSTER_GEN_IDENTIFY,		MANUFACTURER_CODE_NONE, ZCL_IDENTIFY_ATTR_NUM,	identify_attrTbl,	zcl_identify_register,	sensorDevice_identifyCb},
#ifdef ZCL_GROUP
	{ZCL_CLUSTER_GEN_GROUPS,		MANUFACTURER_CODE_NONE,	0, 						NULL,  				zcl_group_register,		sensorDevice_groupCb},
#endif
#ifdef ZCL_POLL_CTRL
	{ZCL_CLUSTER_GEN_POLL_CONTROL,  MANUFACTURER_CODE_NONE, ZCL_POLLCTRL_ATTR_NUM, 	pollCtrl_attrTbl,   zcl_pollCtrl_register,	sensorDevice_pollCtrlCb},
#endif
#ifdef ZCL_THERMOSTAT_UI_CFG
	{ZCL_CLUSTER_HAVC_USER_INTERFACE_CONFIG, MANUFACTURER_CODE_NONE, ZCL_THERMOSTAT_UI_CFG_ATTR_NUM, thermostat_ui_cfg_attrTbl,	zcl_thermostat_ui_cfg_register, 	NULL},
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT
	{ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT,	MANUFACTURER_CODE_NONE, ZCL_TEMPERATURE_MEASUREMENT_ATTR_NUM, temperature_measurement_attrTbl, 	zcl_temperature_measurement_register, 	NULL},
#endif
#ifdef ZCL_RELATIVE_HUMIDITY
	{ZCL_CLUSTER_MS_RELATIVE_HUMIDITY,	MANUFACTURER_CODE_NONE, ZCL_RELATIVE_HUMIDITY_ATTR_NUM, 		relative_humdity_attrTbl,	zcl_relative_humidity_register, 	NULL},
#endif
#ifdef ZCL_OCCUPANCY_SENSING
	{ZCL_CLUSTER_MS_OCCUPANCY_SENSING, MANUFACTURER_CODE_NONE, ZCL_OCCUPANCY_ATTR_NUM,	occupancy_attrTbl, zcl_occupancySensing_register, 	NULL},
#endif
#ifdef ZCL_IAS_ZONE
	{ZCL_CLUSTER_SS_IAS_ZONE,		MANUFACTURER_CODE_NONE, ZCL_IASZONE_ATTR_NUM,	iasZone_attrTbl,	zcl_iasZone_register,	sensorDevice_iasZoneCb},
#endif
};

const u8 SENSOR_DEVICE_CB_CLUSTER_NUM = (sizeof(g_sensorDeviceClusterList)/sizeof(g_sensorDeviceClusterList[0]));

#if (DEV_SERVICES & SERVICE_PLM)

const zcl_specClusterInfo_t g_sensorDeviceClusterList2[] =
{
//	{ZCL_CLUSTER_GEN_BASIC,			MANUFACTURER_CODE_NONE, ZCL_BASIC_ATTR_NUM, 	basic_attrTbl,  	zcl_basic_register,		sensorDevice_basicCb},
	{ZCL_CLUSTER_MS_RELATIVE_HUMIDITY,	MANUFACTURER_CODE_NONE, ZCL_RELATIVE_HUMIDITY_ATTR_NUM2, relative_humdity_attrTbl2,	zcl_relative_humidity_register, 	NULL}
};

const u8 SENSOR_DEVICE_CB_CLUSTER_NUM2 = (sizeof(g_sensorDeviceClusterList2)/sizeof(g_sensorDeviceClusterList2[0]));
#endif


#if NV_ENABLE
/**********************************************************************
 * FUNCTIONS NV
 */

#if USE_CHG_NAME

static const u8 modelId[] = ZCL_BASIC_MODEL_ID;
static const u8 manuName[] = ZCL_BASIC_MFG_NAME;

void read_dev_name(void) {
	if(NV_SUCC != nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_APP_DEV_NAME, ZCL_BASIC_MAX_LENGTH, g_zcl_basicAttrs.modelId)
			|| g_zcl_basicAttrs.modelId[0] == 0
			|| g_zcl_basicAttrs.modelId[0] >= ZCL_BASIC_MAX_LENGTH) {
		memcpy(g_zcl_basicAttrs.modelId, modelId, sizeof(modelId));
	}
	if(NV_SUCC != nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_APP_MAN_NAME, ZCL_BASIC_MAX_LENGTH, g_zcl_basicAttrs.manuName)
			|| g_zcl_basicAttrs.manuName[0] == 0
			|| g_zcl_basicAttrs.manuName[0] >= ZCL_BASIC_MAX_LENGTH) {
		memcpy(g_zcl_basicAttrs.manuName, manuName, sizeof(manuName));
	}
}


void save_dev_name(u16 attrID) {
	u8 buf[ZCL_BASIC_MAX_LENGTH];
	if(attrID == ZCL_ATTRID_BASIC_MODEL_ID) {
		if(g_zcl_basicAttrs.modelId[0] == 0 || g_zcl_basicAttrs.modelId[0] >= ZCL_BASIC_MAX_LENGTH)
			memcpy(g_zcl_basicAttrs.modelId, modelId, sizeof(modelId));
		else {
			if(NV_SUCC != nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_APP_DEV_NAME, ZCL_BASIC_MAX_LENGTH, buf)
				|| memcmp(g_zcl_basicAttrs.modelId, buf, g_zcl_basicAttrs.modelId[0] + 1)) {
				nv_flashWriteNew(1, NV_MODULE_APP, NV_ITEM_APP_DEV_NAME, ZCL_BASIC_MAX_LENGTH, g_zcl_basicAttrs.modelId);
			}
		}
	} else if (attrID == ZCL_ATTRID_BASIC_MFR_NAME) {
		if(g_zcl_basicAttrs.manuName[0] == 0 || g_zcl_basicAttrs.manuName[0] >= ZCL_BASIC_MAX_LENGTH)
			memcpy(g_zcl_basicAttrs.manuName, manuName, sizeof(manuName));
		else {
			if(NV_SUCC != nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_APP_MAN_NAME, ZCL_BASIC_MAX_LENGTH, buf)
				|| memcmp(g_zcl_basicAttrs.manuName, buf, g_zcl_basicAttrs.manuName[0] + 1)) {
				nv_flashWriteNew(1, NV_MODULE_APP, NV_ITEM_APP_DEV_NAME, ZCL_BASIC_MAX_LENGTH, g_zcl_basicAttrs.manuName);
			}
		}
	}
}

#endif // USE_CHG_NAME

#ifdef ZCL_THERMOSTAT_UI_CFG

void test_set_measure_longpoll_interval(void) {
	if(g_zcl_thermostatUICfgAttrs.measure_interval < READ_SENSOR_TIMER_MIN_SEC)
		g_zcl_thermostatUICfgAttrs.measure_interval = READ_SENSOR_TIMER_MIN_SEC;
	else if(g_zcl_thermostatUICfgAttrs.measure_interval > READ_SENSOR_TIMER_MAX_SEC)
		g_zcl_thermostatUICfgAttrs.measure_interval = READ_SENSOR_TIMER_MAX_SEC;
	g_sensorAppCtx.measure_interval = (g_zcl_thermostatUICfgAttrs.measure_interval * CLOCK_16M_SYS_TIMER_CLK_1S) - 25*CLOCK_16M_SYS_TIMER_CLK_1MS;
	g_zcl_pollCtrlAttrs.longPollIntervalMin = g_zcl_thermostatUICfgAttrs.measure_interval * 4;
}

// cmp buf
zcl_thermostatUICfgAttr_t zcl_nv_thermostatUiCfg;
/*********************************************************************
 * @fn      zcl_thermostatConfig_save
 *
 * @brief
 *
 * @param   None
 *
 * @return
 */
nv_sts_t zcl_thermostatConfig_save(void)
{
	nv_sts_t st = NV_SUCC;

	if(memcmp(&zcl_nv_thermostatUiCfg, &g_zcl_thermostatUICfgAttrs, sizeof(g_zcl_thermostatUICfgAttrs))) {
#if USE_DISPLAY
		if(zcl_nv_thermostatUiCfg.display_off ^ g_zcl_thermostatUICfgAttrs.display_off) {
			init_lcd();
		}
#endif
		memcpy(&zcl_nv_thermostatUiCfg, &g_zcl_thermostatUICfgAttrs, sizeof(g_zcl_thermostatUICfgAttrs));
		test_set_measure_longpoll_interval();
		if(zb_getPollRate() > g_zcl_pollCtrlAttrs.shortPollInterval * POLL_RATE_QUARTERSECONDS)
			zb_setPollRate(DEFAULT_POLL_RATE);
		st = nv_flashWriteNew(1, NV_MODULE_APP,  NV_ITEM_APP_THERMOSTAT_UI_CFG, sizeof(zcl_thermostatUICfgAttr_t), (u8*)&zcl_nv_thermostatUiCfg);
	}
#if USE_TRIGGER
	else
		st = trigger_save();
#endif
	return st;
}

#endif // ZCL_THERMOSTAT_UI_CFG

/*********************************************************************
 * @fn      init_nv_app
 *
 * @brief
 *
 * @param   None
 *
 * @return
 */
void init_nv_app(void) {
	u32 ver = 0;
	if(nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_APP_DEV_VER, sizeof(ver), (u8 *)&ver) == NV_SUCC
		&& ver == USE_NV_APP) {
#ifdef ZCL_THERMOSTAT_UI_CFG
		if(nv_flashReadNew(1, NV_MODULE_APP,  NV_ITEM_APP_THERMOSTAT_UI_CFG, sizeof(zcl_nv_thermostatUiCfg), (u8*)&zcl_nv_thermostatUiCfg) == NV_SUCC){
			memcpy(&g_zcl_thermostatUICfgAttrs, &zcl_nv_thermostatUiCfg, sizeof(g_zcl_thermostatUICfgAttrs));
		} else {
			memcpy(&g_zcl_thermostatUICfgAttrs, &g_zcl_thermostatUICfgDefault, sizeof(g_zcl_thermostatUICfgAttrs));
		}
#endif // ZCL_THERMOSTAT_UI_CFG
	} else {
#ifdef ZCL_THERMOSTAT_UI_CFG
		memcpy(&g_zcl_thermostatUICfgAttrs, &g_zcl_thermostatUICfgDefault, sizeof(g_zcl_thermostatUICfgAttrs));
		memcpy(&zcl_nv_thermostatUiCfg, &g_zcl_thermostatUICfgDefault, sizeof(zcl_nv_thermostatUiCfg));
#endif
		nv_resetAll();
		nv_resetModule(NV_MODULE_APP);
#ifdef ZCL_THERMOSTAT_UI_CFG
		nv_flashWriteNew(1, NV_MODULE_APP,  NV_ITEM_APP_THERMOSTAT_UI_CFG, sizeof(zcl_nv_thermostatUiCfg), (u8*)&zcl_nv_thermostatUiCfg);
#endif
		ver = USE_NV_APP;
		nv_flashWriteNew(1, NV_MODULE_APP, NV_ITEM_APP_DEV_VER, sizeof(ver), (u8 *)&ver);
#if	USE_DISPLAY
		if(!g_zcl_thermostatUICfgAttrs.display_off) {
			show_reset_screen();
		}
#endif // USE_DISPLAY
		drv_pm_sleep(PM_SLEEP_MODE_DEEPSLEEP, PM_WAKEUP_SRC_TIMER, 1000);
	}
#if USE_TRIGGER
	trigger_init();
#endif
#ifdef ZCL_THERMOSTAT_UI_CFG
	test_set_measure_longpoll_interval();
#endif
#if USE_CHG_NAME
	read_dev_name();
#endif
}

#endif // #if NV_ENABLE


