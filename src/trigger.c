/*
 * trigger.c
 *
 *  Created on: 02.01.2021
 *      Author: pvvx
 */
#include "tl_common.h"

#if USE_TRIGGER

#include "zcl_include.h"
//#include "zcl_relative_humidity.h"
#include "zcl_thermostat_ui_cfg.h"
#include "device.h"

#include "trigger.h"

#ifndef ZCL_THERMOSTAT_UI_CFG
#error "Set ZCL_THERMOSTAT_UI_CFG!"
#endif

#define measured_val1	g_zcl_temperatureAttrs.measuredValue
#define measured_val2	g_zcl_relHumidityAttrs.measuredValue

const trigger_t def_trg = {
		.temp_threshold = 2100, // 21 °C
		.humi_threshold = 5000, // 50 %
		.temp_hysteresis = 0, // disable
		.humi_hysteresis = -1,  // disable
};

trigger_t trg;
trigger_t trg_chk;


void send_onoff(void) {
	if(trg.flg.trg_output != trg.flg.trigger_on) {
		trg.flg.trg_output = trg.flg.trigger_on;
#ifdef 	GPIO_TRG
		gpio_setup_up_down_resistor(GPIO_TRG, trg.flg.trg_output ? PM_PIN_PULLUP_10K : PM_PIN_PULLDOWN_100K);
#endif
		if(zb_isDeviceJoinedNwk()) {
			epInfo_t dstEpInfo;
			TL_SETSTRUCTCONTENT(dstEpInfo, 0);
			dstEpInfo.dstAddrMode = APS_SHORT_GROUPADDR_NOEP;
			dstEpInfo.dstEp = SENSOR_DEVICE_ENDPOINT;
			dstEpInfo.dstAddr.shortAddr = 0xfffc;
			if(trg.flg.trg_output) {
				zcl_onOff_onCmd(SENSOR_DEVICE_ENDPOINT, &dstEpInfo, TRUE);
			} else {
				zcl_onOff_offCmd(SENSOR_DEVICE_ENDPOINT, &dstEpInfo, TRUE);
			}
		}
	}
}

void trigger_init(void) {
	if(nv_flashReadNew(1, NV_MODULE_APP,  NV_ITEM_APP_TRIGGER_UI_CFG, sizeof(trg) - 1, (u8*)&trg) != NV_SUCC){
		memcpy(&trg, &def_trg, sizeof(def_trg));
	}
	memcpy(&trg_chk, &trg, sizeof(trg));
}

nv_sts_t trigger_save(void) {
	nv_sts_t st = NV_SUCC;
	if(memcmp(&trg_chk, &trg, sizeof(trg_chk) - 1)) {
		st = nv_flashWriteNew(1, NV_MODULE_APP,  NV_ITEM_APP_TRIGGER_UI_CFG, sizeof(trg) - 1, (u8*)&trg);
		memcpy(&trg_chk, &trg, sizeof(trg));
	}
	return st;
}

__attribute__((optimize("-Os")))
void set_trigger_out(void) {
	if (trg.temp_hysteresis) {
		if (trg.flg.temp_out_on) { // temp_out on
			if (trg.temp_hysteresis < 0) {
				if (measured_val1 > trg.temp_threshold - trg.temp_hysteresis) {
					trg.flg.temp_out_on = false;
				}
			} else {
				if (measured_val1 < trg.temp_threshold - trg.temp_hysteresis) {
					trg.flg.temp_out_on = false;
				}
			}
		} else { // temp_out off
			if (trg.temp_hysteresis < 0) {
				if (measured_val1 < trg.temp_threshold + trg.temp_hysteresis) {
					trg.flg.temp_out_on = true;
				}
			} else {
				if (measured_val1 > trg.temp_threshold + trg.temp_hysteresis) {
					trg.flg.temp_out_on = true;
				}
			}
		}
	} else trg.flg.temp_out_on = false;
	if (trg.humi_hysteresis) {
		if (trg.flg.humi_out_on) { // humi_out on
			if (trg.humi_hysteresis < 0) {
				if (measured_val2 > trg.humi_threshold - trg.humi_hysteresis) {
					// humi > threshold
					trg.flg.humi_out_on = false;
				}
			} else { // hysteresis > 0
				if (measured_val2 < trg.humi_threshold - trg.humi_hysteresis) {
					// humi < threshold
					trg.flg.humi_out_on = false;
				}
			}
		} else { // humi_out off
			if (trg.humi_hysteresis < 0) {
				if (measured_val2 < trg.humi_threshold + trg.humi_hysteresis) {
					// humi < threshold
					trg.flg.humi_out_on = true;
				}
			} else { // hysteresis > 0
				if (measured_val2 > trg.humi_threshold + trg.humi_hysteresis) {
					// humi > threshold
					trg.flg.humi_out_on = true;
				}
			}
		}
	} else trg.flg.humi_out_on = false;

	if (trg.temp_hysteresis || trg.humi_hysteresis) {
		trg.flg.trigger_on = true;
//		trg.flg.trg_output = (trg.flg.humi_out_on || trg.flg.temp_out_on);
	} else {
		trg.flg.trigger_on = false;
	}
}

#endif  // USE_TRIGGER
