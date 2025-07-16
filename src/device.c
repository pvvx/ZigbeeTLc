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
#include "battery.h"
#include "lcd.h"
#include "reporting.h"
#include "ext_ota.h"
//#include "zcl_thermostat_ui_cfg.h"
#if USE_BLE
#include "ble_cfg.h"
#include "zigbee_ble_switch.h"
#endif
#if USE_TRIGGER
#include "trigger.h"
#endif
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
char int_to_hex(u8 num) {
	const char * hex_ascii = {"0123456789ABCDEF"};
	if (num > 15) return hex_ascii[0];
	return hex_ascii[num];
}

/*
void populate_sw_build(void) {
	g_zcl_basicAttrs.swBuildId[1] = int_to_hex(STACK_RELEASE>>4);
	g_zcl_basicAttrs.swBuildId[2] = int_to_hex(STACK_RELEASE & 0xf);
	g_zcl_basicAttrs.swBuildId[3] = int_to_hex(STACK_BUILD>>4);
	g_zcl_basicAttrs.swBuildId[4] = int_to_hex(STACK_BUILD & 0xf);
	g_zcl_basicAttrs.swBuildId[6] = int_to_hex(APP_RELEASE>>4);
	g_zcl_basicAttrs.swBuildId[7] = int_to_hex(APP_RELEASE & 0xf);
	g_zcl_basicAttrs.swBuildId[8] = int_to_hex(APP_BUILD>>4);
	g_zcl_basicAttrs.swBuildId[9] = int_to_hex(APP_BUILD & 0xf);
}*/

void populate_date_code(void) {
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


void populate_hw_version(void) {
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
#if USE_BLE
    ble_attr.my_HardStr[0] = 'B';
    ble_attr.my_HardStr[1] = '1';
    ble_attr.my_HardStr[2] = '.';
#endif
    if (scr.i2c_address == B14_I2C_ADDR << 1) {
        if (sensor_ht.sensor_type == TH_SENSOR_SHTC3) { // sensor_version == 0)
            g_zcl_basicAttrs.hwVersion = BOARD_LYWSD03MMC_B14; // B1.4
#if USE_BLE
            ble_attr.my_HardStr[3] = '4';
#endif
        } else {
            g_zcl_basicAttrs.hwVersion = BOARD_LYWSD03MMC_B20; // B2.0 or B1.7
#if USE_BLE
            ble_attr.my_HardStr[1] = '2';
            ble_attr.my_HardStr[3] = '0';
#endif
        }
    } else if (scr.i2c_address == (B16_I2C_ADDR << 1)) {
        if (sensor_ht.sensor_type == TH_SENSOR_SHTC3) { // (sensor_version == 0)
            g_zcl_basicAttrs.hwVersion = BOARD_LYWSD03MMC_B15; // B1.5
#if USE_BLE
            ble_attr.my_HardStr[3] = '5';
#endif
        } else if (scr.i2c_address == N16_I2C_ADDR) {
            g_zcl_basicAttrs.hwVersion = BOARD_LYWSD03MMC_N16; // N1.6
#if USE_BLE
            ble_attr.my_HardStr[3] = '1';
#endif
        } else {
            g_zcl_basicAttrs.hwVersion = BOARD_LYWSD03MMC_B16; // B1.6
#if USE_BLE
            ble_attr.my_HardStr[3] = '6';
#endif
        }
    } else if (scr.i2c_address == (B19_I2C_ADDR << 1)) {
        g_zcl_basicAttrs.hwVersion = BOARD_LYWSD03MMC_B19; // B1.9
#if USE_BLE
        ble_attr.my_HardStr[3] = '9';
#endif
    }
#else // BOARD != BOARD_LYWSD03MMC
    g_zcl_basicAttrs.hwVersion = BOARD;
#if USE_BLE
    ble_attr.my_HardStr[0] = 'V';
    ble_attr.my_HardStr[1] = '0';
    ble_attr.my_HardStr[2] = '0' + BOARD / 10;
    ble_attr.my_HardStr[3] = '0' + BOARD % 10;
#endif
#endif
}


#if SHOW_SMILEY

u8 is_comfort(s16 t, u16 h) {
	u8 ret = 0;
	if (t >= g_zcl_thermostatUICfgAttrs.temp_comfort_min
		&& t <= g_zcl_thermostatUICfgAttrs.temp_comfort_max
		&& h >= g_zcl_thermostatUICfgAttrs.humi_comfort_min
		&& h <= g_zcl_thermostatUICfgAttrs.humi_comfort_max)
		ret = 1;
	return ret;
}
#endif


void read_sensor_and_save(void) {
	if(!read_sensor()) {
#if USE_SENSOR_TH
		if(sensor_ht.flag & FLG_MEASURE_HT_RP) {
			sensor_ht.flag &= ~FLG_MEASURE_HT_RP;
			g_zcl_temperatureAttrs.measuredValue = sensor_ht.temp;
			g_zcl_relHumidityAttrs.measuredValue = sensor_ht.humi;
#if USE_TRIGGER
			set_trigger_out();
#endif
		}
#endif
	}
	g_zcl_powerAttrs.batteryVoltage = (u8)((measured_battery.average_mv + 50) / 100);
    g_zcl_powerAttrs.batteryPercentage = (u8)measured_battery.level;
#if	USE_DISPLAY
    if(!g_zcl_thermostatUICfgAttrs.display_off) {
    	if(sensor_ht.flag & FLG_MEASURE_HT_LCD) {
    		sensor_ht.flag &= ~FLG_MEASURE_HT_LCD;
#ifdef ZCL_THERMOSTAT_UI_CFG
    		if (g_zcl_thermostatUICfgAttrs.TemperatureDisplayMode) {
    			// (°F) = (Temperature in degrees Celsius (°C) * 9/5) + 32.
    			show_big_number_x10(((sensor_ht.temp * 9 + 25) / 50) + 320, TEMP_SYMBOL_F); // convert C to F
    		} else {
    			show_big_number_x10((sensor_ht.temp + 5) / 10, TEMP_SYMBOL_C);
    		}
#else // !ZCL_THERMOSTAT_UI_CFG
    		show_big_number_x10((sensor_ht.temp  + 5) / 10, TEMP_SYMBOL_C);
#endif // ZCL_THERMOSTAT_UI_CFG

#ifdef USE_DISPLAY_SMALL_NUMBER_X10
    		show_small_number_x10((sensor_ht.humi + 5)/ 10, 1);
#else
    		show_small_number((sensor_ht.humi + 50) / 100, 1);
#endif
#ifdef USE_DISPLAY_BATTERY_LEVEL
    		show_battery_symbol(true, g_zcl_powerAttrs.batteryPercentage);
#else
    		show_battery_symbol(g_zcl_powerAttrs.batteryPercentage < 25);
#endif
#if SHOW_SMILEY
#ifdef ZCL_THERMOSTAT_UI_CFG
    		if(g_zcl_thermostatUICfgAttrs.showSmiley == 0)
    			show_smiley(is_comfort(sensor_ht.temp, sensor_ht.humi) ? SMILE_HAPPY : SMILE_SAD);
    		else
    			show_smiley(SMILE_NONE);
#else
    		show_smiley(is_comfort(sensor_ht.temp, sensor_ht.humi) ? SMILE_HAPPY : SMILE_SAD);
#endif // ZCL_THERMOSTAT_UI_CFG
#endif // SHOW_SMILEY
    	} else {
    		show_err_sensors();
    	}
#if USE_BLE
    	extern u8 blt_state;
   		show_ble_symbol(blt_state);
#endif
    	update_lcd();
    }

#endif // USE_DISPLAY
    g_sensorAppCtx.readSensorTime = clock_time();
	while(clock_time() - g_sensorAppCtx.secTimeTik >= CLOCK_16M_SYS_TIMER_CLK_1S) {
		g_sensorAppCtx.secTimeTik += CLOCK_16M_SYS_TIMER_CLK_1S;
		g_sensorAppCtx.reportupsec++; // + 1 sec
	}
}


/**********************************************************************
 * FUNCTIONS
 */
s32 sensors_task(void *arg) {
	(void) arg;
	if(clock_time() - g_sensorAppCtx.readSensorTime >= g_sensorAppCtx.measure_interval) {
		read_sensor_and_save();
	}
#ifdef USE_EPD
	task_lcd();
#endif
#if !USE_BLE && USE_DISPLAY
	if(zb_isDeviceJoinedNwk())
			return -1; // delete timer
#endif
	return 0;
}

/**********************************************************************
 * FUNCTIONS
 */
void app_task(void)
{
	u16 rep_uptime_sec;
#if !USE_BLE  && USE_SENSOR_TH
	static u8 flg_cnt;
	sensors_task(NULL);
	task_keys();
#endif
	if(bdb_isIdle()) {
		// report handler
		if(zb_isDeviceJoinedNwk()) {
#if	USE_DISPLAY
			if(!g_zcl_thermostatUICfgAttrs.display_off) {
				if(!g_sensorAppCtx.timerLedEvt)
					show_connected_symbol(true);
			}
#ifdef USE_BLINK_LED
			if(!g_sensorAppCtx.timerLedEvt)
				light_off();
#endif // USE_BLINK_LED

#else // !USE_DISPLAY
			if(!g_sensorAppCtx.timerLedEvt)
				light_off();
#endif // USE_DISPLAY
			rep_uptime_sec = g_sensorAppCtx.reportupsec;
			if(rep_uptime_sec) {
				g_sensorAppCtx.reportupsec = 0;
				app_chk_report(rep_uptime_sec);
#if USE_TRIGGER
				send_onoff();
#endif
			}
		} else { // Device not Joined
#if	USE_DISPLAY
			if(!g_zcl_thermostatUICfgAttrs.display_off) {
				if(!g_sensorAppCtx.timerLedEvt)
					show_connected_symbol(false);
			}
#ifdef USE_BLINK_LED
			if(!g_sensorAppCtx.timerLedEvt)
				light_blink_start(1, 50, 10000);
#endif // USE_BLINK_LED
#else // !USE_DISPLAY
			if(!g_sensorAppCtx.timerLedEvt)
				light_blink_start(1, 50, 10000);
			//gpio_write(GPIO_LED, LED_ON); // - не включать PIN_PULLUP/PULLDOWN !
#endif // USE_DISPLAY
		}
#if PM_ENABLE
#ifdef USE_EPD
		task_lcd();
#endif
#if !USE_BLE
#if	USE_SENSOR_TH
		if(flg_cnt)
			flg_cnt--;
		else {
			if(sensor_ht.read_callback) {
				sensor_ht.read_callback();
			} else {
				drv_pm_lowPowerEnter();
			}
		}
#else
		drv_pm_lowPowerEnter();
#endif // USE_SENSOR_TH
#endif // !USE_BLE
#endif // PM_ENABLE
	}
#if !USE_BLE && USE_SENSOR_TH
	else
		flg_cnt = 1;
#endif // !USE_BLE
}

void sensorDeviceSysException(void)
{
#if DEBUG_ENABLE
	extern volatile u16 T_evtExcept[4];
	show_big_number_x10(T_evtExcept[0], 0);
#ifdef USE_DISPLAY_SMALL_NUMBER_X10
	show_small_number_x10(T_evtExcept[1], TEMP_SYMBOL_E);
#else
	show_small_number(T_evtExcept[1], TEMP_SYMBOL_E);
#endif
	update_lcd();
#ifdef USE_EPD
	while(task_lcd())
		sleep_us(USE_EPD*1000);
#endif
	drv_pm_longSleep(PM_SLEEP_MODE_DEEPSLEEP, PM_WAKEUP_SRC_TIMER, 20*1000);
#else
	zb_resetDevice();
#endif
}

/**
 *  @brief Definition for BDB finding and binding cluster
 */
u16 bdb_findBindClusterList[] =
{
	ZCL_CLUSTER_GEN_POWER_CFG,
#ifdef ZCL_TEMPERATURE_MEASUREMENT
	ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT,
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_MEASUREMENT
    ZCL_CLUSTER_MS_RELATIVE_HUMIDITY,
#endif
};

/**
 *  @brief Definition for BDB finding and binding cluster number
 */
#define FIND_AND_BIND_CLUSTER_NUM		(sizeof(bdb_findBindClusterList)/sizeof(bdb_findBindClusterList[0]))

/*********************************************************************
 * @fn      user_app_init
 *
 * @brief   This function initialize the application
 *
 * @param   None
 *
 * @return  None
 */
void user_app_init(void)
{
	u32 reportableChange;

	/* Populate properties with compiled-in values */
	// populate_sw_build();
	populate_date_code();

#if USE_BLE
	blc_initMacAddress(CFG_MAC_ADDRESS, mac_public, mac_random_static);
#endif

#if (ZCL_THERMOSTAT_UI_CFG_SUPPORT || USE_CHG_NAME)
	init_nv_app();
#endif

	/* Initialize ZB stack */
	zb_init();
	/* Register stack CB */
	zb_zdoCbRegister((zdo_appIndCb_t *)&appCbLst);

#if	USE_DISPLAY
	LCD_INIT_DELAY();
#endif

	init_sensor();

#if	USE_DISPLAY
	init_lcd();
	show_connected_symbol(false);
#endif
#if DEBUG_ENABLE
	/* Register except handler for test */
	sys_exceptHandlerRegister(sensorDeviceSysException);
#endif


	populate_hw_version();


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

	zcl_reportingTabInit();

	/* Register ZCL specific cluster information */
	zcl_register(SENSOR_DEVICE_ENDPOINT, SENSOR_DEVICE_CB_CLUSTER_NUM, (zcl_specClusterInfo_t *)g_sensorDeviceClusterList);

#if ZCL_OTA_SUPPORT
    ota_init(OTA_TYPE_CLIENT, (af_simple_descriptor_t *)&sensorDevice_simpleDesc, &sensorDevice_otaInfo, &sensorDevice_otaCb);
#endif

    // read sensors
	read_sensor_and_save();

    light_off();

	// keys init
	keys_init();

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
	reportableChange = 0;
    bdb_defaultReportingCfg(
		SENSOR_DEVICE_ENDPOINT,
		HA_PROFILE_ID,
		ZCL_CLUSTER_GEN_POWER_CFG,
		ZCL_ATTRID_BATTERY_VOLTAGE,
		360,
		3600,
		(u8 *)&reportableChange
	);
    reportableChange = 0;
    bdb_defaultReportingCfg(
		SENSOR_DEVICE_ENDPOINT,
		HA_PROFILE_ID,
		ZCL_CLUSTER_GEN_POWER_CFG,
		ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING,
		360,
		3600,
		(u8 *)&reportableChange
	);
    reportableChange = 10;
	bdb_defaultReportingCfg(
		SENSOR_DEVICE_ENDPOINT,
		HA_PROFILE_ID,
		ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT,
		ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MEASUREDVALUE,
#if READ_SENSOR_TIMER_SEC > 30
		30,
		180,
#else
		READ_SENSOR_TIMER_SEC,
		5*60,
#endif
		(u8 *)&reportableChange
	);
    reportableChange = 50;
	bdb_defaultReportingCfg(
		SENSOR_DEVICE_ENDPOINT,
		HA_PROFILE_ID,
		ZCL_CLUSTER_MS_RELATIVE_HUMIDITY,
		ZCL_RELATIVE_HUMIDITY_ATTRID_MEASUREDVALUE,
#if READ_SENSOR_TIMER_SEC > 30
		30,
		180,
#else
		READ_SENSOR_TIMER_SEC,
		5*60,
#endif
		(u8 *)&reportableChange
	);

	bdb_findBindMatchClusterSet(FIND_AND_BIND_CLUSTER_NUM, bdb_findBindClusterList);

	/* Initialize BDB */
	u8 repower = drv_pm_deepSleep_flag_get() ? 0 : 1;
	bdb_init((af_simple_descriptor_t *)&sensorDevice_simpleDesc, &g_bdbCommissionSetting, &g_zbDemoBdbCb, repower);

#if USE_BLE && defined(STARTUP_IN_BLE) && (!STARTUP_IN_BLE) // Startup in Zigbee ?
	u32 r = drv_disable_irq();
	switch_to_zb_context();
	drv_restore_irq(r);
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
void user_init(bool isRetention)
{

#if ZBHCI_EN
	zbhciInit();
#endif

	if(!isRetention){

		/* Initialize user application */
		user_app_init();

	} else {
		/* Re-config phy when system recovery from deep sleep with retention */
		mac_phyReconfig();
	}
}
