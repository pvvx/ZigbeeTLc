/*
 * bthome_beacon.c
 *
 *  Created on: 17.10.23
 *      Author: pvvx
 */

#include "tl_common.h"
#include "ble_cfg.h" // 8258/gap/gap.h"
#include "stack/ble/ble.h" // 8258/gap/gap.h"
#include "device.h"
#include "sensors.h"
#include "battery.h"
#include "bthome_beacon.h"
#include "zigbee_ble_switch.h"
#if (DEV_SERVICES & SERVICE_PLM)
#include "rh.h"
#endif

#define USE_OUT_AVERAGE_BATTERY		1

adv_buf_t adv_buf = {
		.data.ht.flag = {02, GAP_ADTYPE_FLAGS, 0x04 | GAP_ADTYPE_LE_GENERAL_DISCOVERABLE_MODE_BIT}
};

extern u8 my_batVal;
//void bls_set_advertise_prepare(void *p);

//_attribute_ram_code_
__attribute__((optimize("-Os")))
u8 bthome_data_beacon(padv_bthome_ns_ht_t p, u32 count) {
	//	padv_bthome_ns1_t p = (padv_bthome_ns1_t)&adv_buf.data;
	p->flag[0] = 0x02; // size
	p->flag[1] = GAP_ADTYPE_FLAGS; // type
	/*	Flags:
	 	bit0: LE Limited Discoverable Mode
		bit1: LE General Discoverable Mode
		bit2: BR/EDR Not Supported
		bit3: Simultaneous LE and BR/EDR to Same Device Capable (Controller)
		bit4: Simultaneous LE and BR/EDR to Same Device Capable (Host)
		bit5..7: Reserved
	 */
	p->flag[2] = 0x04 | GAP_ADTYPE_LE_GENERAL_DISCOVERABLE_MODE_BIT; // Flags
	p->head.type = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->head.UUID = ADV_BTHOME_UUID16;
	p->info = BtHomeID_Info;
	p->p_id = BtHomeID_PacketId;
	p->pid = (u8)count;
	p->data.b_id = BtHomeID_battery;
	p->data.battery_level = measured_battery.batVal;
	if(sensor_ht.flag & FLG_MEASURE_HT_ADV) {
		sensor_ht.flag &= ~FLG_MEASURE_HT_ADV;
		p->data.t_id = BtHomeID_temperature;
		p->data.temperature = sensor_ht.temp; // x0.01 C
		p->data.h_id = BtHomeID_humidity;
		p->data.humidity = sensor_ht.humi; // x0.01 %
#if (DEV_SERVICES & SERVICE_PLM)
		p->data.m_id = BtHomeID_moisture16;
		p->data.moisture = sensor_rh.rh; // x0.01 %
#endif
#if USE_OUT_AVERAGE_BATTERY
		p->data.battery_mv = measured_battery.mv; // x mV
#else
		p->data.battery_mv = measured_battery.mv; // x mV
#endif
		p->head.size = sizeof(adv_bthome_ns_ht_t) - sizeof(p->head.size) - sizeof(p->flag);
	} else {
		p->data.t_id = BtHomeID_voltage;
#if USE_OUT_AVERAGE_BATTERY
		p->data.temperature = measured_battery.mv; // x mV
#else
		p->data.temperature = measured_battery.mv; // x mV
#endif
		p->head.size = sizeof(adv_bthome_ns_bt_t) - sizeof(p->head.size) - sizeof(p->flag);
	}
	return p->head.size;
}

void set_adv_time(u32 adv_time) {
	blta.advInt_min = adv_time;
	blta.advInt_max = adv_time + 10;
	blta.adv_interval = adv_time*625*CLOCK_16M_SYS_TIMER_CLK_1US; // system tick
}

//	bls_set_advertise_prepare(app_advertise_prepare_handler); // TODO: not work if EXTENDED_ADVERTISING
int app_advertise_prepare_handler(rf_packet_adv_t * p)	{
	(void) p;
	if(adv_buf.adv_restore_count) {
		if(adv_buf.adv_restore_count == CONNECT_ADV_COUNT)
			// restore next adv. interval
			set_adv_time(CONNECT_ADV_INTERVAL);
		adv_buf.adv_restore_count--;
		if(adv_buf.adv_restore_count == 0)
			// restore next adv. interval
			set_adv_time(DEF_ADV_INTERVAL);
	}
	if (measured_battery.flag & FLG_MEASURE_BAT_ADV) { // new sensor_ht ?
		measured_battery.flag &= ~FLG_MEASURE_BAT_ADV;
		adv_buf.send_count++; // count & id advertise, = beacon_nonce.cnt32
		bls_ll_setAdvData((u8 *)&adv_buf.data.ht, bthome_data_beacon(&adv_buf.data.ht, adv_buf.send_count));
	}
	return 1;		// = 1 ready to send ADV packet, = 0 not send ADV
}



