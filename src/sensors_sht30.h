#ifndef _SENSOR_SHT30_H_
#define _SENSOR_SHT30_H_

#if (SENSOR_TYPE == SENSOR_SHT30)


typedef struct _measured_data_t {
	u16	battery_mv; // mV
	u16	average_battery_mv; // mV
	s16	temp; // in 0.01 C
	s16	humi; // in 0.01 %
	u16	battery_level; // in 0.5%
} measured_data_t;

extern u8 sensor_i2c_addr;
extern u8 sensor_version;
extern measured_data_t measured_data;

#define sensor_go_sleep()

void init_sensor(void);
int read_sensor(void);

#endif // (SENSOR_TYPE == SENSOR_SHT30)
#endif // _SENSOR_SHT30_H_
