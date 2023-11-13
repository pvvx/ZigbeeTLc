#include "reporting.h"
#include "utility.h"

extern void reportAttr(reportCfgInfo_t *pEntry);

app_reporting_t app_reporting[ZCL_REPORTING_TABLE_NUM];

static s32 app_reportMinAttrTimerCb(void *arg) {
    app_reporting_t *app_reporting = (app_reporting_t*)arg;
    reportCfgInfo_t *pEntry = app_reporting->pEntry;

    zclAttrInfo_t *pAttrEntry = zcl_findAttribute(pEntry->endPoint, pEntry->clusterID, pEntry->attrID);
    if(!pAttrEntry){
        //should not happen.
        ZB_EXCEPTION_POST(SYS_EXCEPTTION_ZB_ZCL_ENTRY);
        app_reporting->timerReportMinEvt = NULL;
        return -1;
    }

    if (pEntry->minInterval == pEntry->maxInterval) {
        reportAttr(pEntry);
        app_reporting->time_posted = clock_time();
    } else {
        if(zcl_analogDataType(pAttrEntry->type)) {
        	if(reportableChangeValueChk(pAttrEntry->type,
                pAttrEntry->data, pEntry->prevData, pEntry->reportableChange)) {
                reportAttr(pEntry);
                app_reporting->time_posted = clock_time();
        	}
        } else {
            u8 len = zcl_getAttrSize(pAttrEntry->type, pAttrEntry->data);
            len = (len>8) ? (8):(len);
        	if(memcmp(pEntry->prevData, pAttrEntry->data, len) != SUCCESS) {
                reportAttr(pEntry);
                app_reporting->time_posted = clock_time();
        	}
        }
    }
    return 0;
}

static s32 app_reportMaxAttrTimerCb(void *arg) {
    app_reporting_t *app_reporting = (app_reporting_t*)arg;
    reportCfgInfo_t *pEntry = app_reporting->pEntry;

    if (clock_time_exceed(app_reporting->time_posted, pEntry->minInterval*1000*1000)) {
        if (app_reporting->timerReportMinEvt) {
            TL_ZB_TIMER_CANCEL(&app_reporting->timerReportMinEvt);
        }
        reportAttr(pEntry);
    }

    return 0;
}

void app_reportAttrTimerStart() {
    if(zcl_reportingEntryActiveNumGet()) {
        for(u8 i = 0; i < ZCL_REPORTING_TABLE_NUM; i++) {
            reportCfgInfo_t *pEntry = &reportingTab.reportCfgInfo[i];
            app_reporting[i].pEntry = pEntry;
            if(pEntry->used && (pEntry->maxInterval != 0xFFFF) && (pEntry->minInterval || pEntry->maxInterval)){
                if(zb_bindingTblSearched(pEntry->clusterID, pEntry->endPoint)) {
                    if (!app_reporting[i].timerReportMinEvt) {
                        if (pEntry->minInterval && pEntry->maxInterval && pEntry->minInterval <= pEntry->maxInterval) {
                            app_reporting[i].timerReportMinEvt = TL_ZB_TIMER_SCHEDULE(app_reportMinAttrTimerCb, &app_reporting[i], pEntry->minInterval*1000);
                        }
                    }
                    if (!app_reporting[i].timerReportMaxEvt) {
                        if (pEntry->maxInterval) {
                            if (pEntry->minInterval < pEntry->maxInterval) {
                                if (pEntry->maxInterval != pEntry->minInterval && pEntry->maxInterval > pEntry->minInterval) {
                                    app_reporting[i].timerReportMaxEvt = TL_ZB_TIMER_SCHEDULE(app_reportMaxAttrTimerCb, &app_reporting[i], pEntry->maxInterval*1000);
                                }
                            }
                        } else {
                            app_reportMinAttrTimerCb(&app_reporting[i]);
                        }

                    }
                }
            }
        }
    }
}
