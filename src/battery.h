/*
 * battery.h
 *
 *  Created on: 24 янв. 2025 г.
 *      Author: pvvx
 */

#ifndef _BATTERY_H_
#define _BATTERY_H_

#define BATTERY_LOW_POWER			2000 //2.0v
#define BATTERY_SAFETY_THRESHOLD	2200 //2.2v
#define LOW_POWER_SLEEP_TIME_ms		180*1000 // 180 sec

// measured_battery.flag:
#define FLG_MEASURE_BAT_ADV		0x01
#define FLG_MEASURE_BAT_CC		0x02

typedef struct _measured_battery_t {
	u32 summ;		// сумматор
	u16	mv; 		// mV
	u16	average_mv; // mV
	u16 cnt;
	u8	level; 		// in 0.5% 0..200
#if USE_BLE
	u8  batVal; 	// 0..100%
#endif
	u8  flag;
} measured_battery_t;

extern measured_battery_t measured_battery;
extern u32 adc_average; // = ADC value * 4, set get_adc_mv()

#ifndef SHL_ADC_VBAT
#define SHL_ADC_VBAT		C5P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PC5 // missing pin on case TLSR825xF512ET24
#define PC5_INPUT_ENABLE	0
#define PC5_DATA_OUT		1
#define PC5_OUTPUT_ENABLE	1
#define PC5_FUNC			AS_GPIO
#endif

void adc_channel_init(ADC_InputPchTypeDef p_ain); // in adc_drv.c
u16 get_adc_mv(int flg); // in adc_drv.c

void battery_detect(bool startup_flg);

#endif /* _BATTERY_H_ */
