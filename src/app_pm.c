
/********************************************************************************************************
 * @file    app_pm.c
 *
 * @brief   This is the source file for app_pm
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *******************************************************************************************************/

#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "ota.h"
#include "device.h"
#include "app_ui.h"
#include "zigbee_ble_switch.h"
#include "stack/ble/ble_config.h"
#include "stack/ble/ble_common.h"
#include "stack/ble/ble.h"
#include "ble_cfg.h"
#include "bthome_beacon.h"
#include "sensors.h"

#if PM_ENABLE

//extern volatile bool g_bleConnDoing;

bool app_zigbeeIdle(void){
	bool ret = 0;
	
	ret = bdb_isIdle() && !tl_stackBusy() && zb_isTaskDone();
	if(ret){
		apsCleanToStopSecondClock();
		ret &= !ev_timer_process(1);
		if(!ret){
			secondClockRun();
		}
		//ret = 1; // TODO ?
	}

	return ret;
}

void app_pm_task(void) {
	drv_pm_sleep_mode_e sleepMode = PM_SLEEP_MODE_DEEP_WITH_RETENTION; //PM_SLEEP_MODE_SUSPEND;//PM_SLEEP_MODE_DEEP_WITH_RETENTION;
	drv_pm_wakeup_src_e wakeupSrc = PM_WAKEUP_SRC_PAD | PM_WAKEUP_SRC_TIMER;
	/*
	 * 3 low power mode:
	 * 1) in ble connection state, internal ble suspend is used,
	 * 2) in ble advertise state, system enters retention-deep(wakeup interval is ble advertise interval)
	 * 3) in ble IDLE state(just zigbee is working),
	 *    system enters retention-deep(poll rate is enabled) mode or
	 *    deep mode(poll rate is disabled)
	 *
	 * */
	if(APP_BLE_STATE_GET() == BLS_LINK_STATE_CONN || ble_attr.g_bleConnDoing){
		if(!bls_pm_conditionCbIsValid()){
			bls_pm_conditionCbRegister(app_zigbeeIdle);   // register it to enable ble suspend mode
		}
	} else if(APP_BLE_STATE_GET() == BLS_LINK_STATE_ADV){
		if(bls_pm_conditionCbIsValid()){
			bls_pm_conditionCbUnregister();
		}
	}

	g_dualModeInfo.switch_to_ble = 0;
	if(sensor_ht.read_callback) {
		sensor_ht.read_callback();
	} else if(CURRENT_SLOT_GET() == DUALMODE_SLOT_ZIGBEE && app_zigbeeIdle()){
		// task_keys();
		if(APP_BLE_STATE_GET() == BLS_LINK_STATE_IDLE){
			if(g_dualModeInfo.bleStart) {
			 	if(g_dualModeInfo.bleStart < CONNECT_ADV_COUNT)
			 		startAdvTime(1, CONNECT_ADV_INTERVAL);
			 	else
			 		startAdvTime(CONNECT_ADV_COUNT, CONNECT_ADV_INTERVAL);
			}
			drv_pm_lowPowerEnter();
		} else if(!is_switch_to_ble()) {
			if(APP_BLE_STATE_GET() == BLS_LINK_STATE_ADV){
				drv_pm_sleep(sleepMode, wakeupSrc, get_ble_next_event_tick());
			} else if(APP_BLE_STATE_GET() == BLS_LINK_STATE_CONN || ble_attr.g_bleConnDoing){
				g_dualModeInfo.switch_to_ble = 1;
			}
		}
	}
}

#endif
