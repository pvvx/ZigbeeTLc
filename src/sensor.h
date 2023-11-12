#pragma once
#include <stdint.h>

extern volatile uint32_t timer_measure_cb; // time start measure

#define SENSOR_MEASURING_TIMEOUT_ms  11 // SHTV3 11 ms, SHT4x max 8.2 ms
#define SENSOR_MEASURING_TIMEOUT  (SENSOR_MEASURING_TIMEOUT_ms * CLOCK_16M_SYS_TIMER_CLK_1MS) // clk tick

#define SHTC3_I2C_ADDR		0x70
#define SHT4x_I2C_ADDR		0x44
#define SHT4xB_I2C_ADDR		0x45

typedef struct _measured_data_t {
	u16		battery_mv; // mV
	s16		temp; // x 0.01 C
	s16		humi; // x 0.01 %
	u16 		battery_level;
} measured_data_t;

extern u8 sensor_i2c_addr;
extern u8 sensor_version;
extern measured_data_t measured_data;

void init_sensor(void);
void start_measure_sensor_deep_sleep(void);
int read_sensor_cb(void);
void sensor_go_sleep(void);
int read_sensor(void);


