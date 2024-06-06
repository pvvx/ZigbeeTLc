
#include "tl_common.h"
#if defined(SENSOR_TYPE) && SENSOR_TYPE == SENSOR_AHT2X3X
#include "app_cfg.h"
#include "device.h"
#include "chip_8258/timer.h"

#include "i2c_drv.h"
#include "device.h"
#include "sensors.h"

#define _SENSOR_SPEED_CODE_SEC_ _attribute_ram_code_sec_

#ifndef USE_SENSOR_ID
#warning "SET USE_SENSOR_ID!"
#define USE_SENSOR_ID	0
#endif

u8 sensor_i2c_addr;

u32 sensor_id; //  = (0x20<<16) | (AHT2x_I2C_ADDR << 8);

measured_data_t measured_data;

//_attribute_ram_code_
static u8 sensor_crc(u8 * msg, int len) {
	int i,x;
	u8 crc = 0xff;
	for(x = 0; x < len; x++) {
		crc ^= msg[x];
		for(i = 8; i > 0; i--) {
			if (crc & 0x80)
				crc = (crc << 1) ^ (CRC_POLYNOMIAL & 0xff);
			else
				crc = (crc << 1);
		}
	}
	return crc;
}

//_attribute_ram_code_
int start_measure_sensor(void) {
	const u8 data[3] = {AHT2x_CMD_TMS, AHT2x_DATA1_TMS, AHT2x_DATA2_TMS};
	return send_i2c_bytes(sensor_i2c_addr, (u8 *)data, sizeof(data));
}

void check_sensor(void) {
	sensor_i2c_addr = scan_i2c_addr(AHT2x_I2C_ADDR << 1);
	if(sensor_i2c_addr) {
		start_measure_sensor();
	}
}

__attribute__((optimize("-Os"))) int read_sensor(void) {
	u32 _temp, _humi;
	u8 data[7];
	battery_detect();
	if (sensor_i2c_addr != 0) {
		if (read_i2c_bytes(sensor_i2c_addr, data, sizeof(data)) == 0
	    && (data[0] & 0x80) == 0
	    && sensor_crc(data, sizeof(data)) == 0) {
			_temp = (data[3] & 0x0F) << 16 | (data[4] << 8) | data[5];
			measured_data.temp = ((u32)(_temp * 1250) >> 16) - 5000 + g_zcl_thermostatUICfgAttrs.temp_offset; // x 0.01 C
			_humi = (data[1] << 12) | (data[2] << 4) | (data[3] >> 4);
			measured_data.humi = ((u32)(_humi * 625) >> 16) + g_zcl_thermostatUICfgAttrs.humi_offset; // x 0.01 %
			if (measured_data.humi < 0)
				measured_data.humi = 0;
			else if (measured_data.humi > 9999)
				measured_data.humi = 9999;
			// measured_data.count++;
			if(!start_measure_sensor())
				return 1;
		} else {
			send_i2c_byte(sensor_i2c_addr, AHT2x_CMD_RST); // Soft reset command
			sensor_i2c_addr = 0;
			return 0;
		}
	}
	check_sensor();
	return 0;
}

void init_sensor(void) {
	send_i2c_byte(0, 0x06); // Reset command using the general call address
	pm_wait_ms(SENSOR_POWER_TIMEOUT_ms);
	sensor_i2c_addr = scan_i2c_addr(AHT2x_I2C_ADDR << 1);
	if(sensor_i2c_addr) {
		//read_i2c_bytes(sensor_i2c_addr, (u8 *)&sensor_id, 1);
		send_i2c_byte(sensor_i2c_addr, AHT2x_CMD_RST); // Soft reset command
		pm_wait_ms(SENSOR_RESET_TIMEOUT_ms);
		send_i2c_byte(sensor_i2c_addr, AHT2x_CMD_RST); // Soft reset command
		pm_wait_ms(SENSOR_RESET_TIMEOUT_ms);
		if(!start_measure_sensor()) {
			pm_wait_ms(SENSOR_MEASURING_TIMEOUT_ms);
			read_sensor();
		} else {
			sensor_i2c_addr = 0;
		}
	}
}


#endif // defined(SENSOR_TYPE) && SENSOR_TYPE == SENSOR_AHT2X3X

