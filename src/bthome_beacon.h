/*
 * bthome_beacon.h
 *
 *  Created on: 17.10.23
 *      Author: pvvx
 */

#ifndef BTHOME_BEACON_H_
#define BTHOME_BEACON_H_

#include "stack/ble/ble_8258/ble_common.h"

#define ADV_BTHOME_UUID16 0xFCD2 // 16-bit UUID Service 0xFCD2 BTHOME

#define BtHomeID_Info	0x40
#define BtHomeID_Info_Encrypt	0x41


// https://github.com/custom-components/ble_monitor/issues/548
typedef enum {
	BtHomeID_PacketId = 0,			//0x00, uint8
	BtHomeID_battery = 0x01,		//0x01, uint8, %
	BtHomeID_temperature = 0x02,	//0x02, sint16, 0.01 °C
	BtHomeID_humidity = 0x03,		//0x03, uint16, 0.01 %
	BtHomeID_pressure = 0x04,		//0x04, uint24, 0.01 hPa
	BtHomeID_illuminance = 0x05,	//0x05, uint24, 0.01 lux
	BtHomeID_weight	= 0x06,			//0x06, uint16, 0.01 kg
	BtHomeID_weight_lb = 0x07,      //0x07, uint16, 0.01 lb
	BtHomeID_dewpoint = 0x08,		//0x08, sint16, 0.01 °C
	BtHomeID_count8 = 0x09,			//0x09,	uint8
	BtHomeID_energy24 = 0x0a,		//0x0A, uint24, 0.001 kWh
	BtHomeID_power24 = 0x0b,		//0x0B, uint24, 0.01 W
	BtHomeID_voltage = 0x0c,		//0x0C, uint16, 0.001 V
	BtHomeID_pm2x5 = 0x0d,			//0x0D, uint16, kg/m3
	BtHomeID_pm10 = 0x0e,			//0x0E, uint16, kg/m3
	BtHomeID_boolean = 0x0f,		//0x0F, uint8, generic boolean
	BtHomeID_switch = 0x10,		  	//0x10, uint8, power on/off
	BtHomeID_opened = 0x11,			//0x11, uint8, opening =0 Closed, = 1 Open
	BtHomeID_co2 = 0x12,			//0x12, uint16
	BtHomeID_tvoc = 0x13,			//0x13, uint16
	BtHomeID_moisture16 = 0x14,		//0x14, uint16, 0.01
	BtHomeID_low_battery = 0x15,	//0x15, uint8, =1 low
	BtHomeID_chg_battery = 0x16,	//0x16, uint8, battery charging
	BtHomeID_carbon_monoxide = 0x17,//0x17, uint8, carbon monoxide
	BtHomeID_cold = 0x18,			//0x18, uint8
	BtHomeID_connectivity = 0x19,	//0x19, uint8
	BtHomeID_door = 0x1a,			//0x1a, uint8, =0 Closed, =1 Open
	BtHomeID_garage_door = 0x1b,	//0x1b, uint8, =0 Closed, =1 Open
	BtHomeID_gas = 0x1c,			//0x1c, uint8, =1 Detected
	BtHomeID_heat = 0x1d,			//0x1d, uint8, =1 Hot
	BtHomeID_light = 0x1e,			//0x1e, uint8, =1 Light detected
	BtHomeID_lock = 0x1f,			//0x1f, uint8, =1 Unlocked
	BtHomeID_moisture_b = 0x20,		//0x20, uint8, =0 Dry, =1 Wet
	BtHomeID_motion = 0x21,			//0x21, uint8, =0 Clear, =1 Detected
	BtHomeID_moving = 0x22,			//0x22, uint8, =1 Moving
	BtHomeID_occupancy = 0x23,		//0x23, uint8, =1 Detected
	BtHomeID_plug = 0x24,			//0x24, uint8, =0 Unplugged, =1 Plugged in
	BtHomeID_presence = 0x25,		//0x25, uint8, =0 Away, =1 Home
	BtHomeID_problem = 0x26,		//0x26, uint8, =0 Ok, =1 Problem
	BtHomeID_running = 0x27,		//0x27, uint8, =0 Not Running, =1 Running
	BtHomeID_safety = 0x28,			//0x28, uint8, =0 Unsafe, =1 Safe
	BtHomeID_smoke = 0x29,			//0x29, uint8, =0 Clear, =1 Detected
	BtHomeID_sound = 0x2a,			//0x2a, uint8, =0 Clear, =1 Detected
	BtHomeID_tamper = 0x2b,			//0x2b, uint8, =0 Off, =1 On
	BtHomeID_vibration = 0x2c,		//0x2c, uint8, =0 Clear, =1 Detected
	BtHomeID_window = 0x2d,			//0x2d, uint8, =0 Closed, =1 Open
	BtHomeID_humidity8 = 0x2e,		//0x2e, uint8
	BtHomeID_moisture8 = 0x2f,		//0x2f, uint8
	BtHomeID_0x30 = 0x30,		//0x30, uint8
	BtHomeID_0x31 = 0x31,		//0x31, uint8
	BtHomeID_0x32 = 0x32,		//0x32, uint8
	BtHomeID_0x33 = 0x33,		//0x33, uint8
	BtHomeID_0x34 = 0x34,		//0x34, uint8
	BtHomeID_0x35 = 0x35,		//0x35, uint8
	BtHomeID_0x36 = 0x36,		//0x36, uint8
	BtHomeID_0x37 = 0x37,		//0x37, uint8
	BtHomeID_0x38 = 0x38,		//0x38, uint8
	BtHomeID_0x39 = 0x39,		//0x39, uint8
	BtHomeID_button = 0x3a,			//0x3a, uint8, =1 press, =2 double_press ... https://bthome.io/format/
	BtHomeID_0x3b = 0x3b,		//0x3b, uint8
	BtHomeID_dimmer = 0x3c,			//0x3c, uint16 ?, =1 rotate left 3 steps, ... https://bthome.io/format/
	BtHomeID_count16 = 0x3d,		//0x3d, uint16
	BtHomeID_count32 = 0x3e,		//0x3e, uint32
	BtHomeID_rotation = 0x3f,		//0x3f, sint16, 0.1
	BtHomeID_distance_mm  = 0x40,	//0x40, uint16, mm
	BtHomeID_distance_m = 0x41,		//0x41, uint16, m, 0.1
	BtHomeID_duration = 0x42,		//0x42, uint24, 0.01
	BtHomeID_current = 0x43,		//0x43, uint16, 0.001
	BtHomeID_speed = 0x44,			//0x44, uint16, 0.01
	BtHomeID_temperature_01 = 0x45,	//0x45, sint16, 0.1
	BtHomeID_UV_index = 0x46,		//0x46, uint8, 0.1
	BtHomeID_volume16_01 = 0x47,	//0x47, uint16, 0.1
	BtHomeID_volume16 = 0x48,		//0x48, uint16, 1
	BtHomeID_Flow_Rate = 0x49,		//0x49, uint16, 0.001
	BtHomeID_voltage_01 = 0x4a,		//0x4a, uint16, 0.1
	BtHomeID_gas24 = 0x4b,			//0x4b, uint24, 0.001
	BtHomeID_gas32 = 0x4c,			//0x4c, uint32, 0.001
	BtHomeID_energy32 = 0x4d,		//0x4d, uint32, 0.001
	BtHomeID_volume32 = 0x4e,		//0x4e, uint32, 0.001
	BtHomeID_water32 = 0x4f,		//0x4f, uint32, 0.001
	BtHomeID_timestamp = 0x50,		//0x50, uint48
	BtHomeID_acceleration = 0x51,	//0x51, uint16, 0.001
	BtHomeID_gyroscope = 0x52,		//0x52, uint16, 0.001
	BtHomeID_text = 0x53,			//0x53, size uint8, uint8[]
	BtHomeID_raw = 0x54				//0x54, size uint8, uint8[]
} BtHomeIDs_e;

typedef struct __attribute__((packed)) _adv_head_bth_t {
	u8		size;   // =
	u8		type;	// = 0x16, 16-bit UUID
	u16		UUID;	// = 0xFCD2, GATT Service BTHome
} adv_head_bth_t, * padv_head_bth_t;

typedef struct __attribute__((packed)) _adv_bthome_data_bt_t {
	u8		b_id;	// = BtHomeID_battery
	u8		battery_level; // 0..100 %
	u8		v_id;	// = BtHomeID_voltage
	u16		battery_mv; // x 0.001 V
} adv_bthome_data_bt_t, * padv_bthome_data_bt_t;

typedef struct __attribute__((packed)) _adv_bthome_data_ht_t {
	u8		b_id;	// = BtHomeID_battery
	u8		battery_level; // 0..100 %
	u8		t_id;	// = BtHomeID_temperature
	s16		temperature; // x 0.01 degree
	u8		h_id;	// = BtHomeID_humidity
	u16		humidity; // x 0.01 %
#if (DEV_SERVICES & SERVICE_PLM)
	u8		m_id;	// = BtHomeID_moisture16
	u16		moisture; // x 0.01 %
#endif
	u8		v_id;	// = BtHomeID_voltage
	u16		battery_mv; // x 0.001 V
} adv_bthome_data_ht_t, * padv_bthome_data_ht_t;


// BTHOME data1, no security
typedef struct __attribute__((packed)) _adv_bthome_ns_bt_t {
	u8 	flag[3];		// Advertise type flags
	adv_head_bth_t head;
	u8		info;	// = 0x40 BtHomeID_Info
	u8		p_id;	// = BtHomeID_PacketId
	u8		pid;	// PacketId (measurement count)
	adv_bthome_data_bt_t data;
} adv_bthome_ns_bt_t, * padv_bthome_ns_bt_t;

// BTHOME data2, no security
typedef struct __attribute__((packed)) _adv_bthome_ns_ht_t {
	u8 	flag[3];		// Advertise type flags
	adv_head_bth_t head;
	u8		info;	// = 0x40 BtHomeID_Info
	u8		p_id;	// = BtHomeID_PacketId
	u8		pid;	// PacketId (measurement count)
	adv_bthome_data_ht_t data;
} adv_bthome_ns_ht_t, * padv_bthome_ns_ht_t;

//#define ADV_BUFFER_SIZE		(31-3)

typedef struct _adv_buf_t {
	u32	send_count; // count & id advertise, = beacon_nonce.cnt32
	u16	old_measured_count; // old measured_data.count
	u8  adv_restore_count;
	union {
		adv_bthome_ns_bt_t bt;
		adv_bthome_ns_ht_t ht;
	} data;
} adv_buf_t;

extern adv_buf_t adv_buf;

void set_adv_time(u32 adv_time);
void bls_set_advertise_prepare(void *p);
int app_advertise_prepare_handler(rf_packet_adv_t * p);

#endif /* BTHOME_BEACON_H_ */
