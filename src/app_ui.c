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
#include "zb_api.h"
#include "zcl_include.h"
#include "lcd.h"
#include "app_ui.h"
#include "sensor.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * LOCAL FUNCTIONS
 */

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
		if(g_sensorAppCtx.keyPressed
			&& g_sensorAppCtx.timerKeyEvt
			&& clock_time_exceed(
					g_sensorAppCtx.keyPressedTime,
					3000 * 1000)) { // 3 sec
			g_sensorAppCtx.keyPressedTime = clock_time();
#if BOARD == BOARD_MHO_C401N
			show_connected_symbol(false);
			update_lcd();
#else
			//show_ble_symbol(true);
			show_blink_screen();
#endif

			tl_bdbReset2FN();
#ifdef USE_EPD
			while(task_lcd())
				pm_wait_ms(USE_EPD);
#endif
			zb_resetDevice();
		} else {
			g_sensorAppCtx.keyPressed = button_on;
			return 0;
		}
	}
	g_sensorAppCtx.timerKeyEvt = NULL;
	g_sensorAppCtx.keyPressed = button_on;
	return -1;
}

void task_keys(void) {
	u8 button_on = gpio_read(BUTTON1)? 0 : 1;
	if(button_on) {
		// button on
		if(!g_sensorAppCtx.keyPressed) {
			// event button on
			g_sensorAppCtx.keyPressedTime = clock_time();
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
