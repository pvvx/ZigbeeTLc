/********************************************************************************************************
 * @file    zcl_illuminance_level_sensing.h
 */
#ifndef ZCL_ILLUMINANCE_LEVEL_SENSING_H
#define ZCL_ILLUMINANCE_LEVEL_SENSING_H

/*********************************************************************
 * CONSTANTS
 */

/* Attribute ID */
#define ZCL_ATTRID_ILSC_LEVEL_STATUS        0x0000
#define ZCL_ATTRID_ILSC_LIGHT_SENSOR_TYPE   0x0001
#define ZCL_ATTRID_ILSC_TARGET_LEVEL        0x0010

typedef enum {
	ILSC_ON_TARGET = 0, // Illuminance on target
	ILSC_BELOW_TARGET,	// Illuminance below target
	ILSC_ABOVE_TARGET,	// Illuminance above target
	ILSC_NONE = 0xff	//
} ilsc_level_status_e;

status_t zcl_illuminanceLevelSensing_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb);

#endif	/* ZCL_ILLUMINANCE_LEVEL_SENSING_H */
