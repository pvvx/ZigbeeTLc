/*
 * sensor_lx.h
 *
 *  Created on: 3 апр. 2026 г.
 *      Author: pvvx
 */

#ifndef _SENSOR_LX_H_
#define _SENSOR_LX_H_


#define ZCL_CUSTOM_ATTRID_MEASURE_INTERVAL	0x5000
#define ZCL_CUSTOM_ATTRID_LX_SENSOR_ZERO	0x5001
#define ZCL_CUSTOM_ATTRID_LX_SENSOR_COEF	0x5002

#define ADC_LX_ZERO_DEF	0
#define ADC_LX_COEF_DEF	30100  // = max 1000 lx // 10000*log10(30000)

/**
 *  @brief Defined for Illuminance Measure cluster attributes
 */

typedef struct {
	u16 k; // coef
	u16 z; // zero
	u8 measureInterval;
} ilumi_cfg_t;

typedef struct {
	ilumi_cfg_t cfg;
	u16 measuredVal;
	u16 minMeasuredVal;
	u16 maxMeasuredVal;
#ifdef ZCL_ATTR_TOLERANCE_ENABLE
	u16 tolerance;
#endif
#ifdef ZCL_ATTR_LIGHT_SENSOR_TYPE_ENABLE
	u8 lightSensorType;
#endif
#ifdef ZCL_ILLUMINANCE_LEVEL_SENSING
	u16 minLevelLx;
	u8 levelStatus;
#endif
} zcl_illuminanceAttr_t;

extern zcl_illuminanceAttr_t g_zcl_illuminanceAttrs;


void init_sensor(void);

int read_illumi_sensor(void);

void zcl_illuminanceLevel_save(void);
void zcl_illuminanceConfig_save(void);
void zcl_occupanceConfig_save(void);

#endif /* _SENSOR_LX_H_ */
