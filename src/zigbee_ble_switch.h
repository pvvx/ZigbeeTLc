/********************************************************************************************************
 * @file    zigbee_ble_switch.h
 *
 * @brief   This is the header file for zigbee_ble_switch
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

#ifndef _ZIGBEE_BLE_SWITCH_H_
#define _ZIGBEE_BLE_SWITCH_H_

#define  ZIGBEE_AFTER_TIME    (4*CLOCK_16M_SYS_TIMER_CLK_1MS)	//4ms
#define  BLE_IDLE_TIME   	  (4*CLOCK_16M_SYS_TIMER_CLK_1MS)	//5ms

typedef enum{
	DUALMODE_SLOT_BLE = 0,
	DUALMODE_SLOT_ZIGBEE,
}app_currentSlot_e;


typedef struct{
	volatile app_currentSlot_e slot;
	u8				bleState;		// DUALMODE_SLOT_BLE / DUALMODE_SLOT_ZIGBEE
	u8				bleStart;		// enable BLE Adv
	volatile u8		switch_to_ble;
}app_dualModeInfo_t;

/*
typedef void (*master_service_t) (void);
typedef void (*master_update_t) (void);
typedef struct{
	master_service_t serviceCb;
	master_update_t  updateCb;
}ble_master_cb_t;
*/

extern app_dualModeInfo_t g_dualModeInfo;
#define CURRENT_SLOT_GET()			 g_dualModeInfo.slot
#define CURRENT_SLOT_SET(s)			 g_dualModeInfo.slot = s
#define APP_BLE_STATE_SET(state)	 g_dualModeInfo.bleState = state
#define APP_BLE_STATE_GET()			 g_dualModeInfo.bleState

#define ZB_RF_ISR_RECOVERY		do{  \
				if(CURRENT_SLOT_GET() == DUALMODE_SLOT_ZIGBEE) rf_set_irq_mask(FLD_RF_IRQ_RX|FLD_RF_IRQ_TX);  \
				}while(0)

void switch_to_zb_context(void);

void switch_to_ble_context(void);

int is_switch_to_ble(void);

int is_switch_to_zigbee(void);

u8 ble_task_stop(void);

void zb_ble_switch_proc(void);

void concurrent_mode_main_loop(void);

#endif // _ZIGBEE_BLE_SWITCH_H_
