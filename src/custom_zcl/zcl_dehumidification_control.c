/*
 * zcl_dehumidification_control.c
 *
 *  Cluster: ZCL_CLUSTER_HAVC_DIHUMIDIFICATION_CONTROL
 *
 *  Created on: 17 авг. 2026 г.
 *      Author: pvvx
 */
#include "zcl_include.h"

#ifdef ZCL_DIHUMIDIFICATION_CONTROL

_CODE_ZCL_ status_t zcl_dehumidification_control_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb)
{
	return zcl_registerCluster(endpoint, ZCL_CLUSTER_HAVC_DIHUMIDIFICATION_CONTROL, manuCode, attrNum, attrTbl, NULL, cb);
}

#endif /* ZCL_DIHUMIDIFICATION_CONTROL */
