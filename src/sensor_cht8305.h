#pragma once
#include <stdint.h>


// Timing
#define SENSOR_POWER_TIMEOUT_ms  	5
#define SENSOR_RESET_TIMEOUT_ms		5
#define SENSOR_MEASURING_TIMEOUT_ms	7
#define SENSOR_MEASURING_TIMEOUT  (SENSOR_MEASURING_TIMEOUT_ms * CLOCK_16M_SYS_TIMER_CLK_1MS) // clk tick

//  I2C addres
#define CHT8305_I2C_ADDR		0x40
#define CHT8305_I2C_ADDR_MAX	0x43

//  Registers
#define CHT8305_REG_TMP		0x00
#define CHT8305_REG_HMD		0x01
#define CHT8305_REG_CFG		0x02
#define CHT8305_REG_ALR		0x03
#define CHT8305_REG_VLT		0x04
#define CHT8305_REG_MID		0xfe
#define CHT8305_REG_VID		0xff

//  Config register mask
#define CHT8305_CFG_SOFT_RESET          0x8000
#define CHT8305_CFG_CLOCK_STRETCH       0x4000
#define CHT8305_CFG_HEATER              0x2000
#define CHT8305_CFG_MODE                0x1000
#define CHT8305_CFG_VCCS                0x0800
#define CHT8305_CFG_TEMP_RES            0x0400
#define CHT8305_CFG_HUMI_RES            0x0300
#define CHT8305_CFG_ALERT_MODE          0x00C0
#define CHT8305_CFG_ALERT_PENDING       0x0020
#define CHT8305_CFG_ALERT_HUMI          0x0010
#define CHT8305_CFG_ALERT_TEMP          0x0008
#define CHT8305_CFG_VCC_ENABLE          0x0004
#define CHT8305_CFG_VCC_RESERVED        0x0003

struct __attribute__((packed)) _cht8305_config_t{
	u16 reserved 	: 2;
	u16 vccen		: 1;
	u16 talt		: 1;
	u16 halt 		: 1;
	u16 aps 		: 1;
	u16 altm		: 2;
	u16 h_res 		: 2;
	u16 t_res		: 1;
	u16 vccs		: 1;
	u16 mode		: 1;
	u16 heater		: 1;
	u16 clkstr		: 1;
	u16 srst		: 1;
} cht8305_config_t;


typedef struct _measured_data_t {
	u16	battery_mv; // mV
	u16	average_battery_mv; // mV
	s16	temp; // in 0.01 C
	s16	humi; // in 0.01 %
	u16	battery_level; // in 0.5%
} measured_data_t;

extern u8 sensor_i2c_addr;
extern u32 sensor_id;
extern measured_data_t measured_data;

void init_sensor(void);
//void start_measure_sensor(void);
int read_sensor(void);
//int read_sensor_cb(void);


