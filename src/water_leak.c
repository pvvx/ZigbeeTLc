#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "app_main.h"
#include "app_ui.h"
#include "battery.h"
#include "reporting.h"
#include "water_leak.h"

#if WATER_LEAK_SENSOR && defined(ZCL_IAS_ZONE)

#define WATER_JOIN_BLINK_ON_MS      150
#define WATER_JOIN_BLINK_OFF_MS     850
#define WATER_LEAK_BLINK_ON_MS      120
#define WATER_LEAK_BLINK_OFF_MS     380
#define WATER_ENROLL_RETRY_MS       5000
#define WATER_BOOT_AWAKE_SEC        90
#define WATER_JOIN_AWAKE_SEC        120
#define WATER_EVENT_AWAKE_SEC       10

static u8 water_state = 0xff;
static u32 water_last_enroll_tick;
static u32 water_awake_tick;
static u32 water_awake_us;
static bool water_fast_poll;
static bool water_join_blink;
static bool water_ota_active;

bool water_leak_keep_awake(void);
static void water_restore_poll_if_idle(void);

static u8 water_read_pin(void)
{
	u8 raw = gpio_read(GPIO_WATER) ? 1 : 0;
	return raw ? WATER_ON : !WATER_ON;
}

static void water_set_wakeup(u8 leaking)
{
#if PM_ENABLE
#if WATER_ON
	cpu_set_gpio_wakeup(GPIO_WATER, leaking ? 0 : 1, 1);
#else
	cpu_set_gpio_wakeup(GPIO_WATER, leaking ? 1 : 0, 1);
#endif
#endif
}

void water_leak_hold_awake(u16 seconds)
{
	water_awake_tick = clock_time();
	water_awake_us = seconds * 1000 * 1000;
	zb_setPollRate(QUEUE_POLL_RATE);
	water_fast_poll = true;
}

void water_leak_joining(void)
{
	water_join_blink = true;
	water_leak_hold_awake(WATER_JOIN_AWAKE_SEC);
	light_blink_stop();
	light_blink_start(255, WATER_JOIN_BLINK_ON_MS, WATER_JOIN_BLINK_OFF_MS);
}

void water_leak_joined(void)
{
	water_join_blink = false;
	water_leak_hold_awake(WATER_JOIN_AWAKE_SEC);
	if(!water_state) {
		light_blink_stop();
		light_off();
	}
}

void water_leak_ota_start(void)
{
	water_ota_active = true;
	zb_setPollRate(QUEUE_POLL_RATE);
	water_fast_poll = true;
}

void water_leak_ota_stop(void)
{
	water_ota_active = false;
	water_restore_poll_if_idle();
}

static void water_restore_poll_if_idle(void)
{
	if(water_fast_poll && !water_leak_keep_awake()) {
		set_PollRate();
		water_fast_poll = false;
	}
}

static void water_update_attr(u8 leaking)
{
	if(leaking) {
		g_zcl_iasZoneAttrs.zoneStatus |= ZONE_STATUS_BIT_ALARM1;
	} else {
		g_zcl_iasZoneAttrs.zoneStatus &= ~ZONE_STATUS_BIT_ALARM1;
	}
}

static void water_update_battery_attrs(void)
{
	battery_detect(0);
#ifdef ZCL_POWER_CFG
	g_zcl_powerAttrs.batteryVoltage = (u8)((measured_battery.average_mv + 50) / 100);
	g_zcl_powerAttrs.batteryPercentage = (u8)measured_battery.level;
#endif
}

static void water_send_battery_reports(void)
{
#ifdef ZCL_POWER_CFG
	app_forcedReport(SENSOR_DEVICE_ENDPOINT, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_VOLTAGE);
	app_forcedReport(SENSOR_DEVICE_ENDPOINT, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING);

	epInfo_t dstEpInfo;
	TL_SETSTRUCTCONTENT(dstEpInfo, 0);
	dstEpInfo.profileId = HA_PROFILE_ID;
	dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
	dstEpInfo.dstAddr.shortAddr = 0x0000;
	dstEpInfo.dstEp = SENSOR_DEVICE_ENDPOINT;
	dstEpInfo.txOptions = APS_TX_OPT_ACK_TX;

	zcl_sendReportCmd(SENSOR_DEVICE_ENDPOINT, &dstEpInfo, TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
			ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_VOLTAGE,
			ZCL_DATA_TYPE_UINT8, &g_zcl_powerAttrs.batteryVoltage);
	zcl_sendReportCmd(SENSOR_DEVICE_ENDPOINT, &dstEpInfo, TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
			ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING,
			ZCL_DATA_TYPE_UINT8, &g_zcl_powerAttrs.batteryPercentage);
#endif
}

static bool water_has_cie(void)
{
	for(u8 i = 0; i < sizeof(g_zcl_iasZoneAttrs.iasCieAddr); i++) {
		if(g_zcl_iasZoneAttrs.iasCieAddr[i]) {
			return true;
		}
	}
	return false;
}

static void water_send_enroll_req(void)
{
	if(!zb_isDeviceJoinedNwk()
			|| !water_has_cie()
			|| g_zcl_iasZoneAttrs.zoneState == ZONE_STATE_ENROLLED) {
		return;
	}

	if(water_last_enroll_tick && !clock_time_exceed(water_last_enroll_tick, WATER_ENROLL_RETRY_MS * 1000)) {
		return;
	}
	water_last_enroll_tick = clock_time();

	epInfo_t dstEpInfo;
	TL_SETSTRUCTCONTENT(dstEpInfo, 0);
	dstEpInfo.profileId = HA_PROFILE_ID;
	dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
	dstEpInfo.dstAddr.shortAddr = 0x0000;
	dstEpInfo.dstEp = SENSOR_DEVICE_ENDPOINT;
	dstEpInfo.txOptions = APS_TX_OPT_ACK_TX;

	zoneEnrollReq_t req;
	req.zoneType = g_zcl_iasZoneAttrs.zoneType;
	req.manufacturerCode = MANUFACTURER_CODE_TELINK;
	zcl_iasZone_enrollReqCmd(SENSOR_DEVICE_ENDPOINT, &dstEpInfo, TRUE, &req);
}

static void water_send_status(u8 leaking)
{
	if(!zb_isDeviceJoinedNwk()) {
		return;
	}

	epInfo_t dstEpInfo;
	TL_SETSTRUCTCONTENT(dstEpInfo, 0);
	dstEpInfo.profileId = HA_PROFILE_ID;
	dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
	dstEpInfo.dstAddr.shortAddr = 0x0000;
	dstEpInfo.dstEp = SENSOR_DEVICE_ENDPOINT;
	dstEpInfo.txOptions = APS_TX_OPT_ACK_TX;

	zoneStatusChangeNoti_t noti;
	noti.zoneStatus = g_zcl_iasZoneAttrs.zoneStatus;
	noti.extStatus = 0;
	noti.zoneId = g_zcl_iasZoneAttrs.zoneId;
	noti.delay = 0;

	water_update_battery_attrs();
	zcl_iasZone_statusChangeNotificationCmd(SENSOR_DEVICE_ENDPOINT, &dstEpInfo, TRUE, &noti);
	water_send_battery_reports();
	water_leak_hold_awake(WATER_EVENT_AWAKE_SEC);
}

void water_leak_init(void)
{
	water_state = water_read_pin();
	water_update_attr(water_state);
	water_set_wakeup(water_state);
	water_update_battery_attrs();
	water_leak_hold_awake(WATER_BOOT_AWAKE_SEC);
	if(water_state) {
		light_blink_start(255, WATER_LEAK_BLINK_ON_MS, WATER_LEAK_BLINK_OFF_MS);
	}
}

void water_leak_task(void)
{
	u8 leaking = water_read_pin();
	if(leaking != water_state) {
		water_state = leaking;
		water_update_attr(leaking);
		water_set_wakeup(leaking);

		if(leaking) {
			water_join_blink = false;
			light_blink_stop();
			light_blink_start(255, WATER_LEAK_BLINK_ON_MS, WATER_LEAK_BLINK_OFF_MS);
		} else {
			light_blink_stop();
			light_off();
		}

		water_send_status(leaking);
	}

	if(zb_isDeviceJoinedNwk()) {
		if(water_join_blink) {
			water_leak_joined();
		}
		water_send_enroll_req();
		if(water_state && !g_sensorAppCtx.timerLedEvt) {
			light_blink_start(255, WATER_LEAK_BLINK_ON_MS, WATER_LEAK_BLINK_OFF_MS);
		}
		water_restore_poll_if_idle();
	} else {
		water_fast_poll = false;
		if(!water_join_blink || !g_sensorAppCtx.timerLedEvt) {
			water_leak_joining();
		}
	}
}

void water_leak_force_report(void)
{
	water_state = water_read_pin();
	water_update_attr(water_state);
	water_set_wakeup(water_state);
	water_send_status(water_state);
}

void water_leak_prepare_sleep(void)
{
	water_set_wakeup(water_read_pin());
}

bool water_leak_keep_awake(void)
{
	if(water_ota_active) {
		return true;
	}
	if(water_state == 1) {
		return true;
	}
	return water_awake_us && !clock_time_exceed(water_awake_tick, water_awake_us);
}

#else

void water_leak_init(void)
{
}

void water_leak_task(void)
{
}

void water_leak_force_report(void)
{
}

void water_leak_hold_awake(u16 seconds)
{
	(void)seconds;
}

void water_leak_joining(void)
{
}

void water_leak_joined(void)
{
}

void water_leak_ota_start(void)
{
}

void water_leak_ota_stop(void)
{
}

void water_leak_prepare_sleep(void)
{
}

bool water_leak_keep_awake(void)
{
	return false;
}

#endif
