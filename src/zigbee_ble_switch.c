/********************************************************************************************************
 * @file    zigbee_ble_switch.c
 *
 * @brief   This is the source file for zigbee_ble_switch
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
#include "lcd.h"
#include "zb_common.h"
#include "stack/ble/ble.h"
#include "stack/ble/ble_config.h"
#include "stack/ble/ble_common.h"
#include "device.h"
#include "sensors.h"
#include "battery.h"
#include "ble_cfg.h"
#include "bthome_beacon.h"
#include "zigbee_ble_switch.h"

app_dualModeInfo_t g_dualModeInfo = {
		.slot     = DUALMODE_SLOT_BLE,
		.bleState = BLS_LINK_STATE_IDLE
};

#define ZB_RF_ISR_RECOVERY		do{  \
		if(CURRENT_SLOT_GET() == DUALMODE_SLOT_ZIGBEE) rf_set_irq_mask(FLD_RF_IRQ_RX|FLD_RF_IRQ_TX);  \
		}while(0)

_attribute_ram_code_
void switch_to_zb_context(void){
	backup_ble_rf_context();

	ZB_RADIO_RX_DISABLE;

	restore_zb_rf_context();
	//switch tx power for zb mode
	ZB_RADIO_TX_POWER_SET(g_zb_txPowerSet);

	ZB_RADIO_RX_ENABLE;

	CURRENT_SLOT_SET(DUALMODE_SLOT_ZIGBEE);
}


_attribute_ram_code_
void switch_to_ble_context(void){
    /* disable zb rx dma to avoid un-excepted rx interrupt*/
	ZB_RADIO_TX_DISABLE;

	ZB_RADIO_RX_DISABLE;

	restore_ble_rf_context();
	/* switch tx power for ble mode */
	ZB_RADIO_TX_POWER_SET(ble_attr.g_ble_txPowerSet);

	ZB_RADIO_RX_ENABLE;

	CURRENT_SLOT_SET(DUALMODE_SLOT_BLE);
}

#if SCAN_IN_ADV_STATE
/*
 * ble is doing scan in the state of ADV
 *
 * @param ref_tick  beginning time the ADV
 *
 * @param index     is to control the scan interval, scan interval is (ADV-interval * index/16)
 *
 * */
static u32	blc_scan_busy_in_adv(u32 ref_tick, u8 index){
	unsigned int ret = 0;
	if(blts.scan_extension_mask & BLS_FLAG_SCAN_IN_ADV_MODE){
		ret = ((unsigned int)((ref_tick + (blta.adv_interval >> 4) * (index & 0x0f)) - reg_system_tick)) < BIT(30);
	}
	return ret;
}
#endif


inline int is_switch_to_ble(void){
	return get_ble_next_event_tick() - (reg_system_tick + ZIGBEE_AFTER_TIME) > BIT(31);
}

inline int is_switch_to_zigbee(void){
	return get_ble_next_event_tick() - (reg_system_tick + BLE_IDLE_TIME) < BIT(31);
}


void zb_task(void){
	ev_main();

#if (MODULE_WATCHDOG_ENABLE)
	drv_wd_clear();
#endif

    tl_zbTaskProcedure();

}

void concurrent_mode_main_loop(void){
	u32 r = 0;

	APP_BLE_STATE_SET(BLE_BLT_STATE_GET()); //bltParam.blt_state);

	//zb_task();
	//blt_sdk_main_loop();
	//return;

	if(CURRENT_SLOT_GET() == DUALMODE_SLOT_BLE){
//		 g_dualModeInfo.bleTaskTick = clock_time();

		 /*
		  * ble task
		  *
		  * */
#if SCAN_IN_ADV_STATE
		 /* add scan functionality after advertising during ADV state, just for slave mode */
		 do{
			 /* disable pm during scan */
#if PM_ENABLE
			 if(BLE_BLT_STATE_GET() == BLS_LINK_STATE_ADV){
				 bls_pm_setSuspendMask (SUSPEND_DISABLE);
			 }
#endif // PM_ENABLE
			 blt_sdk_main_loop();
		 }while((BLE_BLT_STATE_GET() == BLS_LINK_STATE_ADV) && blc_scan_busy_in_adv(g_dualModeInfo.bleTaskTick,2));//MY_ADV_INTERVAL*3/4

		 /* enable pm after scan */
#if PM_ENABLE
		 bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
#endif // PM_ENABLE

#else // SCAN_IN_ADV_STATE
		 blt_sdk_main_loop();
#endif // SCAN_IN_ADV_STATE
#if USE_BLE_OTA
		 if(ble_attr.ota_is_working) {
			 bls_pm_setManualLatency(0);
		 }
#endif
		 if(APP_BLE_STATE_GET() == BLS_LINK_STATE_CONN) {
			 if(ble_attr.batteryValueInCCC && (measured_battery.flag & FLG_MEASURE_BAT_CC)) {
				 measured_battery.flag &= ~FLG_MEASURE_BAT_CC;
				 bls_att_pushNotifyData(BATT_LEVEL_INPUT_DP_H, (u8 *)&ble_attr.my_batVal, 1);
			 }
		 }

		 DBG_ZIGBEE_STATUS(0x30);

		 r = drv_disable_irq();

		 if(((get_ble_event_state() && is_switch_to_zigbee()) || APP_BLE_STATE_GET() == BLS_LINK_STATE_IDLE)
				 && (!g_dualModeInfo.switch_to_ble)
			 ){
			 /*
			  * ready to switch to ZIGBEE mode
			  *
			  * */
			DBG_ZIGBEE_STATUS(0x31);

			switch_to_zb_context();

			drv_restore_irq(r);
			zb_task();

		 } else {
			 drv_restore_irq(r);
		 }
		 DBG_ZIGBEE_STATUS(0x32);
	 } else {
		 /*
		  * now in zigbee mode
		  *
		  * */
		 DBG_ZIGBEE_STATUS(0x33);

		 r = drv_disable_irq();

		 if((!zb_rfTxDoing() && is_switch_to_ble() && APP_BLE_STATE_GET() != BLS_LINK_STATE_IDLE)
				 || g_dualModeInfo.switch_to_ble) {
			 /*
			  * ready to switch to BLE mode
			  *
			  * */
			 switch_to_ble_context();

			 DBG_ZIGBEE_STATUS(0x34);

			 drv_restore_irq(r);
			 return;
		 }

		 drv_restore_irq(r);
		 DBG_ZIGBEE_STATUS(0x35);
		 zb_task();
	 }
}

u8 ble_task_stop(void){

	ble_sts_t ret = BLE_SUCCESS;

	if(APP_BLE_STATE_GET() != BLS_LINK_STATE_CONN) {

		u8 r = irq_disable();
		ret = bls_ll_setAdvEnable(BLC_ADV_DISABLE);

		/* rf irq is cleared in the "bls_ll_setAdvEnable",
		 * so that the rf tx/rx interrupt will be missed if the "bls_ll_setAdvEnable" is called in Zigbee mode
		 */
		if(ret == BLE_SUCCESS){
			if(g_dualModeInfo.slot == DUALMODE_SLOT_ZIGBEE)
				rf_set_irq_mask(FLD_RF_IRQ_RX|FLD_RF_IRQ_TX);
		}
		irq_restore(r);

		if(ret  == BLE_SUCCESS) {
			g_dualModeInfo.bleStart = 0;
			set_adv_time(ADV_INTERVAL_50MS);
#if	USE_DISPLAY
			show_ble_symbol(false);
			update_lcd();
#endif
		}
	}
	return ret;
}


/* functions startAdvTime() */
void startAdvTime(u8 count, u16 interval) {

	set_adv_time(ADV_INTERVAL_50MS);

	u8 r = irq_disable();

	ble_sts_t ret = bls_ll_setAdvEnable(BLC_ADV_ENABLE);
	/* rf irq is cleared in the "bls_ll_setAdvEnable",
	 * so that the rf tx/rx interrupt will be missed if the "bls_ll_setAdvEnable" is called in Zigbee mode
	 */
	if(ret == BLE_SUCCESS) {
		if(g_dualModeInfo.slot == DUALMODE_SLOT_ZIGBEE)
			rf_set_irq_mask(FLD_RF_IRQ_RX|FLD_RF_IRQ_TX);
	}
	irq_restore(r);

	if(ret == BLE_SUCCESS) {
		//blta.adv_begin_tick = clock_time() + 50 *CLOCK_16M_SYS_TIMER_CLK_1MS;
		//blta.adv_interval = 50*625*CLOCK_16M_SYS_TIMER_CLK_1US; // system tick
		adv_buf.adv_restore_count = count;
		g_dualModeInfo.bleStart = 0;
		measured_battery.flag |= FLG_MEASURE_BAT_ADV;
#if	USE_DISPLAY
		show_ble_symbol(true);
		update_lcd();
#endif
	}

}
