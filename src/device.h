#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "zcl_include.h"

/**********************************************************************
 * CONSTANT
 */
#define SENSOR_DEVICE_ENDPOINT  0x01

/**********************************************************************
 * TYPEDEFS
 */
typedef struct{
	u8 keyType; /* ERTIFICATION_KEY or MASTER_KEY key for touch-link or distribute network
	 	 	 	 SS_UNIQUE_LINK_KEY or SS_GLOBAL_LINK_KEY for distribute network */
	u8 key[16];	/* the key used */
}app_linkKey_info_t;

typedef struct{
#if	USE_DISPLAY
	ev_timer_event_t *timerTaskEvt;
#endif
	ev_timer_event_t *timerLedEvt;
	ev_timer_event_t *timerKeyEvt;

	u32 secTimeTik;
	u32 keyPressedTime;
	u32 readSensorTime; // read sensor timer (tik)
	u16 reportupsec; // report add (sec)

	u16 ledOnTime;
	u16 ledOffTime;
	u8 	oriSta;		// original state before blink
	u8 	sta;		// current state in blink
	u8 	times;		// blink times
	u8  state;

	u8  keyPressed;
	u8  key1flag;

	app_linkKey_info_t tcLinkKey;
}app_ctx_t;

/**
 *  @brief Defined for basic cluster attributes
 */
typedef struct{
	u8 	zclVersion;
	u8	appVersion;
	u8	stackVersion;
	u8	hwVersion;
	u8	manuName[ZCL_BASIC_MAX_LENGTH];
	u8	modelId[ZCL_BASIC_MAX_LENGTH];
	u8	swBuildId[ZCL_BASIC_MAX_LENGTH];
 	u8  dateCode[ZCL_BASIC_MAX_LENGTH];
	u8	powerSource;
	u8	deviceEnable;
}zcl_basicAttr_t;

/**
 *  @brief Defined for identify cluster attributes
 */
typedef struct{
	u16	identifyTime;
}zcl_identifyAttr_t;

/**
 *  @brief Defined for power configuration cluster attributes
 */
typedef struct{
#ifdef POWER_MAINS
	u16 mainsVoltage;
	u8  mainsFrequency;
#endif
	u8  batteryVoltage;      //0x20
	u8  batteryPercentage;   //0x21
}zcl_powerAttr_t;

/**
 *  @brief Defined for temperature cluster attributes
 */
typedef struct {
	s16 measuredValue;
	s16 minValue;
	s16 maxValue;
	u16 tolerance;
}zcl_temperatureAttr_t;

/**
 *  @brief Defined for relative humidity cluster attributes
 */
typedef struct {
	u16 measuredValue;
	u16 minValue;
	u16 maxValue;
	u16 tolerance;
}zcl_relHumidityAttr_t;

/**
 *  @brief Defined for thermostat UI config cluster attributes
 */
typedef struct {
	s8 temp_offset;
	s8 humi_offset;
#if	USE_DISPLAY
	u8 TemperatureDisplayMode;
	u8 showSmiley;
	s8 temp_comfort_min;
	s8 temp_comfort_max;
	u8 humi_comfort_min;
	u8 humi_comfort_max;
#endif
}zcl_thermostatUICfgAttr_t;


/**
 *  @brief  Defined for poll control cluster attributes
 */
typedef struct{
	u32	chkInInterval;
	u32	longPollInterval;
	u32	chkInIntervalMin;
	u32	longPollIntervalMin;
	u16	shortPollInterval;
	u16	fastPollTimeout;
	u16	fastPollTimeoutMax;
}zcl_pollCtrlAttr_t;

typedef struct _comfort_t {
	s16 t[2];
	u16 h[2];
} scomfort_t;


/**********************************************************************
 * GLOBAL VARIABLES
 */
extern app_ctx_t g_sensorAppCtx;

extern bdb_appCb_t g_zbDemoBdbCb;

extern bdb_commissionSetting_t g_bdbCommissionSetting;

extern u8 SENSOR_DEVICE_CB_CLUSTER_NUM;
extern const zcl_specClusterInfo_t g_sensorDeviceClusterList[];
extern const af_simple_descriptor_t sensorDevice_simpleDesc;

/* Attributes */
extern zcl_basicAttr_t g_zcl_basicAttrs;
extern zcl_identifyAttr_t g_zcl_identifyAttrs;
extern zcl_powerAttr_t g_zcl_powerAttrs;
#ifdef ZCL_THERMOSTAT_UI_CFG
extern zcl_thermostatUICfgAttr_t g_zcl_thermostatUICfgAttrs;
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT
extern zcl_temperatureAttr_t g_zcl_temperatureAttrs;
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_MEASUREMENT
extern zcl_relHumidityAttr_t g_zcl_relHumidityAttrs;
#endif
// extern zcl_iasZoneAttr_t g_zcl_iasZoneAttrs;
extern zcl_pollCtrlAttr_t g_zcl_pollCtrlAttrs;

#define zcl_iasZoneAttrGet()	&g_zcl_iasZoneAttrs
#define zcl_pollCtrlAttrGet()	&g_zcl_pollCtrlAttrs

extern scomfort_t cmf;

/**********************************************************************
 * FUNCTIONS
 */
#if 1
#define pm_wait_ms(t) cpu_stall_wakeup_by_timer0(t*CLOCK_16M_SYS_TIMER_CLK_1MS);
#define pm_wait_us(t) cpu_stall_wakeup_by_timer0(t*CLOCK_16M_SYS_TIMER_CLK_1US);
#else
#define pm_wait_ms(t) sleep_us((t)*1000);
#define pm_wait_us(t) sleep_us(t);
#endif

void sensorDevice_zclProcessIncomingMsg(zclIncoming_t *pInHdlrMsg);

status_t sensorDevice_basicCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sensorDevice_identifyCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sensorDevice_iasZoneCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sensorDevice_powerCfgCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sensorDevice_pollCtrlCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sensorDevice_groupCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
void sensorDevice_zclCheckInStart(void);

void sensorDevice_leaveCnfHandler(nlme_leave_cnf_t *pLeaveCnf);
void sensorDevice_leaveIndHandler(nlme_leave_ind_t *pLeaveInd);
void sensorDevice_otaProcessMsgHandler(u8 evt, u8 status);

#define ZCL_THERMOSTAT_UI_CFG_ATTRID_OFFSET_TEMP	0x0100
#define ZCL_THERMOSTAT_UI_CFG_ATTRID_OFFSET_HUMI	0x0101
#define ZCL_THERMOSTAT_UI_CFG_ATTRID_COMFORT_MIN_T	0x0102
#define ZCL_THERMOSTAT_UI_CFG_ATTRID_COMFORT_MAX_T	0x0103
#define ZCL_THERMOSTAT_UI_CFG_ATTRID_COMFORT_MIN_H	0x0104
#define ZCL_THERMOSTAT_UI_CFG_ATTRID_COMFORT_MAX_H	0x0105

nv_sts_t zcl_thermostatConfig_save(void);
nv_sts_t zcl_thermostatConfig_restore(void);

void scan_task(void);
s32 sensors_task(void *arg);
void read_sensor_and_save(void);
void set_comfort(void);

#endif /* _DEVICE_H_ */
