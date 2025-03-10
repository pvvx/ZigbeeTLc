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

#if USE_SENSOR_TH

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


typedef int (*sensors_stage_t)(void *);
typedef void (*sensors_callback_t)(void);

typedef struct {
	u32 val1_k;	// temp_k / current_k
	u32 val2_k;	// humi_k / voltage_k
	s16 val1_z;	// temp_z / current_z
	s16 val2_z;	// humi_z / voltage_z
} sensor_coef_t; // [12]

enum {
	MMODE_START_WAIT_READ	=0,	// wakeup, start, wait + (read [+sleep])
	MMODE_READ_START,			// read, next start, none
	MMODE_READ_ONLY				// read, none, none
} MEASUREMENT_MODE_e;

typedef struct {
	sensor_coef_t coef;
//	MEASUREMENT_MODE: 0,1,2
	sensors_stage_t stage1_measure; // wakeup, read, read
	sensors_stage_t stage2_measure; // start, next start, none
	sensors_callback_t measure_callback; // wait + (read [+sleep]), none, none
	u32 measure_timeout_us;
	u8 sensor_type; // SENSOR_TYPES
	u8	mode; // =0 - start-wait-read, =1 - read-start, =2 read only
} sensor_def_cfg_t;

typedef struct {
	s16	temp; // in 0.01 C
	s16	humi; // in 0.01 %
	sensor_coef_t coef;
	sensors_stage_t stage1_measure;
	sensors_stage_t stage2_measure;
	sensors_callback_t measure_callback;
	sensors_callback_t read_callback;
	u32	id;
	u32	measure_timeout_us;
	u32	start_measure_tik;
	u8	i2c_addr;
	u8	sensor_type; // SENSOR_TYPES
	u8	mode; // =0 - start-wait-read, =1 - read-start, =2 read only (auto step measure)
	u8  flag; // =0xff if read data ok, = 0 if not check_sensor
} sensor_th_t;

#define FLG_MEASURE_HT_ADV		0x01
#define FLG_MEASURE_HT_LCD		0x02
#define FLG_MEASURE_HT_RP		0x04
#define FLG_MEASURE_HT_CC		0x08


extern sensor_th_t sensor_ht;


void init_sensor(void);

#if USE_SENSOR_SHTC3
void sensor_go_sleep(void);
#else
#define sensor_go_sleep()
#endif

int read_sensor(void);

#endif // USE_SENSOR_TH

#endif /* _SENSORS_H_ */
