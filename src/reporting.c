#include "reporting.h"
#include "utility.h"

extern void reportAttr(reportCfgInfo_t *pEntry);

/*********************************************************************
 * @fn      app_chk_report
 *
 * @brief	check if there is report.
 *
 * @param   NULL
 *
 * @return	NULL
 */
void app_chk_report(u16 uptime_sec) {
	if(zcl_reportingEntryActiveNumGet()){
		zclAttrInfo_t *pAttrEntry = NULL;
		u16 len = 0;
		bool flg_report = false;
		bool flg_chk_attr = false;
		for(u8 i = 0; i < ZCL_REPORTING_TABLE_NUM; i++){
			reportCfgInfo_t *pEntry = &reportingTab.reportCfgInfo[i];
			if(pEntry->used && (pEntry->maxInterval != 0xFFFF)) {
				flg_report = false;
				// used
				if(pEntry->minInterval == 0) {
					// there is no minimum limit
					flg_chk_attr = true;
				} else if (pEntry->minInterval || pEntry->maxInterval) {
					if(pEntry->minIntCnt > uptime_sec)
						pEntry->minIntCnt -= uptime_sec;
					else
						pEntry->minIntCnt = 0;
					if(pEntry->maxIntCnt > uptime_sec)
						pEntry->maxIntCnt -= uptime_sec;
					else
						pEntry->maxIntCnt = 0;
					if(!pEntry->maxIntCnt) {
						flg_report = true;
					} else if(!pEntry->minIntCnt) {
						flg_chk_attr = true;
					}
 				}
				if(flg_chk_attr || flg_report) {
					pAttrEntry = zcl_findAttribute(pEntry->endPoint, pEntry->clusterID, pEntry->attrID);
					if(!pAttrEntry){
						// should not happen.
						ZB_EXCEPTION_POST(SYS_EXCEPTTION_ZB_ZCL_ENTRY);
						return;
					}
				}
				if(flg_chk_attr) {
					// report pAttrEntry
					if(zcl_analogDataType(pAttrEntry->type)) {
						if(reportableChangeValueChk(pAttrEntry->type, pAttrEntry->data, pEntry->prevData, pEntry->reportableChange)) {
							flg_report = true;
						}
					} else {
						len = zcl_getAttrSize(pAttrEntry->type, pAttrEntry->data);
						len = (len>8) ? (8):(len);
						if(memcmp(pEntry->prevData, pAttrEntry->data, len) != SUCCESS) {
							flg_report = true;
						}
					}
				}
				if(flg_report) {
					reportAttr(pEntry);
					pEntry->minIntCnt = pEntry->minInterval;
					pEntry->maxIntCnt = pEntry->maxInterval;
				}
			}
		}
	} else {
		// no report
	}
}
