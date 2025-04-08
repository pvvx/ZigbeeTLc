/********************************************************************************************************
 * @file    app_ui.c
 *
 * @brief   This is the source file for app_ui
 *
 *******************************************************************************************************/

/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "device.h"
#include "sensors.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "lcd.h"
#include "app_ui.h"
#include "reporting.h"
#include "zcl_thermostat_ui_cfg.h"
#if USE_BLE
#include "stack/ble/ble_8258/ble.h"
#include "ble_cfg.h"
#include "bthome_beacon.h"
#endif

/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */
#ifndef BUTTON1_ON
#define BUTTON1_ON 		0
#define BUTTON1_OFF 	1
#endif

/**********************************************************************
 * LOCAL FUNCTIONS
 */

#ifdef  GPIO_LED
static inline void led_on(void) {
#if LED_ON == 1
	gpio_write(GPIO_LED, 1);
	gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLUP_10K);
#else
	gpio_write(GPIO_LED, 0);
	gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLDOWN_100K);
#endif
}

static inline void led_off(void) {
#if LED_ON == 1
	gpio_write(GPIO_LED, 0);
	gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLDOWN_100K);
#else
	gpio_write(GPIO_LED, 1);
	gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLUP_10K);
#endif
}
#endif //GPIO_LED


void light_on(void)
{
#ifdef  GPIO_LED
    led_on();
#endif
#if USE_DISPLAY
	show_light(true);
    update_lcd();
#endif
}

void light_off(void)
{
#ifdef  GPIO_LED
    led_off();
#endif
#if USE_DISPLAY
	show_light(false);
    update_lcd();
#endif
}

void light_init(void)
{
	light_off();
}

s32 zclLightTimerCb(void *arg)
{
	u32 interval = 0;

	if(g_sensorAppCtx.sta == g_sensorAppCtx.oriSta){
		g_sensorAppCtx.times--;
		if(g_sensorAppCtx.times <= 0){
			g_sensorAppCtx.timerLedEvt = NULL;
			return -1;
		}
	}

	g_sensorAppCtx.sta = !g_sensorAppCtx.sta;
	if(g_sensorAppCtx.sta){
		light_on();
		interval = g_sensorAppCtx.ledOnTime;
	}else{
		light_off();
		interval = g_sensorAppCtx.ledOffTime;
	}
#ifdef USE_EPD
	interval <<= 2;
#endif
	return interval;
}

void light_blink_start(u8 times, u16 ledOnTime, u16 ledOffTime)
{
	u32 interval = 0;
	g_sensorAppCtx.times = times;

	if(!g_sensorAppCtx.timerLedEvt){
		if(g_sensorAppCtx.oriSta){
			light_off();
			g_sensorAppCtx.sta = 0;
			interval = ledOffTime;
		}else{
			light_on();
			g_sensorAppCtx.sta = 1;
			interval = ledOnTime;
		}
		g_sensorAppCtx.ledOnTime = ledOnTime;
		g_sensorAppCtx.ledOffTime = ledOffTime;
#ifdef USE_EPD
		interval <<= 2;
#endif
		g_sensorAppCtx.timerLedEvt = TL_ZB_TIMER_SCHEDULE(zclLightTimerCb, NULL, interval);
	}
}

void light_blink_stop(void)
{
	if(g_sensorAppCtx.timerLedEvt){
		TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerLedEvt);
		//g_sensorAppCtx.timerLedEvt = NULL;
		g_sensorAppCtx.times = 0;
		if(g_sensorAppCtx.oriSta){
			light_on();
		}else{
			light_off();
		}
	}
}

/*******************************************************************
 * @brief	Button detect init:
 */
void keys_init(void) {
	g_sensorAppCtx.keyPressed = gpio_read(BUTTON1)? BUTTON1_ON : BUTTON1_OFF;
}
/*******************************************************************
 * @brief	Button timer callback
 */
static s32 keyTimerCb(void *arg) {
	task_keys();
	if(!g_sensorAppCtx.keyPressed) {
		g_sensorAppCtx.timerKeyEvt = NULL;
		return -1;
	}
	return 0;
}
/*******************************************************************
 * @brief	Button click detect:
 * 			short press button = send reports
 * 			keep press button 2s = Temperature DisplayMode С/F
 * 			keep press button 7s = Zigbee reset
 * 			keep press button 15s = Factory reset
 */
void task_keys(void) {
	if (g_sensorAppCtx.key_on_flag) { // event button on ?
		if(clock_time_exceed(g_sensorAppCtx.keyPressedTime, 14900 * 1000)) { // 15 sec
			TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerKeyEvt);
			//g_sensorAppCtx.timerKeyEvt = NULL;
			nv_resetModule(NV_MODULE_APP);
			init_nv_app();
#if	USE_DISPLAY
			if(!g_zcl_thermostatUICfgAttrs.display_off) {
				show_reset_screen();
			}
#endif // USE_DISPLAY
			drv_pm_longSleep(PM_SLEEP_MODE_DEEPSLEEP, PM_WAKEUP_SRC_TIMER, 3*1000);
		}
		else if((g_sensorAppCtx.key_on_flag & 2)
		 && clock_time_exceed(g_sensorAppCtx.keyPressedTime, 6900 * 1000)) { // 7 sec
			g_sensorAppCtx.key_on_flag &= ~2;
#if	USE_DISPLAY
			if(!g_zcl_thermostatUICfgAttrs.display_off) {
				show_reset_screen();
			}
#endif // USE_DISPLAY
			tl_bdbReset2FN();
			//nv_resetAll();
		}
#if USE_DISPLAY && defined(ZCL_THERMOSTAT_UI_CFG)
		else if((g_sensorAppCtx.key_on_flag & 1)
			&&	clock_time_exceed(g_sensorAppCtx.keyPressedTime, 1900 * 1000)) { // 2 sec
			g_sensorAppCtx.key_on_flag &= ~1;
			g_zcl_thermostatUICfgAttrs.TemperatureDisplayMode ^= 1;
			g_zcl_thermostatUICfgAttrs.TemperatureDisplayMode &= 1;
//			zcl_thermostatConfig_save();
			read_sensor_and_save();
		}
#endif
	}
	u8 button_on = gpio_read(BUTTON1)? BUTTON1_ON : BUTTON1_OFF;
	if(button_on) {
		// button on
		if(!g_sensorAppCtx.keyPressed) {
			// event button on
			light_on();
			g_sensorAppCtx.key_on_flag = 0xff;
			g_sensorAppCtx.keyPressedTime = clock_time();
			app_set_thb_report();
#if USE_BLE
			if(blt_state == BLS_LINK_STATE_ADV) {
				set_adv_time(ADV_INTERVAL_500MS);
				adv_buf.adv_restore_count = CONNECT_ADV_COUNT + 10; // на 5 секунд
				if(g_sensorAppCtx.timerKeyEvt)
					TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerKeyEvt);
			} else
#endif
			if(!g_sensorAppCtx.timerKeyEvt) {
				g_sensorAppCtx.timerKeyEvt = TL_ZB_TIMER_SCHEDULE(keyTimerCb, NULL, 500); // 500 ms
			}
		}
	} else {
		// button off
		if(g_sensorAppCtx.keyPressed) {
			// event button off
			light_off();
			if(g_sensorAppCtx.key_on_flag) {
				if((g_sensorAppCtx.key_on_flag & 2) == 0)
					drv_pm_longSleep(PM_SLEEP_MODE_DEEPSLEEP, PM_WAKEUP_SRC_TIMER, 3*1000);
				else if((g_sensorAppCtx.key_on_flag & 1) == 0)
					zcl_thermostatConfig_save();
			}
		}
		g_sensorAppCtx.key_on_flag = 0;
		if(g_sensorAppCtx.timerKeyEvt)
			TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerKeyEvt);
	}
	g_sensorAppCtx.keyPressed = button_on;
#if PM_ENABLE
#if (BUTTON1_OFF)
	cpu_set_gpio_wakeup(BUTTON1, button_on, 1); // button_on: 0: low-level wakeup, 1: high-level wakeup
#else
	cpu_set_gpio_wakeup(BUTTON1, !button_on, 1); // !button_on: Level_Low=0, Level_High =1
#endif
#endif // PM_ENABLE
}
