/*
 * sensor.h
 *
 *  Created on: 14 нояб. 2023 г.
 *      Author: pvvx
 */

#ifndef SRC_SENSORS_H_
#define SRC_SENSORS_H_

#if defined(SENSOR_TYPE)
#if SENSOR_TYPE == SENSOR_SHTXX
#include "sensors_shtxx.h"
#elif SENSOR_TYPE == SENSOR_CHT8305
#include "sensor_cht8305.h"
#else
#error "Define USE_SENSOR!"
#endif
#endif

#endif /* SRC_SENSORS_H_ */
