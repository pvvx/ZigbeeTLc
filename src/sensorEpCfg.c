/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zcl_include.h"
#include "zcl_relative_humidity.h"
#include "zcl_thermostat_ui_cfg.h"
#include "device.h"
#include "lcd.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */
#if BOARD == BOARD_LYWSD03MMC

#define ZCL_BASIC_MFG_NAME     {6,'X','i','a','o','m','i'}
#define ZCL_BASIC_MODEL_ID	   {12,'L','Y','W','S','D','0','3','M','M','C','-','z'}

#elif BOARD == BOARD_CGDK2

#define ZCL_BASIC_MFG_NAME     {8,'Q','i','n','g','p','i','n','g'} // Qingping
#define ZCL_BASIC_MODEL_ID	   {7,'C','G','D','K','2','-','z'} // CGDK2

#elif BOARD == BOARD_MHO_C122

#define ZCL_BASIC_MFG_NAME     {9,'M','i','a','M','i','a','o','C','e'} // MiaoMiaoCe
#define ZCL_BASIC_MODEL_ID	   {10,'M','H','O','-','C','1','2','2','-','z'} // MHO-C122

#elif BOARD == BOARD_MHO_C401N

#define ZCL_BASIC_MFG_NAME     {9,'M','i','a','M','i','a','o','C','e'} // MiaoMiaoCe
#define ZCL_BASIC_MODEL_ID	   {11,'M','H','O','-','C','4','0','1','N','-','z'} // MHO-C401N

#elif BOARD == BOARD_TS0201_TZ3000

#define ZCL_BASIC_MFG_NAME     {4,'T','u','y','a'} // Tuya
#define ZCL_BASIC_MODEL_ID	   {10,'T','S','0','2','0','1','-','2','e','p'} // TS0201

#elif BOARD == BOARD_TH03Z

#define ZCL_BASIC_MFG_NAME     {4,'T','u','y','a'} // Tuya
#define ZCL_BASIC_MODEL_ID	   {7,'T','H','0','3','Z','-','z'} // TH03Z

#elif BOARD == BOARD_ZTH01

#define ZCL_BASIC_MFG_NAME     {4,'T','u','y','a'} // Tuya
#define ZCL_BASIC_MODEL_ID	   {7,'Z','T','H','0','1','-','z'} // ZTH01

#elif BOARD == BOARD_ZTH02

#define ZCL_BASIC_MFG_NAME     {4,'T','u','y','a'} // Tuya
#define ZCL_BASIC_MODEL_ID	   {7,'Z','T','H','0','2','-','z'} // ZTH02

#else
#error "Define BOARD!"
#endif // BOARD

#ifndef ZCL_BASIC_SW_BUILD_ID
#define ZCL_BASIC_SW_BUILD_ID     	{9,'0','0','0','0','-','0','0','0','0'}
#endif

#ifndef ZCL_BASIC_DATE_CODE
#define ZCL_BASIC_DATE_CODE     	{8,'0','0','0','0','0','0','0','0'}
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
#ifdef ZCL_IAS_ZONE
	ZCL_CLUSTER_SS_IAS_ZONE,
#endif
};

/**
 *  @brief Definition for Outgoing cluster / Client Cluster
 */
const u16 sensorDevice_outClusterList[] =
{
#ifdef ZCL_GROUP
	ZCL_CLUSTER_GEN_GROUPS,
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


/* Basic */
zcl_basicAttr_t g_zcl_basicAttrs =
{
	.zclVersion 	= 0x03,
	.appVersion 	= 0x00,
	.stackVersion 	= 0x02,
	.hwVersion		= 0x00,
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
	{ ZCL_ATTRID_BASIC_MFR_NAME,     		ZCL_DATA_TYPE_CHAR_STR, ACCESS_CONTROL_READ,  						(u8*)g_zcl_basicAttrs.manuName},
#if USE_CHG_NAME
	{ ZCL_ATTRID_BASIC_MODEL_ID,     		ZCL_DATA_TYPE_CHAR_STR, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE,	(u8*)g_zcl_basicAttrs.modelId},
#else
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
#endif


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
		.temp_comfort_min = 2000,  // +20.00 C  ?0@0<5B@K !", !8
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


	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

#define	ZCL_THERMOSTAT_UI_CFG_ATTR_NUM		 sizeof(thermostat_ui_cfg_attrTbl) / sizeof(zclAttrInfo_t)
#endif


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
#ifdef ZCL_IAS_ZONE
	{ZCL_CLUSTER_SS_IAS_ZONE,		MANUFACTURER_CODE_NONE, ZCL_IASZONE_ATTR_NUM,	iasZone_attrTbl,	zcl_iasZone_register,	sensorDevice_iasZoneCb},
#endif
};

u8 SENSOR_DEVICE_CB_CLUSTER_NUM = (sizeof(g_sensorDeviceClusterList)/sizeof(g_sensorDeviceClusterList[0]));


//#define	ZCL_TEMPERATURE_MEASUREMENT_ATTR_NUM		 sizeof(temperature_measurement_attrTbl2) / sizeof(zclAttrInfo_t)

//=========================================================================================
//#pragma message("ZCL_DUAL_TEMPERATURE_MEASUREMENT is " STRINGIFY(ZCL_DUAL_TEMPERATURE_MEASUREMENT))
#ifdef ZCL_DUAL_TEMPERATURE_MEASUREMENT
//================================================================================================

/**
 *  @brief Definition for Incoming cluster / Sever Cluster
 */
const u16 sensorDevice2_inClusterList[] =
{
#ifdef ZCL_TEMPERATURE_MEASUREMENT
	ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT,
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_MEASUREMENT
    ZCL_CLUSTER_MS_RELATIVE_HUMIDITY,
#endif
};

/**
 *  @brief Definition for Outgoing cluster / Client Cluster
 */
const u16 sensorDevice2_outClusterList[] =
{
};

/**
 *  @brief Definition for Server cluster number and Client cluster number
 */
#define sensorDevice2_IN_CLUSTER_NUM		(sizeof(sensorDevice2_inClusterList)/sizeof(sensorDevice2_inClusterList[0]))
#define sensorDevice2_OUT_CLUSTER_NUM	0


const af_simple_descriptor_t sensorDevice2_simpleDesc =
{
	HA_PROFILE_ID,                      	/* Application profile identifier */
	HA_DEV_TEMPERATURE_SENSOR,              /* Application device identifier */
	SENSOR2_DEVICE_ENDPOINT,         		/* Endpoint */
	1,										/* Application device version */
	0,										/* Reserved */
	sensorDevice2_IN_CLUSTER_NUM,           	/* Application input cluster count */
	sensorDevice2_OUT_CLUSTER_NUM,          	/* Application output cluster count */
	(u16 *)sensorDevice2_inClusterList,    	/* Application input cluster list */
	(u16 *)sensorDevice2_outClusterList,   	/* Application output cluster list */
};

zcl_temperatureAttr_t g_zcl_temperatureAttrs2 =
{
	.measuredValue	= 0x8000,
	.minValue 		= -5000,
	.maxValue		= 17500,
	.tolerance		= 0,
};
const zclAttrInfo_t temperature_measurement_attrTbl2[] =
{
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MEASUREDVALUE,       	ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_temperatureAttrs2.measuredValue },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MINMEASUREDVALUE,      ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_temperatureAttrs2.minValue },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MAXMEASUREDVALUE,      ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_temperatureAttrs2.maxValue },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_TOLERANCE,       		ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_temperatureAttrs2.tolerance },

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

/**
 *  @brief Definition for second temperature sensor temperature measurement ZCL cluster
 */
const zcl_specClusterInfo_t g_sensorDevice2ClusterList[] =
{
	#ifdef ZCL_TEMPERATURE_MEASUREMENT
		{ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT,	MANUFACTURER_CODE_NONE, ZCL_TEMPERATURE_MEASUREMENT_ATTR_NUM, temperature_measurement_attrTbl2, 	zcl_temperature_measurement_register, 	NULL},
	#endif

};
u8 SENSOR_DEVICE_CB_CLUSTER_NUM2 = (sizeof(g_sensorDevice2ClusterList)/sizeof(g_sensorDevice2ClusterList[0]));

#endif
//=====================================================================================================================
//===================================================================================================================

/**********************************************************************
 * FUNCTIONS
 */
#if NV_ENABLE
	zcl_thermostatUICfgAttr_t zcl_nv_thermostatUiCfg;
#endif



#if USE_CHG_NAME

#define FLASH_DEV_NAME_ADDR  0x075000
static const u8 modelId[] = ZCL_BASIC_MODEL_ID;

void read_dev_name(void) {
	flash_read_page(FLASH_DEV_NAME_ADDR, ZCL_BASIC_MAX_LENGTH, g_zcl_basicAttrs.modelId);
	if(g_zcl_basicAttrs.modelId[0] == 0 || g_zcl_basicAttrs.modelId[0] >= ZCL_BASIC_MAX_LENGTH)
		memcpy(g_zcl_basicAttrs.modelId, modelId, sizeof(modelId));
}

void save_dev_name(void) {
	u8 buf[ZCL_BASIC_MAX_LENGTH];
	if(g_zcl_basicAttrs.modelId[0] == 0 || g_zcl_basicAttrs.modelId[0] >= ZCL_BASIC_MAX_LENGTH)
		memcpy(g_zcl_basicAttrs.modelId, modelId, sizeof(modelId));
	flash_read_page(FLASH_DEV_NAME_ADDR, ZCL_BASIC_MAX_LENGTH, buf);
	if(memcmp(g_zcl_basicAttrs.modelId, buf, g_zcl_basicAttrs.modelId[0] + 1)) {
		flash_write_status(0, 0);
		flash_erase_sector(FLASH_DEV_NAME_ADDR);
		flash_write_page(FLASH_DEV_NAME_ADDR, g_zcl_basicAttrs.modelId[0] + 1, g_zcl_basicAttrs.modelId);
	}
}

#endif // USE_CHG_NAME


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

#ifdef ZCL_THERMOSTAT_UI_CFG
#if NV_ENABLE
	if(memcmp(&zcl_nv_thermostatUiCfg, &g_zcl_thermostatUICfgAttrs, sizeof(g_zcl_thermostatUICfgAttrs))) {
#if USE_DISPLAY
		if(zcl_nv_thermostatUiCfg.display_off != g_zcl_thermostatUICfgAttrs.display_off) {
			init_lcd(); // show_blink_screen();
		}
#endif
		memcpy(&zcl_nv_thermostatUiCfg, &g_zcl_thermostatUICfgAttrs, sizeof(g_zcl_thermostatUICfgAttrs));
		if(g_zcl_thermostatUICfgAttrs.measure_interval < READ_SENSOR_TIMER_MIN_SEC)
			g_zcl_thermostatUICfgAttrs.measure_interval = READ_SENSOR_TIMER_SEC;
		g_sensorAppCtx.measure_interval = (g_zcl_thermostatUICfgAttrs.measure_interval * CLOCK_16M_SYS_TIMER_CLK_1S) - 25*CLOCK_16M_SYS_TIMER_CLK_1MS;
		zb_setPollRate(DEFAULT_POLL_RATE);
		st = nv_flashWriteNew(1, NV_MODULE_ZCL,  NV_ITEM_ZCL_THERMOSTAT_UI_CFG, sizeof(zcl_thermostatUICfgAttr_t), (u8*)&zcl_nv_thermostatUiCfg);
	}
#else
	st = NV_ENABLE_PROTECT_ERROR;
#endif
#endif
	return st;
}

/*********************************************************************
 * @fn      zcl_thermostatConfig_restore
 *
 * @brief
 *
 * @param   None
 *
 * @return
 */
nv_sts_t zcl_thermostatConfig_restore(void)
{
	nv_sts_t st = NV_SUCC;

#ifdef ZCL_THERMOSTAT_UI_CFG
#if NV_ENABLE
	st = nv_flashReadNew(1, NV_MODULE_ZCL,  NV_ITEM_ZCL_THERMOSTAT_UI_CFG, sizeof(zcl_nv_thermostatUiCfg), (u8*)&zcl_nv_thermostatUiCfg);

	if(st == NV_SUCC){
		memcpy(&g_zcl_thermostatUICfgAttrs, &zcl_nv_thermostatUiCfg, sizeof(g_zcl_thermostatUICfgAttrs));
	} else {
		memcpy(&g_zcl_thermostatUICfgAttrs, &g_zcl_thermostatUICfgDefault, sizeof(g_zcl_thermostatUICfgAttrs));
	}
	if(g_zcl_thermostatUICfgAttrs.measure_interval < READ_SENSOR_TIMER_MIN_SEC)
		g_zcl_thermostatUICfgAttrs.measure_interval = READ_SENSOR_TIMER_SEC;
	g_sensorAppCtx.measure_interval = (g_zcl_thermostatUICfgAttrs.measure_interval * CLOCK_16M_SYS_TIMER_CLK_1S) - 25*CLOCK_16M_SYS_TIMER_CLK_1MS;
#if USE_DISPLAY
	if(g_zcl_thermostatUICfgAttrs.display_off) {
		init_lcd(); // show_blink_screen();
	}
#endif

#else
	st = NV_ENABLE_PROTECT_ERROR;
#endif
#endif
	return st;
}




