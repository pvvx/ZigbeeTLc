/*
 * sensor.h
 *
 *  Created on: 14 нояб. 2023 г.
 *      Author: pvvx
 */

#ifndef _SENSORS_H_
#define _SENSORS_H_

#if defined(SENSOR_TYPE)
#include "sensors_shtc3_4x.h"
#include "sensors_sht30.h"
#include "sensor_cht8305.h"
#include "sensor_aht2x_3x.h"
#else
//#error "Define USE_SENSOR!"
#endif

#if USE_SENSOR_ID
extern u32 sensor_id;
#endif

void adc_channel_init(ADC_InputPchTypeDef p_ain); // in adc_drv.c
u16 get_adc_mv(void); // in adc_drv.c
void battery_detect(void);

#endif /* _SENSORS_H_ */
