/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "device.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "ota.h"
#include "device.h"
#if ZBHCI_EN
#include "zbhci.h"
#endif

#include "app_ui.h"
#include "zcl_relative_humidity.h"
#include "chip_8258/timer.h"
#include "sensors.h"
#include "lcd.h"
#include "reporting.h"
#include "ext_ota.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * GLOBAL VARIABLES
 */
app_ctx_t g_sensorAppCtx;

#if ZCL_OTA_SUPPORT
extern ota_callBack_t sensorDevice_otaCb;

//running code firmware information
ota_preamble_t sensorDevice_otaInfo = {
	.fileVer 			= FILE_VERSION,
	.imageType 			= IMAGE_TYPE,
	.manufacturerCode 	= MANUFACTURER_CODE_TELINK,
};
#endif


//Must declare the application call back function which used by ZDO layer
const zdo_appIndCb_t appCbLst = {
	bdb_zdoStartDevCnf,//start device cnf cb
	NULL,//reset cnf cb
	NULL,//device announce indication cb
	sensorDevice_leaveIndHandler,//leave ind cb
	sensorDevice_leaveCnfHandler,//leave cnf cb
	NULL,//nwk update ind cb
	NULL,//permit join ind cb
	NULL,//nlme sync cnf cb
	NULL,//tc join ind cb
	NULL,//tc detects that the frame counter is near limit
};


/**
 *  @brief Definition for bdb commissioning setting
 */
bdb_commissionSetting_t g_bdbCommissionSetting = {
	.linkKey.tcLinkKey.keyType = SS_GLOBAL_LINK_KEY,
	.linkKey.tcLinkKey.key = (u8 *)tcLinkKeyCentralDefault,       		//can use unique link key stored in NV

	.linkKey.distributeLinkKey.keyType = MASTER_KEY,
	.linkKey.distributeLinkKey.key = (u8 *)linkKeyDistributedMaster,  	//use linkKeyDistributedCertification before testing

	.linkKey.touchLinkKey.keyType = MASTER_KEY,
	.linkKey.touchLinkKey.key = (u8 *)touchLinkKeyMaster,   			//use touchLinkKeyCertification before testing

#if TOUCHLINK_SUPPORT
	.touchlinkEnable = 1,												/* enable touch-link */
#else
	.touchlinkEnable = 0,												/* disable touch-link */
#endif
	.touchlinkChannel = DEFAULT_CHANNEL, 								/* touch-link default operation channel for target */
	.touchlinkLqiThreshold = 0xA0,			   							/* threshold for touch-link scan req/resp command */
};

/**********************************************************************
 * LOCAL VARIABLES
 */

/**********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      stack_init
 *
 * @brief   This function initialize the ZigBee stack and related profile. If HA/ZLL profile is
 *          enabled in this application, related cluster should be registered here.
 *
 * @param   None
 *
 * @return  None
 */
void stack_init(void)
{
	zb_init();
	zb_zdoCbRegister((zdo_appIndCb_t *)&appCbLst);
}

#if SHOW_SMILEY

scomfort_t cmf;

void set_comfort(void) {
	cmf.t[0] = g_zcl_thermostatUICfgAttrs.temp_comfort_min *100;
	cmf.t[1] = g_zcl_thermostatUICfgAttrs.temp_comfort_max *100;
	cmf.h[0] = g_zcl_thermostatUICfgAttrs.humi_comfort_min *100;
	cmf.h[1] = g_zcl_thermostatUICfgAttrs.humi_comfort_max *100;
}


u8 is_comfort(s16 t, u16 h) {
	u8 ret = 0;
	if (t >= cmf.t[0] && t <= cmf.t[1] && h >= cmf.h[0] && h <= cmf.h[1])
		ret = 1;
	return ret;
}
#endif

void read_sensor_and_save(void) {
#if	USE_DISPLAY
	read_sensor();
#ifdef ZCL_THERMOSTAT_UI_CFG
	if (g_zcl_thermostatUICfgAttrs.TemperatureDisplayMode) {
		// (°F) = (Temperature in degrees Celsius (°C) * 9/5) + 32.
		show_big_number_x10(((measured_data.temp * 9 + 25) / 50) + 320, 2); // convert C to F
	} else {
		show_big_number_x10((measured_data.temp + 5) / 10, 1);
	}
#else // !ZCL_THERMOSTAT_UI_CFG
	g_zcl_temperatureAttrs.measuredValue = measured_data.temp;

	show_big_number_x10((measured_data.temp  + 5) / 10, 1);
#endif // ZCL_THERMOSTAT_UI_CFG
	g_zcl_temperatureAttrs.measuredValue = measured_data.temp;
    g_zcl_relHumidityAttrs.measuredValue = measured_data.humi;

	g_zcl_powerAttrs.batteryVoltage = (u8)((measured_data.average_battery_mv + 50) / 100);
    g_zcl_powerAttrs.batteryPercentage = (u8)measured_data.battery_level;

#if BOARD == BOARD_CGDK2
    show_small_number_x10((measured_data.humi + 5)/ 10, 1);
    show_battery_symbol(true);
#else // BOARD != BOARD_CGDK2
    show_small_number((measured_data.humi + 50) / 100, 1);
    show_battery_symbol(g_zcl_powerAttrs.batteryPercentage < 10);
#endif // BOARD == BOARD_CGDK2
#if SHOW_SMILEY
#ifdef ZCL_THERMOSTAT_UI_CFG
    if(g_zcl_thermostatUICfgAttrs.showSmiley == 0)
    	show_smiley(is_comfort(measured_data.temp, measured_data.humi) ? 1 : 2);
    else
      	show_smiley(0);
#else
	show_smiley(is_comfort(measured_data.temp, measured_data.humi) ? 1 : 2);
#endif // ZCL_THERMOSTAT_UI_CFG
#endif // SHOW_SMILEY
    update_lcd();
#else // !USE_DISPLAY
	if (read_sensor()) {
		g_zcl_temperatureAttrs.measuredValue = measured_data.temp;
		g_zcl_relHumidityAttrs.measuredValue = measured_data.humi;
	}

	g_zcl_powerAttrs.batteryVoltage = (u8)((measured_data.average_battery_mv + 50) / 100);
    g_zcl_powerAttrs.batteryPercentage = (u8)measured_data.battery_level;

#endif // USE_DISPLAY
    g_sensorAppCtx.readSensorTime = clock_time();
	while(clock_time() - g_sensorAppCtx.secTimeTik >= CLOCK_16M_SYS_TIMER_CLK_1S) {
		g_sensorAppCtx.secTimeTik += CLOCK_16M_SYS_TIMER_CLK_1S;
		g_sensorAppCtx.reportupsec++; // + 1 sec
	}
}


/*********************************************************************
 * @fn      user_app_init
 *
 * @brief   This function initialize the application(Endpoint) information for this node.
 *
 * @param   None
 *
 * @return  None
 */
void user_app_init(void)
{
#if ZCL_POLL_CTRL_SUPPORT
	af_powerDescPowerModeUpdate(POWER_MODE_RECEIVER_COMES_PERIODICALLY);
#else
	af_powerDescPowerModeUpdate(POWER_MODE_RECEIVER_COMES_WHEN_STIMULATED);
#endif
    /* Initialize ZCL layer */
	/* Register Incoming ZCL Foundation command/response messages */
	zcl_init(sensorDevice_zclProcessIncomingMsg);

	/* Register endPoint */
	af_endpointRegister(SENSOR_DEVICE_ENDPOINT, (af_simple_descriptor_t *)&sensorDevice_simpleDesc, zcl_rx_handler, NULL);
#if ZCL_THERMOSTAT_UI_CFG_SUPPORT
	zcl_thermostatConfig_restore();
#endif
	zcl_reportingTabInit();

	/* Register ZCL specific cluster information */
	zcl_register(SENSOR_DEVICE_ENDPOINT, SENSOR_DEVICE_CB_CLUSTER_NUM, (zcl_specClusterInfo_t *)g_sensorDeviceClusterList);

#if ZCL_OTA_SUPPORT
    ota_init(OTA_TYPE_CLIENT, (af_simple_descriptor_t *)&sensorDevice_simpleDesc, &sensorDevice_otaInfo, &sensorDevice_otaCb);
#endif
    // read sensors
	read_sensor_and_save();
}

/**********************************************************************
 * FUNCTIONS
 */
s32 sensors_task(void *arg) {
	if(clock_time() - g_sensorAppCtx.readSensorTime > READ_SENSOR_TIMER_MS*CLOCK_16M_SYS_TIMER_CLK_1MS) {
		read_sensor_and_save();
	}
#ifdef USE_EPD
	task_lcd();
#endif
    return 0;
}

/**********************************************************************
 * FUNCTIONS
 */

void app_task(void)
{
	u16 rep_uptime_sec;
	task_keys();
	sensors_task(NULL);
	if(bdb_isIdle()){
		// report handler
		if(zb_isDeviceJoinedNwk()){
#if	USE_DISPLAY
#if BOARD == BOARD_MHO_C401N
			show_connected_symbol(true);
#else
			if(!g_sensorAppCtx.timerLedEvt)
				show_ble_symbol(false);
#endif
			if(g_sensorAppCtx.timerTaskEvt) {
				TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerTaskEvt);
				g_sensorAppCtx.timerTaskEvt = NULL;
			}
#else // !USE_DISPLAY
			if(!g_sensorAppCtx.timerLedEvt)
				light_off();
#endif // USE_DISPLAY
			rep_uptime_sec = g_sensorAppCtx.reportupsec;
			if(rep_uptime_sec > READ_SENSOR_TIMER_SEC - 1) {
				g_sensorAppCtx.reportupsec = 0;
				app_chk_report(rep_uptime_sec);
			}
		} else {
#if	USE_DISPLAY
#if BOARD == BOARD_MHO_C401N
			show_connected_symbol(false);
#else
			if(!g_sensorAppCtx.timerLedEvt)
				show_ble_symbol(true);
#endif
#else
			if(!g_sensorAppCtx.timerLedEvt)
				gpio_write(GPIO_LED, LED_ON);
#endif
		}
#if PM_ENABLE
#ifdef USE_EPD
		task_lcd();
#endif
		drv_pm_lowPowerEnter();
#endif
	}
}


void sensorDeviceSysException(void)
{
#if DEBUG_ENABLE
	extern volatile u16 T_evtExcept[4];
	show_big_number_x10(T_evtExcept[0], 0);
#if BOARD == BOARD_CGDK2
	show_small_number_x10(T_evtExcept[1], false);
#else
	show_small_number(T_evtExcept[1], false);
#endif
	update_lcd();
#ifdef USE_EPD
	while(task_lcd())
		sleep_us(USE_EPD*1000);
#endif
	drv_pm_sleep(PM_SLEEP_MODE_DEEPSLEEP, 0, 20*1000);
#else
	zb_resetDevice();
#endif
}

char int_to_hex(u8 num) {
	const char * hex_ascii = {"0123456789ABCDEF"};
	if (num > 15) return hex_ascii[0];
	return hex_ascii[num];
}

void populate_sw_build() {
	g_zcl_basicAttrs.swBuildId[1] = int_to_hex(STACK_RELEASE>>4);
	g_zcl_basicAttrs.swBuildId[2] = int_to_hex(STACK_RELEASE & 0xf);
	g_zcl_basicAttrs.swBuildId[3] = int_to_hex(STACK_BUILD>>4);
	g_zcl_basicAttrs.swBuildId[4] = int_to_hex(STACK_BUILD & 0xf);
	g_zcl_basicAttrs.swBuildId[6] = int_to_hex(APP_RELEASE>>4);
	g_zcl_basicAttrs.swBuildId[7] = int_to_hex(APP_RELEASE & 0xf);
	g_zcl_basicAttrs.swBuildId[8] = int_to_hex(APP_BUILD>>4);
	g_zcl_basicAttrs.swBuildId[9] = int_to_hex(APP_BUILD & 0xf);
}

void populate_date_code() {
	u8 month;
	if (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n') month = 1;
	else if (__DATE__[0] == 'F') month = 2;
	else if (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r') month = 3;
	else if (__DATE__[0] == 'A' && __DATE__[1] == 'p') month = 4;
	else if (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y') month = 5;
	else if (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n') month = 6;
	else if (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l') month = 7;
	else if (__DATE__[0] == 'A' && __DATE__[1] == 'u') month = 8;
	else if (__DATE__[0] == 'S') month = 9;
	else if (__DATE__[0] == 'O') month = 10;
	else if (__DATE__[0] == 'N') month = 11;
	else if (__DATE__[0] == 'D') month = 12;

	g_zcl_basicAttrs.dateCode[1] = __DATE__[7];
	g_zcl_basicAttrs.dateCode[2] = __DATE__[8];
	g_zcl_basicAttrs.dateCode[3] = __DATE__[9];
	g_zcl_basicAttrs.dateCode[4] = __DATE__[10];
	g_zcl_basicAttrs.dateCode[5] = '0' + month / 10;
	g_zcl_basicAttrs.dateCode[6] = '0' + month % 10;
	g_zcl_basicAttrs.dateCode[7] = __DATE__[4] >= '0' ? (__DATE__[4]) : '0';
	g_zcl_basicAttrs.dateCode[8] = __DATE__[5];
}


void populate_hw_version() {
#if BOARD == BOARD_LYWSD03MMC
/*
 HW  | LCD I2C addr | SHTxxx I2C addr | Note
-----|--------------|-----------------|---------
B1.4 | 0x3C         | 0x70   (SHTC3)  |
B1.5 | UART!        | 0x70   (SHTC3)  |
B1.6 | UART!        | 0x44   (SHT4x)  |
B1.7 | 0x3C         | 0x44   (SHT4x)  | Test   original string HW
B1.9 | 0x3E         | 0x44   (SHT4x)  |
B2.0 | 0x3C         | 0x44   (SHT4x)  | Test   original string HW
*/
    if (i2c_address_lcd == B14_I2C_ADDR << 1) {
        if (sensor_i2c_addr == (SHTC3_I2C_ADDR << 1)) // sensor_version == 0)
            g_zcl_basicAttrs.hwVersion = 14;
        else
            g_zcl_basicAttrs.hwVersion = 20; // or 17
    } else if (i2c_address_lcd == (B16_I2C_ADDR << 1)) {
        if (sensor_i2c_addr == (SHTC3_I2C_ADDR << 1)) // (sensor_version == 0)
            g_zcl_basicAttrs.hwVersion = 15;
        else
            g_zcl_basicAttrs.hwVersion = 16;
    } else if (i2c_address_lcd == (B19_I2C_ADDR << 1)) {
        g_zcl_basicAttrs.hwVersion = 19;
    }
#else // BOARD != BOARD_LYWSD03MMC
#if SENSOR_TYPE == SENSOR_SHTXX
    if (sensor_i2c_addr == (SHTC3_I2C_ADDR << 1)) {
#if USE_SENSOR_ID
    	g_zcl_basicAttrs.hwVersion = 2 + (sensor_id == 0);
#else
    	g_zcl_basicAttrs.hwVersion = 2;
#endif
    } else
    	g_zcl_basicAttrs.hwVersion = 1;

#elif SENSOR_TYPE == SENSOR_CHT8305
    if(sensor_i2c_addr != 0)
    	g_zcl_basicAttrs.hwVersion = 1 + ((sensor_i2c_addr >> 1) & 3);

#endif // SENSOR_TYPE
#endif
}
/*********************************************************************
 * @fn      user_init
 *
 * @brief   User level initialization code.
 *
 * @param   isRetention - if it is waking up with ram retention.
 *
 * @return  None
 */
u16 reportableChange[4];

void user_init(bool isRetention)
{
#if PA_ENABLE
	rf_paInit(PA_TX, PA_RX);
#endif

#if ZBHCI_EN
	zbhciInit();
#endif

	if(!isRetention){

		/* Populate properties with compiled-in values */
		populate_sw_build();
		populate_date_code();

		/* Initialize Stack */
		stack_init();
#if	USE_DISPLAY
		init_lcd();
#endif
		init_sensor();

		populate_hw_version();

#if DEBUG_ENABLE
		/* Register except handler for test */
		sys_exceptHandlerRegister(sensorDeviceSysException);
#endif

		/* Initialize user application */
		user_app_init();

		/* User's Task */
#if ZBHCI_EN
		ev_on_poll(EV_POLL_HCI, zbhciTask);
#endif
		ev_on_poll(EV_POLL_IDLE, app_task);

		/* Load the pre-install code from flash */
		if(bdb_preInstallCodeLoad(&g_sensorAppCtx.tcLinkKey.keyType, g_sensorAppCtx.tcLinkKey.key) == RET_OK){
			g_bdbCommissionSetting.linkKey.tcLinkKey.keyType = g_sensorAppCtx.tcLinkKey.keyType;
			g_bdbCommissionSetting.linkKey.tcLinkKey.key = g_sensorAppCtx.tcLinkKey.key;
		}
		/* Set default reporting configuration */
		reportableChange[0] = 0;
        bdb_defaultReportingCfg(
			SENSOR_DEVICE_ENDPOINT,
			HA_PROFILE_ID,
			ZCL_CLUSTER_GEN_POWER_CFG,
			ZCL_ATTRID_BATTERY_VOLTAGE,
			360,
			3600,
			(u8 *)&reportableChange[0]
		);
        reportableChange[1] = 0;
        bdb_defaultReportingCfg(
			SENSOR_DEVICE_ENDPOINT,
			HA_PROFILE_ID,
			ZCL_CLUSTER_GEN_POWER_CFG,
			ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING,
			360,
			3600,
			(u8 *)&reportableChange[1]
		);
        reportableChange[2] = 10;
		bdb_defaultReportingCfg(
			SENSOR_DEVICE_ENDPOINT,
			HA_PROFILE_ID,
			ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT,
			ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MEASUREDVALUE,
			30,
			120,
			(u8 *)&reportableChange[2]
		);
        reportableChange[3] = 50;
		bdb_defaultReportingCfg(
			SENSOR_DEVICE_ENDPOINT,
			HA_PROFILE_ID,
			ZCL_CLUSTER_MS_RELATIVE_HUMIDITY,
			ZCL_RELATIVE_HUMIDITY_ATTRID_MEASUREDVALUE,
			30,
			120,
			(u8 *)&reportableChange[3]
		);
		/* Initialize BDB */
		u8 repower = drv_pm_deepSleep_flag_get() ? 0 : 1;
		bdb_init((af_simple_descriptor_t *)&sensorDevice_simpleDesc, &g_bdbCommissionSetting, &g_zbDemoBdbCb, repower);
#if	USE_DISPLAY
///		g_sensorAppCtx.timerTaskEvt = TL_ZB_TIMER_SCHEDULE(sensors_task, NULL, READ_SENSOR_TIMER_MS);
#endif
	} else {
		/* Re-config phy when system recovery from deep sleep with retention */
		mac_phyReconfig();
	}
#ifdef USE_EPD
//	task_lcd();
#endif
}
