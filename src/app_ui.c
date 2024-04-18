/********************************************************************************************************
 * @file    app_ui.c
 *
 * @brief   This is the source file for app_ui
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
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

/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * LOCAL FUNCTIONS
 */
#if	USE_DISPLAY
void light_on(void)
{
    show_ble_symbol(true);
    update_lcd();
}

void light_off(void)
{
    show_ble_symbol(false);
    update_lcd();
}

void light_init(void)
{
    show_ble_symbol(false);
    update_lcd();
}
#else
void light_on(void)
{
	gpio_write(GPIO_LED, LED_ON);
#if LED_ON == 1
	gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLUP_10K);
#else
	gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLDOWN_100K);
#endif
}

void light_off(void)
{
	gpio_write(GPIO_LED, LED_OFF);
	gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_UP_DOWN_FLOAT);
}

void light_init(void)
{
	light_off();
}
#endif

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
#if	USE_DISPLAY
	if(g_zcl_thermostatUICfgAttrs.display_off)
		return;
#endif
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
		g_sensorAppCtx.timerLedEvt = NULL;
		g_sensorAppCtx.times = 0;
		if(g_sensorAppCtx.oriSta){
			light_on();
		}else{
			light_off();
		}
	}
}

/*******************************************************************
 * @brief	Button click detect:
 * 			keep press button1 3s === factory reset
 */

static s32 keyTimerCb(void *arg)
{
	u8 button_on = gpio_read(BUTTON1)? 0 : 1;
	if(button_on) {
		// button on
		if(g_sensorAppCtx.keyPressed && g_sensorAppCtx.timerKeyEvt) {
#if USE_DISPLAY && defined(ZCL_THERMOSTAT_UI_CFG)
			if(!g_sensorAppCtx.key1flag && clock_time_exceed( g_sensorAppCtx.keyPressedTime, 1900 * 1000)) { // 2 sec
				g_sensorAppCtx.key1flag = 1;
				g_zcl_thermostatUICfgAttrs.TemperatureDisplayMode ^= 1;
				g_zcl_thermostatUICfgAttrs.TemperatureDisplayMode &= 1;
				zcl_thermostatConfig_save();
				read_sensor_and_save();
			} else
#endif
			if(clock_time_exceed( g_sensorAppCtx.keyPressedTime, 6900 * 1000)) { // 7 sec
				g_sensorAppCtx.keyPressedTime = clock_time();
				tl_bdbReset2FN();
#if	USE_DISPLAY
				if(!g_zcl_thermostatUICfgAttrs.display_off) {
#ifdef USE_EPD
					while(task_lcd())
						pm_wait_ms(USE_EPD);
#endif
					show_blink_screen();
#ifdef USE_EPD
					while(task_lcd())
						pm_wait_ms(USE_EPD);
#endif
				}
#else
					light_on();
#endif // USE_DISPLAY
				//	zb_resetDevice();
				drv_pm_sleep(PM_SLEEP_MODE_DEEPSLEEP, PM_WAKEUP_SRC_PAD | PM_WAKEUP_SRC_TIMER, 5*1000);
			}
		} else {
			g_sensorAppCtx.keyPressed = button_on;
		}
		return 0;
	}
	g_sensorAppCtx.timerKeyEvt = NULL;
	g_sensorAppCtx.keyPressed = button_on;
	return -1;
}

void task_keys(void) {
	u8 button_on = gpio_read(BUTTON1)? 0 : 1;
	if(button_on) {
		// button on
#if	!USE_DISPLAY
		light_on();
#endif
		if(!g_sensorAppCtx.keyPressed) {
			// event button on
			g_sensorAppCtx.key1flag = 0;
			g_sensorAppCtx.keyPressedTime = clock_time();
			app_set_thb_report();
			if(!g_sensorAppCtx.timerKeyEvt)
				g_sensorAppCtx.timerKeyEvt
				= TL_ZB_TIMER_SCHEDULE(keyTimerCb, NULL, 500); // 500 ms
		}
	}
	g_sensorAppCtx.keyPressed = button_on;
#if PM_ENABLE
	cpu_set_gpio_wakeup(BUTTON1, button_on , 1);
#endif
}
