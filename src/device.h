#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "zcl_include.h"

/**********************************************************************
 * CONSTANT
 */
#define SENSOR_DEVICE_ENDPOINT  0x01
#define SENSOR_DEVICE_ENDPOINT2  0x02
#define SENSOR_DEVICE_ENDPOINT3  0x03
#define SENSOR_DEVICE_ENDPOINT4  0x04

/**********************************************************************
 * TYPEDEFS
 */
typedef struct{
	u8 keyType; /* ERTIFICATION_KEY or MASTER_KEY key for touch-link or distribute network
	 	 	 	 SS_UNIQUE_LINK_KEY or SS_GLOBAL_LINK_KEY for distribute network */
	u8 key[16];	/* the key used */
}app_linkKey_info_t;

typedef struct{
	u16 reportupsec; // report add (sec)

	u8  keyPressed;	// gpio keyPressed
	u8  key_on_flag; // new keyPressed and flags (task_keys() used)

	u32 secTimeTik;	// used for calculate measure seconds
	u32 readSensorTime; // read sensor timer (tik)
	u32 keyPressedTime; // key pressed time (tik)
	u32 measure_interval; // measure step

	u16 ledOnTime;	// used blink
	u16 ledOffTime; // used blink

	u8 	oriSta;		// original state before blink
	u8 	sta;		// current state in blink

	u8 	times;		// blink times

#if	!USE_BLE && USE_DISPLAY
	ev_timer_event_t *timerTaskEvt;
#endif
	ev_timer_event_t *timerLedEvt;
	ev_timer_event_t *timerKeyEvt;
#if REJOIN_FAILURE_TIMER
	ev_timer_event_t *timerRejoinBackoffEvt;
#endif
	ev_timer_event_t *timerSteerEvt;
#if REJOIN_FAILURE_TIMER
	ev_timer_event_t *timerRejoinBackOffEvt;
#endif
#ifdef ZCL_IDENTIFY
	ev_timer_event_t *timerIdentifyEvt;
#endif

#if REJOIN_FAILURE_TIMER
	u16	rejoin_cnt; // startup counter
#endif

	app_linkKey_info_t tcLinkKey;
} app_ctx_t;

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

typedef struct {
	u16 measuredValue;
	u16 minValue;
	u16 maxValue;
	u16 tolerance;
}zcl_MoistureAttr_t;

/**
 *  @brief Defined for thermostat UI config cluster attributes
 */
typedef struct {
	s16 temp_offset;	// in 0.01 C
	s16 humi_offset;	// in 0.01 %
#if	USE_DISPLAY
	s16 temp_comfort_min; // in 0.01 C
	s16 temp_comfort_max; // in 0.01 C
	u16 humi_comfort_min; // in 0.01 %
	u16 humi_comfort_max; // in 0.01 %
	u8 TemperatureDisplayMode;
	u8 showSmiley;
	u8 display_off;
#endif
	u8 measure_interval;
}zcl_thermostatUICfgAttr_t;

/**
 *  @brief Defined for Occupancy cluster attributes
 */
typedef struct {
	u8  occupancy;
	u8  sensor_type;
	u16 delay;
} zcl_occupancyAttr_t;

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

extern const u8 SENSOR_DEVICE_CB_CLUSTER_NUM;
extern const zcl_specClusterInfo_t g_sensorDeviceClusterList[];
extern const af_simple_descriptor_t sensorDevice_simpleDesc;

/* Attributes */
extern zcl_basicAttr_t g_zcl_basicAttrs;
extern zcl_identifyAttr_t g_zcl_identifyAttrs;
extern zcl_powerAttr_t g_zcl_powerAttrs;
#ifdef ZCL_THERMOSTAT_UI_CFG
extern zcl_thermostatUICfgAttr_t g_zcl_thermostatUICfgAttrs;
extern const zcl_thermostatUICfgAttr_t g_zcl_thermostatUICfgDefault;
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT
extern zcl_temperatureAttr_t g_zcl_temperatureAttrs;
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_MEASUREMENT
extern zcl_relHumidityAttr_t g_zcl_relHumidityAttrs;
#endif
#if (DEV_SERVICES & SERVICE_PLM)
extern const u8 SENSOR_DEVICE_CB_CLUSTER_NUM2;
extern const zcl_specClusterInfo_t g_sensorDeviceClusterList2[];
extern const af_simple_descriptor_t sensorDevice_simpleDesc2;
extern zcl_MoistureAttr_t g_zcl_MoistureAttrs;
#endif

// extern zcl_iasZoneAttr_t g_zcl_iasZoneAttrs;
extern zcl_pollCtrlAttr_t g_zcl_pollCtrlAttrs;

#define zcl_iasZoneAttrGet()	&g_zcl_iasZoneAttrs
#define zcl_pollCtrlAttrGet()	&g_zcl_pollCtrlAttrs

extern scomfort_t cmf;

/**********************************************************************
 * FUNCTIONS
 */
#if !USE_BLE
#define pm_wait_ms(t) cpu_stall_wakeup_by_timer0(t*(CLOCK_SYS_CLOCK_HZ/1000));
#define pm_wait_us(t) cpu_stall_wakeup_by_timer0(t*(CLOCK_SYS_CLOCK_HZ/1000000));
#else
#define pm_wait_ms(t) sleep_us((t)*1000);
#define pm_wait_us(t) sleep_us(t);
#endif

void init_nv_app(void);

void sensorDevice_zclProcessIncomingMsg(zclIncoming_t *pInHdlrMsg);

status_t sensorDevice_basicCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sensorDevice_identifyCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sensorDevice_iasZoneCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sensorDevice_powerCfgCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sensorDevice_pollCtrlCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sensorDevice_groupCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
void sensorDevice_zclCheckInStart(void);
void set_PollRate(void);

void sensorDevice_leaveCnfHandler(nlme_leave_cnf_t *pLeaveCnf);
void sensorDevice_leaveIndHandler(nlme_leave_ind_t *pLeaveInd);
void sensorDevice_otaProcessMsgHandler(u8 evt, u8 status);

#if USE_BLE
void user_ble_init(bool isRetention);
extern u8  mac_public[6];
extern u8  mac_random_static[6];

void blc_initMacAddress(u32 flash_addr, u8 *mac_public, u8 *mac_random_static);
void blc_newMacAddress(u32 flash_addr, u8 *mac_pub, u8 *mac_rand);

void app_pm_task(void);

#if USE_CHG_NAME
void read_dev_name(void);
void save_dev_name(u16 attrID);
#endif

#endif // USE_BLE

void user_init(bool isRetention);

char int_to_hex(u8 num);

#ifdef ZCL_POLL_CTRL
typedef struct {
	ev_timer_event_t *zclFastPollTimeoutTimerEvt;
	ev_timer_event_t *zclCheckInTimerEvt;
	bool isFastPollMode;
} poll_ctrl_wrk_t;
extern poll_ctrl_wrk_t poll_ctrl_wrk;
#endif

void scan_task(void);
s32 sensors_task(void *arg);
void read_sensor_and_save(void);

#endif /* _DEVICE_H_ */
