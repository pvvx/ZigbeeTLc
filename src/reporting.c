/*
 * reporting.c
 * Created: pvvx
 */
#include "reporting.h"
#include "utility.h"

extern void reportAttr(reportCfgInfo_t *pEntry);

/*********************************************************************
 * @fn      app_chk_report
 *
 * @brief	check if there is report.
 *
 * @param   time from old check in sec
 *
 * @return	NULL
 */
void app_chk_report(u16 uptime_sec) {
	zclAttrInfo_t *pAttrEntry = NULL;
	u16 len;
	bool flg_report, flg_chk_attr;
	if(reportingTab.reportNum) {
		for(u8 i = 0; i < ZCL_REPORTING_TABLE_NUM; i++){
			reportCfgInfo_t *pEntry = &reportingTab.reportCfgInfo[i];
/**
 *  @brief  If minInterval is 0, then there is no minimum limit;
 *  		if maxInterval is 0xffff, then the configuration info for that attribute need not be maintained;
 *  		if minInterval is 0xffff and maxInterval is 0, than back to default reporting configuration, reportable change field set to 0.
 */
			if(pEntry->used && (pEntry->maxInterval != 0xFFFF)) {
				// used
				flg_chk_attr = false;
				flg_report = false;
				if(pEntry->minInterval == 0) {
					// there is no minimum limit
					flg_chk_attr = true;
				} else if (pEntry->minInterval || pEntry->maxInterval) {
					// timer minInterval seconds
					if(pEntry->minIntCnt > uptime_sec)
						pEntry->minIntCnt -= uptime_sec;
					else
						pEntry->minIntCnt = 0;
					// timer maxInterval seconds
					if(pEntry->maxIntCnt > uptime_sec)
						pEntry->maxIntCnt -= uptime_sec;
					else
						pEntry->maxIntCnt = 0;

					if(pEntry->maxIntCnt == 0) {
						flg_report = true;
					} else if(pEntry->minIntCnt == 0) {
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
						len = (len > 8) ? (8): (len);
						if(memcmp(pEntry->prevData, pAttrEntry->data, len) != SUCCESS) {
							flg_report = true;
						}
					}
				}
				if(flg_report) {
					pEntry->minIntCnt = pEntry->minInterval;
					pEntry->maxIntCnt = pEntry->maxInterval;
					reportAttr(pEntry);
				}
			}
		}
	} else {
		// no report
	}
}
/*********************************************************************
 * @fn      app_set_thb_report
 *
 * @brief	set temp, humi, bat is report.
 *
 * @param   NULL
 *
 * @return	NULL
 */
void app_set_thb_report(void) {
	if(reportingTab.reportNum) {
		for(u8 i = 0; i < ZCL_REPORTING_TABLE_NUM; i++){
			reportCfgInfo_t *pEntry = &reportingTab.reportCfgInfo[i];
			if(pEntry->used && (
					pEntry->clusterID == ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT
					|| pEntry->clusterID == ZCL_CLUSTER_MS_RELATIVE_HUMIDITY
					|| pEntry->clusterID == ZCL_CLUSTER_GEN_POWER_CFG)) {
				pEntry->maxIntCnt = 0;
			}
		}
	}
}
