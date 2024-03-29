#ifndef _SENSOR_SHTC3_4X_H_
#define _SENSOR_SHTC3_4X_H_

#if (SENSOR_TYPE == SENSOR_SHTC3_4X)

#define SHTC3_I2C_ADDR		0x70
#define SHT4x_I2C_ADDR		0x44
#define SHT4xB_I2C_ADDR		0x45

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

void init_sensor(void);
void sensor_go_sleep(void);
int read_sensor(void);

#endif // (SENSOR_TYPE == SENSOR_SHTC3_4X)
#endif // _SENSOR_SHTC3_4X_H_
