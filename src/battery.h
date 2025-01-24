/*
 * battery.h
 *
 *  Created on: 24 янв. 2025 г.
 *      Author: pvvx
 */

#ifndef _BATTERY_H_
#define _BATTERY_H_

typedef struct _measured_battery_t {
	u16	mv; // mV
	u16	average_mv; // mV
	u16	level; // in 0.5%
} measured_battery_t;

extern measured_battery_t measured_battery;

void adc_channel_init(ADC_InputPchTypeDef p_ain); // in adc_drv.c
u16 get_adc_mv(void); // in adc_drv.c

void battery_detect(void);

#endif /* _BATTERY_H_ */
