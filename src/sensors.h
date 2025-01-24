/*
 * sensor.h
 *
 *  Created on: 14 нояб. 2023 г.
 *      Author: pvvx
 */

#ifndef _SENSORS_H_
#define _SENSORS_H_

enum {
	TH_SENSOR_NONE = 0,
	TH_SENSOR_SHTC3,   // 1
	TH_SENSOR_SHT4x,   // 2
	TH_SENSOR_SHT30,	// 3 CHT8320
	TH_SENSOR_CHT8305,	// 4
	TH_SENSOR_AHT2x,	// 5
	TH_SENSOR_CHT8215,	// 6
	IU_SENSOR_INA226,	// 7
	IU_SENSOR_MY18B20,	// 8
	IU_SENSOR_MY18B20x2,	// 9
	IU_SENSOR_HX71X,	// 10
	IU_SENSOR_PWMRH,	// 11
	IU_SENSOR_NTC,		// 12
	TH_SENSOR_TYPE_MAX // 13
} SENSOR_TYPES_e;

#define AHT2x_I2C_ADDR			0x38
#define CHT8305_I2C_ADDR		0x40
#define CHT8305_I2C_ADDR_MAX	0x43
#define SHT30_I2C_ADDR			0x44 // 0x44, 0x45
#define SHT30_I2C_ADDR_MAX		0x45
#define SHT4x_I2C_ADDR			0x44 // 0x44, 0x45, 0x46
#define SHT4x_I2C_ADDR_MAX		0x46
#define SHTC3_I2C_ADDR			0x70

#ifndef USE_SENSOR_CHT8305
#define USE_SENSOR_CHT8305 	0
#endif
#ifndef USE_SENSOR_CHT8215
#define USE_SENSOR_CHT8215	0
#endif
#ifndef USE_SENSOR_AHT20_30
#define USE_SENSOR_AHT20_30	0
#endif
#ifndef USE_SENSOR_SHT4X
#define USE_SENSOR_SHT4X 	0
#endif
#ifndef USE_SENSOR_SHTC3
#define USE_SENSOR_SHTC3 	0
#endif
#ifndef USE_SENSOR_SHT30
#define USE_SENSOR_SHT30 	0
#endif

#if USE_SENSOR_TH

typedef int (*sensors_callback_t)(void *);

typedef struct _thsensor_coef_t {
	u32 val1_k;	// temp_k / current_k
	u32 val2_k;	// humi_k / voltage_k
	s16 val1_z;	// temp_z / current_z
	s16 val2_z;	// humi_z / voltage_z
} sensor_coef_t; // [12]

enum {
	MMODE_START_WAIT_READ	=0,
	MMODE_READ_START,
	MMODE_READ_ONLY
} MEASUREMENT_MODE_e;

typedef struct _sensor_def_cfg_t {
	sensor_coef_t coef;
	sensors_callback_t start_measure;
	sensors_callback_t read_measure;
	u32 measure_timeout;
	u8 sensor_type; // SENSOR_TYPES
	u8	mode; // =0 - start-wait-read, =1 - read-start, =2 read only
} sensor_def_cfg_t;

typedef struct _sensor_th_t {
	s16	temp; // in 0.01 C
	s16	humi; // in 0.01 %
	sensor_coef_t coef;
	sensors_callback_t start_measure;
	sensors_callback_t read_measure;
	u32	id;
	u32	measure_timeout;
	u8	i2c_addr;
	u8	sensor_type; // SENSOR_TYPES
	u8	mode; // =0 - start-wait-read, =1 - read-start, =2 read only (auto step measure)
} sensor_th_t;

extern sensor_th_t sensor_ht;


void init_sensor(void);

#if USE_SENSOR_SHTC3
void sensor_go_sleep(void);
#else
#define sensor_go_sleep()
#endif

int read_sensor(void);

#endif

#endif /* _SENSORS_H_ */
