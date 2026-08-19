/*
 * zcl_dehumidification_control.h
 *
 *  Cluster: ZCL_CLUSTER_HAVC_DIHUMIDIFICATION_CONTROL
 *
 *  Created on: 17 авг. 2026 г.
 *      Author: pvvx
 */

#ifndef _ZCL_ZCL_DEHUMIDIFICATION_CONTROL_H_
#define _ZCL_ZCL_DEHUMIDIFICATION_CONTROL_H_

#define ZCL_ATTRID_DHUM_RELATIVE_HUMIDITY	0x0000 // uint8, 0x00 – 0x64, R, O
#define ZCL_ATTRID_DHUM_COOLING				0x0001 // uint8, 0 - DehumidificationMaxCool, RP, M
#define ZCL_ATTRID_DHUM_RH_SETPOINT			0x0010 // uint8, 0x1E – 0x64, RW, def: 0x32, M
#define ZCL_ATTRID_DHUM_RH_MODE				0x0011 // enum8, 0x00 – 0x01, RW, def: 0x00, O
#define ZCL_ATTRID_DHUM_LOCKOUT				0x0012 // enum8, 0x00 – 0x01, RW, def: 0x01, O
#define ZCL_ATTRID_DHUM_HYSTERESIS			0x0013 // uint8, 0x02 – 0x14, RW, def: 0x02, M
#define ZCL_ATTRID_DHUM_MAXCOOL				0x0014 // uint8, 0x14 – 0x64, RW, def: 0x14, M
#define ZCL_ATTRID_DHUM_DISPLAY				0x0015 // enum8, 0x00 – 0x01, RW, def: 0x00, O

// RHDehumidificationSetpoint attribute
#define DHUM_SetpointMax  100	// %
#define DHUM_SetpointMin  30	// %

// DehumidificationHysteresis attribute
#define DHUM_HysteresisMax  20	// %
#define DHUM_HysteresisMin  2	// %

//DehumidificationMaxCool attribute
#define DHUM_MaxCoolMax  	100	// %
#define DHUM_MaxCoolMin  	20	// %

// RelativeHumidityMode Attribute Values
#define DHUM_RH_ModeLocale	0
#define DHUM_RH_ModeNetwork	1

// DehumidificationLockout Attribute Values
#define DHUM_LockoutNotAllowed	0
#define DHUM_LockoutAllowed		1

// RelativeHumidityDisplay attribute
#define DHUM_NotDisplayed	0
#define DHUM_Displayed		1

status_t zcl_dehumidification_control_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb);

#endif /* _ZCL_ZCL_DEHUMIDIFICATION_CONTROL_H_ */
