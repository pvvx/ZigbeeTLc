/********************************************************************************************************
 * @file    zcl_illuminance_level_sensing.c
 */

/**********************************************************************
 * INCLUDES
 */
#include "zcl_include.h"

#ifdef ZCL_ILLUMINANCE_LEVEL_SENSING
/**********************************************************************
 * LOCAL CONSTANTS
 */

/**********************************************************************
 * LOCAL TYPES
 */

/**********************************************************************
 * LOCAL VARIABLES
 */

/**********************************************************************
 * LOCAL FUNCTIONS
 */
_CODE_ZCL_ status_t zcl_illuminanceLevelSensing_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb)
{
	return zcl_registerCluster(endpoint, ZCL_CLUSTER_MS_ILLUMINANCE_LEVEL_SENSING_CONFIG, manuCode, attrNum, attrTbl, NULL, cb);
}
#endif  /* ZCL_ILLUMINANCE_LEVEL_SENSING */
