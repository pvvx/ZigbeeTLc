/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zcl_include.h"
#include "zcl_thermostat_ui_cfg.h"
#include "app_main.h"
#include "app_ui.h"
#include "reporting.h"
#ifdef ZCL_ON_OFF
#include "app_onoff.h"
#endif
#if USE_BLE
#include "ble_cfg.h"
#include "zigbee_ble_switch.h"
#include "stack/ble/ble.h"
#include "ble_cfg.h"
#endif
#if USE_SENSOR_LX
#include "zcl_illuminance_level_sensing.h"
#include "sensor_lx.h"
#endif
#if (DEV_SERVICES & SERVICE_PIR)
#include "sensor_pir.h"
#endif

/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * LOCAL FUNCTIONS
 */
#ifdef ZCL_READ
void sensorDevice_zclReadRspCmd(u16 clusterId, zclReadRspCmd_t *pReadRspCmd);
#endif
#ifdef ZCL_WRITE
void sensorDevice_zclWriteRspCmd(u16 clusterId, zclWriteRspCmd_t *pWriteRspCmd);
void sensorDevice_zclWriteReqCmd(u16 clusterId, zclWriteCmd_t *pWriteReqCmd);
#endif
#ifdef ZCL_REPORT
void sensorDevice_zclCfgReportCmd(u8 endpoint, u16 clusterId, zclCfgReportCmd_t *pCfgReportCmd);
void sensorDevice_zclCfgReportRspCmd(u16 clusterId, zclCfgReportRspCmd_t *pCfgReportRspCmd);
void sensorDevice_zclReportCmd(u16 clusterId, zclReportCmd_t *pReportCmd);
#endif
void sensorDevice_zclDfltRspCmd(u16 clusterId, zclDefaultRspCmd_t *pDftRspCmd);

/**********************************************************************
 * GLOBAL VARIABLES
 */

/**********************************************************************
 * LOCAL VARIABLES
 */

/**********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      sensorDevice_zclProcessIncomingMsg
 *
 * @brief   Process ZCL Foundation incoming message.
 *
 * @param   pInMsg - pointer to the received message
 *
 * @return  None
 */
void sensorDevice_zclProcessIncomingMsg(zclIncoming_t *pInHdlrMsg)
{
	u16 cluster = pInHdlrMsg->msg->indInfo.cluster_id;
	switch(pInHdlrMsg->hdr.cmd)
	{
#ifdef ZCL_READ
//		case ZCL_CMD_READ_RSP:
//			sensorDevice_zclReadRspCmd(cluster, pInHdlrMsg->attrCmd);
//			break;
#endif
#ifdef ZCL_WRITE
//		case ZCL_CMD_WRITE_RSP:
//			sensorDevice_zclWriteRspCmd(cluster, pInHdlrMsg->attrCmd);
//			break;
		case ZCL_CMD_WRITE:
			sensorDevice_zclWriteReqCmd(cluster, pInHdlrMsg->attrCmd);
			break;
#endif
#ifdef ZCL_REPORT
		case ZCL_CMD_CONFIG_REPORT:
			sensorDevice_zclCfgReportCmd(pInHdlrMsg->msg->indInfo.dst_ep, cluster, pInHdlrMsg->attrCmd);
			break;
//		case ZCL_CMD_CONFIG_REPORT_RSP:
//			sensorDevice_zclCfgReportRspCmd(cluster, pInHdlrMsg->attrCmd);
//			break;
//		case ZCL_CMD_REPORT:
//			sensorDevice_zclReportCmd(cluster, pInHdlrMsg->attrCmd);
//			break;
#endif
//		case ZCL_CMD_DEFAULT_RSP:
//			sensorDevice_zclDfltRspCmd(cluster, pInHdlrMsg->attrCmd);
//			break;
		default:
			break;
	}
}

#ifdef ZCL_READ
/*********************************************************************
 * @fn      sensorDevice_zclReadRspCmd
 *
 * @brief   Handler for ZCL Read Response command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
void sensorDevice_zclReadRspCmd(u16 clusterId, zclReadRspCmd_t *pReadRspCmd)
{
#if USE_PRINTF
	u8 numAttr = pReadRspCmd->numAttr;
	zclReadRspStatus_t *attr = pReadRspCmd->attrList;
	for(int i = 0; i < numAttr; i++) {
		sws_printf("zclReadRsp: %04x:%04x %02x..\n", clusterId, attr[i].attrID, attr[i].data[0]);
	}
#endif
}
#endif	/* ZCL_READ */

#ifdef ZCL_WRITE
/*********************************************************************
 * @fn      sensorDevice_zclWriteRspCmd
 *
 * @brief   Handler for ZCL Write Response command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
void sensorDevice_zclWriteRspCmd(u16 clusterId, zclWriteRspCmd_t *pWriteRspCmd)
{
#if USE_PRINTF
	u8 numAttr = pWriteRspCmd->numAttr;
	zclWriteRspStatus_t *attr = pWriteRspCmd->attrList;
	for(int i = 0; i < numAttr; i++) {
		sws_printf("zclWriteRsp: %04x:%04x %02x\n", clusterId, attr[i].attrID, attr[i].status);
	}
#endif
}

/*********************************************************************
 * @fn      sensorDevice_zclWriteReqCmd
 *
 * @brief   Handler for ZCL Write Request command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
void sensorDevice_zclWriteReqCmd(u16 clusterId, zclWriteCmd_t *pWriteReqCmd)
{
	u8 numAttr = pWriteReqCmd->numAttr;
	zclWriteRec_t *attr = pWriteReqCmd->attrList;
#if USE_PRINTF
	for(int i = 0; i < numAttr; i++) {
		sws_printf("zclWriteReq: %04x:%04x %02x..\n", clusterId, attr[i].attrID, attr[i].attrData[0]);
	}
#endif
#ifdef ZCL_POLL_CTRL
	 if(clusterId == ZCL_CLUSTER_GEN_POLL_CONTROL){
		for(int i = 0; i < numAttr; i++) {
			if(attr[i].attrID == ZCL_ATTRID_CHK_IN_INTERVAL) {
				sensorDevice_zclCheckInStart();
#if ZCL_THERMOSTAT_UI_CFG || defined(ZCL_ILLUMINANCE_LEVEL_SENSING)
			} else if(attr[i].attrID == ZCL_ATTRID_LONG_POLL_INTERVAL) {
				if(test_set_measure_longpoll_interval(g_zcl_pollCtrlAttrs.longPollInterval >> 2)
						== ZCL_STA_SUCCESS) {
#if ZCL_THERMOSTAT_UI_CFG
					zcl_thermostatConfig_save();
#else
#ifdef ZCL_ILLUMINANCE_LEVEL_SENSING
					zcl_illuminanceConfig_save();
#endif
#endif
				}
#endif
			}
		}
	} else
#endif // ZCL_POLL_CTRL
#ifdef ZCL_ON_OFF
	if(clusterId == ZCL_CLUSTER_GEN_ON_OFF) {
		zcl_onoffConfig_save();
	} else
#endif
#ifdef ZCL_ILLUMINANCE_LEVEL_SENSING
	if(clusterId == ZCL_CLUSTER_MS_ILLUMINANCE_LEVEL_SENSING_CONFIG) {
		for(int i = 0; i < numAttr; i++) {
			if(attr[i].attrID == ZCL_ATTRID_ILSC_TARGET_LEVEL) {
				zcl_illuminanceLevel_save();
			} else if(attr[i].attrID >= ZCL_CUSTOM_ATTRID_MEASURE_INTERVAL
					&& attr[i].attrID <= ZCL_CUSTOM_ATTRID_LX_SENSOR_COEF) {
				zcl_illuminanceConfig_save();
			}
		}
	} else
#endif
#ifdef ZCL_THERMOSTAT_UI_CFG
	if(clusterId == ZCL_CLUSTER_HAVC_USER_INTERFACE_CONFIG) {
		zcl_thermostatConfig_save();
	} else
#endif // ZCL_THERMOSTAT_UI_CFG
#ifdef ZCL_ILLUMINANCE_LEVEL_SENSING
	if(clusterId == ZCL_CLUSTER_MS_OCCUPANCY_SENSING) {
		zcl_occupanceConfig_save();
	} else
#endif
#if USE_BLE
	 if(clusterId == ZCL_CLUSTER_GEN_BASIC) {
		for(int i = 0; i < numAttr; i++) {
			if(attr[i].attrID == ZCL_ATTRID_BASIC_DEV_ENABLED && attr[i].dataType == ZCL_DATA_TYPE_BOOLEAN) {
				if(attr[i].attrData[0]) { // or if(g_zcl_basicAttrs.deviceEnable) ?
					ble_task_stop();	// отключение BLE
				} else {
					g_dualModeInfo.bleStart = CONNECT_ADV_COUNT;
				}
			}
#if USE_CHG_NAME
			else if(attr[i].dataType == ZCL_DATA_TYPE_CHAR_STR
				&& (attr[i].attrID == ZCL_ATTRID_BASIC_MFR_NAME || attr[i].attrID == ZCL_ATTRID_BASIC_MODEL_ID)) {
				save_dev_name(attr[i].attrID);
			}
#endif // USE_CHG_NAME
		}
	} else
#endif
	{}
}
#endif	/* ZCL_WRITE */


/*********************************************************************
 * @fn      sensorDevice_zclDfltRspCmd
 *
 * @brief   Handler for ZCL Default Response command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
void sensorDevice_zclDfltRspCmd(u16 clusterId, zclDefaultRspCmd_t *pDftRspCmd)
{
	sws_printf("zclDfltRsp: %04x %02x %02x..\n", clusterId, pDftRspCmd->commandID, pDftRspCmd->statusCode);
}

#ifdef ZCL_REPORT
/*********************************************************************
 * @fn      sensorDevice_zclCfgReportCmd
 *
 * @brief   Handler for ZCL Configure Report command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
void sensorDevice_zclCfgReportCmd(u8 endpoint, u16 clusterId, zclCfgReportCmd_t *pCfgReportCmd)
{
	for(u8 i = 0; i < ZCL_REPORTING_TABLE_NUM; i++){
		reportCfgInfo_t *pEntry = &reportingTab.reportCfgInfo[i];
		if(pEntry->used && pEntry->clusterID == clusterId && pEntry->endPoint == endpoint) {
			sws_printf("zclCfgReport: %04x:%04x %02x %d,%d,%d\n",
					clusterId, pEntry->attrID, endpoint,
					pEntry->minInterval, pEntry->maxInterval, pEntry->reportableChange[0]);
			pEntry->minIntCnt = 0;
			pEntry->maxIntCnt = 0;
		}
	}
}
/*********************************************************************
 * @fn      sensorDevice_zclCfgReportRspCmd
 *
 * @brief   Handler for ZCL Configure Report Response command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
void sensorDevice_zclCfgReportRspCmd(u16 clusterId, zclCfgReportRspCmd_t *pCfgReportRspCmd)
{
#if USE_PRINTF
	u8 numAttr = pCfgReportRspCmd->numAttr;
	zclCfgReportStatus_t *attr = pCfgReportRspCmd->attrList;
	for(int i = 0; i < numAttr; i++) {
		sws_printf("zclCfgReportRsp: %04x:%04x %02x\n", clusterId, attr[i].attrID, attr[i].status);
	}
#endif
}

/*********************************************************************
 * @fn      sensorDevice_zclReportCmd
 *
 * @brief   Handler for ZCL Report command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
void sensorDevice_zclReportCmd(u16 clusterId, zclReportCmd_t *pReportCmd)
{
#if USE_PRINTF
	u8 numAttr = pReportCmd->numAttr;
	zclReport_t *attr = pReportCmd->attrList;
	for(int i = 0; i < numAttr; i++) {
		sws_printf("zclCfgReportRsp: %04x:%04x %02x..\n", clusterId, attr[i].attrID, attr[i].attrData[0]);
	}
#endif
}
#endif	/* ZCL_REPORT */

#ifdef ZCL_BASIC
/*********************************************************************
 * @fn      sensorDevice_basicCb
 *
 * @brief   Handler for ZCL Basic Reset command.
 *
 * @param   pAddrInfo
 * @param   cmdId
 * @param   cmdPayload
 *
 * @return  status_t
 */
status_t sensorDevice_basicCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload)
{
	sws_printf("basicCb: %02x\n", cmdId);
	if(cmdId == ZCL_CMD_BASIC_RESET_FAC_DEFAULT){
		//Reset all the attributes of all its clusters to factory defaults
		//zcl_nv_attr_reset();
	}

	return ZCL_STA_SUCCESS;
}
#endif	/* ZCL_BASIC */

#ifdef ZCL_IDENTIFY
s32 sensorDevice_zclIdentifyTimerCb(void *arg)
{
	if(g_zcl_identifyAttrs.identifyTime <= 0){
		g_sensorAppCtx.timerIdentifyEvt = NULL;
		return -1;
	}
	g_zcl_identifyAttrs.identifyTime--;
	return 0;
}

void sensorDevice_zclIdentifyTimerStop(void)
{
	if(g_sensorAppCtx.timerIdentifyEvt){
		TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerIdentifyEvt);
	}
}

/*********************************************************************
 * @fn      sensorDevice_zclIdentifyCmdHandler
 *
 * @brief   Handler for ZCL Identify command. This function will set blink LED.
 *
 * @param	endpoint
 * @param	srcAddr
 * @param   identifyTime - identify time
 *
 * @return  None
 */
void sensorDevice_zclIdentifyCmdHandler(u8 endpoint, u16 srcAddr, u16 identifyTime)
{
	g_zcl_identifyAttrs.identifyTime = identifyTime;

	if(identifyTime == 0){
		sensorDevice_zclIdentifyTimerStop();
		light_blink_stop();
	}else{
		if(!g_sensorAppCtx.timerIdentifyEvt){
			light_blink_start(identifyTime, 500, 500);
			g_sensorAppCtx.timerIdentifyEvt = TL_ZB_TIMER_SCHEDULE(sensorDevice_zclIdentifyTimerCb, NULL, 1000);
		}
	}
}

/*********************************************************************
 * @fn      sensorDevice_zcltriggerCmdHandler
 *
 * @brief   Handler for ZCL trigger command.
 *
 * @param   pTriggerEffect
 *
 * @return  None
 */
void sensorDevice_zcltriggerCmdHandler(zcl_triggerEffect_t *pTriggerEffect)
{
	u8 effectId = pTriggerEffect->effectId;
	//u8 effectVariant = pTriggerEffect->effectVariant;

	switch(effectId){
		case IDENTIFY_EFFECT_BLINK:
			light_blink_start(1, 500, 500);
			break;
		case IDENTIFY_EFFECT_BREATHE:
			light_blink_start(15, 300, 700);
			break;
		case IDENTIFY_EFFECT_OKAY:
			light_blink_start(2, 250, 250);
			break;
		case IDENTIFY_EFFECT_CHANNEL_CHANGE:
			light_blink_start(1, 500, 7500);
			break;
		case IDENTIFY_EFFECT_FINISH_EFFECT:
			light_blink_start(1, 300, 700);
			break;
		case IDENTIFY_EFFECT_STOP_EFFECT:
			light_blink_stop();
			break;
		default:
			break;
	}
}

/*********************************************************************
 * @fn      sensorDevice_zclIdentifyQueryRspCmdHandler
 *
 * @brief   Handler for ZCL Identify Query response command.
 *
 * @param   endpoint
 * @param   srcAddr
 * @param   identifyRsp
 *
 * @return  None
 */
void sensorDevice_zclIdentifyQueryRspCmdHandler(u8 endpoint, u16 srcAddr, zcl_identifyRspCmd_t *identifyRsp)
{
#if FIND_AND_BIND_SUPPORT
	if(identifyRsp->timeout){
		findBindDst_t dstInfo;
		dstInfo.addr = srcAddr;
		dstInfo.endpoint = endpoint;

		bdb_addIdentifyActiveEpForFB(dstInfo);
	}
#endif
}

/*********************************************************************
 * @fn      sensorDevice_identifyCb
 *
 * @brief   Handler for ZCL Identify command.
 *
 * @param   pAddrInfo
 * @param   cmdId
 * @param   cmdPayload
 *
 * @return  status_t
 */
status_t sensorDevice_identifyCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload)
{
	if(pAddrInfo->dstEp == SENSOR_DEVICE_ENDPOINT){
		if(pAddrInfo->dirCluster == ZCL_FRAME_CLIENT_SERVER_DIR){
			switch(cmdId){
				case ZCL_CMD_IDENTIFY:
					sensorDevice_zclIdentifyCmdHandler(pAddrInfo->dstEp, pAddrInfo->srcAddr, ((zcl_identifyCmd_t *)cmdPayload)->identifyTime);
					break;
				case ZCL_CMD_TRIGGER_EFFECT:
					sensorDevice_zcltriggerCmdHandler((zcl_triggerEffect_t *)cmdPayload);
					break;
				default:
					break;
			}
		}else{
			if(cmdId == ZCL_CMD_IDENTIFY_QUERY_RSP){
				sensorDevice_zclIdentifyQueryRspCmdHandler(pAddrInfo->dstEp, pAddrInfo->srcAddr, (zcl_identifyRspCmd_t *)cmdPayload);
			}
		}
	}

	return ZCL_STA_SUCCESS;
}
#endif	/* ZCL_IDENTIFY */


/*********************************************************************
 * @fn      sensorDevice_powerCfgCb
 *
 * @brief   Handler for ZCL Power Configuration command.
 *
 * @param   pAddrInfo
 * @param   cmdId
 * @param   cmdPayload
 *
 * @return  status_t
 */
status_t sensorDevice_powerCfgCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload)
{
//	if(cmdId == ZCL_CMD_BASIC_RESET_FAC_DEFAULT){
		//Reset all the attributes of all its clusters to factory defaults
		//zcl_nv_attr_reset();
//	}
	sws_printf("powerCfgCb: %02x:%02x:%02x %02x\n", pAddrInfo->profileId,
			pAddrInfo->srcAddr, pAddrInfo->dstAddr, cmdId);
	return ZCL_STA_SUCCESS;
}


#ifdef ZCL_IAS_ZONE
/*********************************************************************
 * @fn      sensorDevice_zclIasZoneEnrollRspCmdHandler
 *
 * @brief   Handler for ZCL IAS ZONE Enroll response command.
 *
 * @param   pZoneEnrollRsp
 *
 * @return  None
 */
static void sensorDevice_zclIasZoneEnrollRspCmdHandler(zoneEnrollRsp_t *pZoneEnrollRsp)
{

}

/*********************************************************************
 * @fn      sensorDevice_zclIasZoneInitNormalOperationModeCmdHandler
 *
 * @brief   Handler for ZCL IAS ZONE normal operation mode command.
 *
 * @param
 *
 * @return  status
 */
static status_t sensorDevice_zclIasZoneInitNormalOperationModeCmdHandler(void)
{
	u8 status = ZCL_STA_FAILURE;

	return status;
}

/*********************************************************************
 * @fn      sensorDevice_zclIasZoneInitTestModeCmdHandler
 *
 * @brief   Handler for ZCL IAS ZONE test mode command.
 *
 * @param   pZoneInitTestMode
 *
 * @return  status
 */
static status_t sensorDevice_zclIasZoneInitTestModeCmdHandler(zoneInitTestMode_t *pZoneInitTestMode)
{
	u8 status = ZCL_STA_FAILURE;

	return status;
}

/*********************************************************************
 * @fn      sensorDevice_iasZoneCb
 *
 * @brief   Handler for ZCL IAS Zone command.
 *
 * @param   pAddrInfo
 * @param   cmdId
 * @param   cmdPayload
 *
 * @return  status_t
 */
status_t sensorDevice_iasZoneCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload)
{
	status_t status = ZCL_STA_SUCCESS;

	if(pAddrInfo->dstEp == SENSOR_DEVICE_ENDPOINT){
		if(pAddrInfo->dirCluster == ZCL_FRAME_CLIENT_SERVER_DIR){
			switch(cmdId){
				case ZCL_CMD_ZONE_ENROLL_RSP:
					sensorDevice_zclIasZoneEnrollRspCmdHandler((zoneEnrollRsp_t *)cmdPayload);
					break;
				case ZCL_CMD_INIT_NORMAL_OPERATION_MODE:
					sensorDevice_zclIasZoneInitNormalOperationModeCmdHandler();
					break;
				case ZCL_CMD_INIT_TEST_MODE:
					sensorDevice_zclIasZoneInitTestModeCmdHandler((zoneInitTestMode_t *)cmdPayload);
					break;
				default:
					break;
			}
		}
	}

	return status;
}
#endif  /* ZCL_IAS_ZONE */

#ifdef ZCL_POLL_CTRL
/*
typedef struct {
	ev_timer_event_t *zclFastPollTimeoutTimerEvt = NULL;
	ev_timer_event_t *zclCheckInTimerEvt = NULL;
	bool isFastPollMode;
} poll_ctrl_wrk_t;
*/

poll_ctrl_wrk_t poll_ctrl_wrk = {
		.isFastPollMode  = FALSE
};

void sensorDevice_zclCheckInCmdSend(void)
{
	epInfo_t dstEpInfo;
	TL_SETSTRUCTCONTENT(dstEpInfo, 0);

	dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
	dstEpInfo.dstEp = SENSOR_DEVICE_ENDPOINT;
	dstEpInfo.profileId = HA_PROFILE_ID;

	zcl_pollCtrl_checkInCmd(SENSOR_DEVICE_ENDPOINT, &dstEpInfo, TRUE);
	sws_puts("checkInCmdSend\n");
}

s32 sensorDevice_zclCheckInTimerCb(void *arg)
{
	zcl_pollCtrlAttr_t *pPollCtrlAttr = zcl_pollCtrlAttrGet();

	if(!pPollCtrlAttr->chkInInterval){
		poll_ctrl_wrk.zclCheckInTimerEvt = NULL;
		return -1;
	}

	sensorDevice_zclCheckInCmdSend();

	return 0;
}

void sensorDevice_zclCheckInStart(void)
{
	if(zb_bindingTblSearched(ZCL_CLUSTER_GEN_POLL_CONTROL, SENSOR_DEVICE_ENDPOINT)){
		zcl_pollCtrlAttr_t *pPollCtrlAttr = zcl_pollCtrlAttrGet();

		if(!poll_ctrl_wrk.zclCheckInTimerEvt){
			poll_ctrl_wrk.zclCheckInTimerEvt = TL_ZB_TIMER_SCHEDULE(sensorDevice_zclCheckInTimerCb, NULL, pPollCtrlAttr->chkInInterval * POLL_RATE_QUARTERSECONDS);

			if(pPollCtrlAttr->chkInInterval){
				sensorDevice_zclCheckInCmdSend();
			}
		}
	}
}

void sensorDevice_zclSetFastPollMode(bool fastPollMode)
{
	zcl_pollCtrlAttr_t *pPollCtrlAttr = zcl_pollCtrlAttrGet();

	poll_ctrl_wrk.isFastPollMode = fastPollMode;
	u32 pollRate = fastPollMode ?
			pPollCtrlAttr->shortPollInterval : pPollCtrlAttr->longPollInterval;

	zb_setPollRate(pollRate * POLL_RATE_QUARTERSECONDS);
}

s32 sensorDevice_zclFastPollTimeoutCb(void *arg)
{
	sensorDevice_zclSetFastPollMode(FALSE);

	poll_ctrl_wrk.zclFastPollTimeoutTimerEvt = NULL;
	return -1;
}

static status_t sensorDevice_zclPollCtrlChkInRspCmdHandler(zcl_chkInRsp_t *pCmd)
{
	zcl_pollCtrlAttr_t *pPollCtrlAttr = zcl_pollCtrlAttrGet();

	if(pCmd->startFastPolling){
		u32 fastPollTimeoutCnt = 0;
#if 1
        if (pCmd->fastPollTimeout) {
            if (pCmd->fastPollTimeout > pPollCtrlAttr->fastPollTimeoutMax) {
                return ZCL_STA_INVALID_FIELD;
            }

            fastPollTimeoutCnt = pCmd->fastPollTimeout;
        } else {
            fastPollTimeoutCnt = pPollCtrlAttr->fastPollTimeout;
        }

        if (fastPollTimeoutCnt) {
        	sensorDevice_zclSetFastPollMode(TRUE);

            if (poll_ctrl_wrk.zclFastPollTimeoutTimerEvt) {
                TL_ZB_TIMER_CANCEL(&poll_ctrl_wrk.zclFastPollTimeoutTimerEvt);
            }
            poll_ctrl_wrk.zclFastPollTimeoutTimerEvt =
        		TL_ZB_TIMER_SCHEDULE(sensorDevice_zclFastPollTimeoutCb,
        				NULL,
						fastPollTimeoutCnt * POLL_RATE_QUARTERSECONDS);
        }
#else
		if(pCmd->fastPollTimeout){
			if(pCmd->fastPollTimeout > pPollCtrlAttr->fastPollTimeoutMax){
				return ZCL_STA_INVALID_FIELD;
			}

			fastPollTimeoutCnt = pCmd->fastPollTimeout;

			if(poll_ctrl_wrk.zclFastPollTimeoutTimerEvt){
				TL_ZB_TIMER_CANCEL(&poll_ctrl_wrk.zclFastPollTimeoutTimerEvt);
			}
		}else{
			if(!poll_ctrl_wrk.zclFastPollTimeoutTimerEvt){
				fastPollTimeoutCnt = pPollCtrlAttr->fastPollTimeout;
			}
		}

		if(!poll_ctrl_wrk.zclFastPollTimeoutTimerEvt && fastPollTimeoutCnt){
			sensorDevice_zclSetFastPollMode(TRUE);

			poll_ctrl_wrk.zclFastPollTimeoutTimerEvt = TL_ZB_TIMER_SCHEDULE(
					sensorDevice_zclFastPollTimeoutCb, NULL,
					fastPollTimeoutCnt * POLL_RATE_QUARTERSECONDS);
		}
#endif
	} else {
		//continue in normal operation and not required to go into fast poll mode.
	}

	return ZCL_STA_SUCCESS;
}

static status_t sensorDevice_zclPollCtrlFastPollStopCmdHandler(void)
{
	if(!poll_ctrl_wrk.isFastPollMode){
		return ZCL_STA_ACTION_DENIED;
	}else{
		if(poll_ctrl_wrk.zclFastPollTimeoutTimerEvt){
			TL_ZB_TIMER_CANCEL(&poll_ctrl_wrk.zclFastPollTimeoutTimerEvt);
		}
		sensorDevice_zclSetFastPollMode(FALSE);
	}

	return ZCL_STA_SUCCESS;
}

#if ZCL_THERMOSTAT_UI_CFG || USE_SENSOR_LX

status_t test_set_measure_longpoll_interval(u32 measureInterval) {
	status_t ret = ZCL_STA_SUCCESS;
	sws_printf("New measureInterval: %d sec\n", measureInterval);
	if(measureInterval < READ_SENSOR_TIMER_MIN_SEC) {
		measureInterval = READ_SENSOR_TIMER_MIN_SEC;
		ret = ZCL_STA_INVALID_VALUE;
	} else if(measureInterval > READ_SENSOR_TIMER_MAX_SEC) {
		measureInterval = READ_SENSOR_TIMER_MAX_SEC;
		ret = ZCL_STA_INVALID_VALUE;
	}
	g_sensorAppCtx.measureInterval =
			((u32)measureInterval * CLOCK_16M_SYS_TIMER_CLK_1S)
			- 25*CLOCK_16M_SYS_TIMER_CLK_1MS;
#if ZCL_THERMOSTAT_UI_CFG
	g_zcl_thermostatUICfgAttrs.measureInterval = (u8)measureInterval;
#elif USE_SENSOR_LX
	g_zcl_illuminanceAttrs.cfg.measureInterval = (u8)measureInterval;
#else
#error "POLL_CTRL measureInterval!"
#endif
	sws_printf("Set measureInterval: %d sec\n", measureInterval);

	g_zcl_pollCtrlAttrs.longPollInterval = measureInterval << 2;
	return ret;
}
#endif

static status_t sensorDevice_zclPollCtrlSetLongPollIntervalCmdHandler(zcl_setLongPollInterval_t *pCmd)
{
#if defined(ZCL_THERMOSTAT_UI_CFG) || USE_SENSOR_LX
	if(test_set_measure_longpoll_interval(pCmd->newLongPollInterval >> 2) == ZCL_STA_SUCCESS) {
		zb_setPollRate(g_zcl_pollCtrlAttrs.longPollInterval * POLL_RATE_QUARTERSECONDS);
		return ZCL_STA_SUCCESS;
	} else
		return ZCL_STA_INVALID_VALUE;
#else
	zcl_pollCtrlAttr_t *pPollCtrlAttr = zcl_pollCtrlAttrGet();
	if((pCmd->newLongPollInterval >= pPollCtrlAttr->longPollIntervalMin)
		&& (pCmd->newLongPollInterval <= 0x6E0000)
		&& (pCmd->newLongPollInterval <= pPollCtrlAttr->chkInInterval) // <= 1 hr
		){
		pPollCtrlAttr->longPollInterval = pCmd->newLongPollInterval;
		zb_setPollRate(pPollCtrlAttr->longPollInterval * POLL_RATE_QUARTERSECONDS);
	}else{
		return ZCL_STA_INVALID_VALUE;
	}
	return ZCL_STA_SUCCESS;
#endif
}

static status_t sensorDevice_zclPollCtrlSetShortPollIntervalCmdHandler(zcl_setShortPollInterval_t *pCmd)
{
	zcl_pollCtrlAttr_t *pPollCtrlAttr = zcl_pollCtrlAttrGet();

	if((pCmd->newShortPollInterval >= 0x01) && (pCmd->newShortPollInterval <= 0xff)
	&& (pCmd->newShortPollInterval <= pPollCtrlAttr->longPollInterval)
		){
		pPollCtrlAttr->shortPollInterval = pCmd->newShortPollInterval;
		zb_setPollRate(pPollCtrlAttr->shortPollInterval * POLL_RATE_QUARTERSECONDS);
	}else{
		return ZCL_STA_INVALID_VALUE;
	}

	return ZCL_STA_SUCCESS;
}

/*********************************************************************
 * @fn      sensorDevice_pollCtrlCb
 *
 * @brief   Handler for ZCL Poll Control command.
 *
 * @param   pAddrInfo
 * @param   cmdId
 * @param   cmdPayload
 *
 * @return  status_t
 */
status_t sensorDevice_pollCtrlCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload)
{
	status_t status = ZCL_STA_SUCCESS;

	if(pAddrInfo->dstEp == SENSOR_DEVICE_ENDPOINT){
		if(pAddrInfo->dirCluster == ZCL_FRAME_CLIENT_SERVER_DIR){
			switch(cmdId){
				case ZCL_CMD_CHK_IN_RSP:
					status = sensorDevice_zclPollCtrlChkInRspCmdHandler((zcl_chkInRsp_t *)cmdPayload);
					break;
				case ZCL_CMD_FAST_POLL_STOP:
					status = sensorDevice_zclPollCtrlFastPollStopCmdHandler();
					break;
				case ZCL_CMD_SET_LONG_POLL_INTERVAL:
					status = sensorDevice_zclPollCtrlSetLongPollIntervalCmdHandler((zcl_setLongPollInterval_t *)cmdPayload);
					break;
				case ZCL_CMD_SET_SHORT_POLL_INTERVAL:
					status = sensorDevice_zclPollCtrlSetShortPollIntervalCmdHandler((zcl_setShortPollInterval_t *)cmdPayload);
					break;
				default:
					break;
			}
		}
	}

	return status;
}
#endif	/* ZCL_POLL_CTRL */

#ifdef ZCL_GROUP
/*********************************************************************
 * @fn      sensorDevice_zclAddGroupRspCmdHandler
 *
 * @brief   Handler for ZCL add group response command.
 *
 * @param   pAddGroupRsp
 *
 * @return  None
 */
static void sensorDevice_zclAddGroupRspCmdHandler(zcl_addGroupRsp_t *pAddGroupRsp)
{

}

/*********************************************************************
 * @fn      sensorDevice_zclViewGroupRspCmdHandler
 *
 * @brief   Handler for ZCL view group response command.
 *
 * @param   pViewGroupRsp
 *
 * @return  None
 */
static void sensorDevice_zclViewGroupRspCmdHandler(zcl_viewGroupRsp_t *pViewGroupRsp)
{

}

/*********************************************************************
 * @fn      sensorDevice_zclRemoveGroupRspCmdHandler
 *
 * @brief   Handler for ZCL remove group response command.
 *
 * @param   pRemoveGroupRsp
 *
 * @return  None
 */
static void sensorDevice_zclRemoveGroupRspCmdHandler(zcl_removeGroupRsp_t *pRemoveGroupRsp)
{

}

/*********************************************************************
 * @fn      sensorDevice_zclGetGroupMembershipRspCmdHandler
 *
 * @brief   Handler for ZCL get group membership response command.
 *
 * @param   pGetGroupMembershipRsp
 *
 * @return  None
 */
static void sensorDevice_zclGetGroupMembershipRspCmdHandler(zcl_getGroupMembershipRsp_t *pGetGroupMembershipRsp)
{

}

/*********************************************************************
 * @fn      sensorDevice_groupCb
 *
 * @brief   Handler for ZCL Group command.
 *
 * @param   pAddrInfo
 * @param   cmdId
 * @param   cmdPayload
 *
 * @return  status_t
 */
status_t sensorDevice_groupCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload)
{
	if(pAddrInfo->dstEp == SENSOR_DEVICE_ENDPOINT){
		if(pAddrInfo->dirCluster == ZCL_FRAME_SERVER_CLIENT_DIR){
			switch(cmdId){
				case ZCL_CMD_GROUP_ADD_GROUP_RSP:
					sensorDevice_zclAddGroupRspCmdHandler((zcl_addGroupRsp_t *)cmdPayload);
					break;
				case ZCL_CMD_GROUP_VIEW_GROUP_RSP:
					sensorDevice_zclViewGroupRspCmdHandler((zcl_viewGroupRsp_t *)cmdPayload);
					break;
				case ZCL_CMD_GROUP_REMOVE_GROUP_RSP:
					sensorDevice_zclRemoveGroupRspCmdHandler((zcl_removeGroupRsp_t *)cmdPayload);
					break;
				case ZCL_CMD_GROUP_GET_MEMBERSHIP_RSP:
					sensorDevice_zclGetGroupMembershipRspCmdHandler((zcl_getGroupMembershipRsp_t *)cmdPayload);
					break;
				default:
					break;
			}
		}
	}

	return ZCL_STA_SUCCESS;
}
#endif	/* ZCL_GROUP */

#ifdef ZCL_ON_OFF

#if USE_RETRY_ONOFF
extern u32 rtcSeconds;
/**********************************************************************
 * FUNCTIONS
 */
/* if(zb_isDeviceJoinedNwk()) ! */
void taskRetryOnOff(void) {
	zcl_onOffAttr_t *pOnOff = zcl_onoffAttrGet();
	if(pOnOff->timeStamp
	  && rtcSeconds - pOnOff->timeStamp < USE_RETRY_ONOFF) {
	    for (int j = 0; j < APS_BINDING_TABLE_NUM; j++) {
	    	aps_binding_entry_t *bind_tbl = &g_apsBindingTbl[j];
	        if (bind_tbl->used > 1 // статус не APS_STATUS_SUCCESS
	         && bind_tbl->clusterId == ZCL_CLUSTER_GEN_ON_OFF
			 && bind_tbl->srcEp == SENSOR_DEVICE_ENDPOINT) {
			    sws_printf("RetryOnOff dst:%08p:%02x, ep:%02x, cmd:%02x\n",
			    		&bind_tbl->dstExtAddrInfo, bind_tbl->used - 1,
						SENSOR_DEVICE_ENDPOINT, pOnOff->onOffrm);
				epInfo_t dstEpInfo;
			    TL_SETSTRUCTCONTENT(dstEpInfo, 0);
			    dstEpInfo.profileId = HA_PROFILE_ID;
			    dstEpInfo.txOptions = APS_TX_OPT_ACK_TX;
			    dstEpInfo.dstEp = bind_tbl->used - 1;
	        	bind_tbl->used = 1; // откл. повтор
			    dstEpInfo.dstAddrMode = bind_tbl->dstAddrMode;
			    memcpy(&dstEpInfo.dstAddr, &bind_tbl->dstExtAddrInfo,
			    		sizeof(dstEpInfo.dstAddr));
			    /* command 0x00 - off, 0x01 - on, 0x02 - toggle */
			    zcl_sendCmd(SENSOR_DEVICE_ENDPOINT, &dstEpInfo, ZCL_CLUSTER_GEN_ON_OFF,
			    	pOnOff->remoteOnOff, TRUE,
			    	ZCL_FRAME_CLIENT_SERVER_DIR,
					FALSE, 0, ZCL_SEQ_NUM, 0, NULL);
			    return;
	        }
	    }
	}
}

/**********************************************************************
 * FUNCTIONS
 */
void afTestOnOffCb(void *arg) {
	apsdeDataConf_t *pApsDataCnf = (apsdeDataConf_t *)arg;
	if(zb_isDeviceJoinedNwk()
		&& pApsDataCnf->clusterId == ZCL_CLUSTER_GEN_ON_OFF) {
	    for (int j = 0; j < APS_BINDING_TABLE_NUM; j++) {
	    	aps_binding_entry_t *bind_tbl = &g_apsBindingTbl[j];
	        if (bind_tbl->used
	         && bind_tbl->clusterId == ZCL_CLUSTER_GEN_ON_OFF
			 && bind_tbl->srcEp == pApsDataCnf->srcEndpoint
			 && !memcmp(&pApsDataCnf->dstAddr, &bind_tbl->dstExtAddrInfo, sizeof(pApsDataCnf->dstAddr))) {
	        	sws_printf("aps: %08p:%02x %04x:%02x %02x\n",
	        			&pApsDataCnf->dstAddr.addr_long,
	        			pApsDataCnf->dstEndpoint,
	        			pApsDataCnf->clusterId,
	        			pApsDataCnf->srcEndpoint,
	        			pApsDataCnf->status
	        	);
        		zcl_onOffAttr_t *pOnOff = zcl_onoffAttrGet();
        		if(rtcSeconds - pOnOff->timeStamp < USE_RETRY_ONOFF) {
        			if(pApsDataCnf->status == APS_STATUS_NO_ACK // 0xA7
        				|| pApsDataCnf->status == MAC_STA_NO_ACK) { // 0xE9
    	        		bind_tbl->used = pApsDataCnf->dstEndpoint + 1; // save dstEndpoint
        			} else { // APS_STATUS_SUCCESS | APS_STATUS_SHORT_ADDR_REQUESTING
    	        		bind_tbl->used = 1; // APS_STATUS_SUCCESS
        			}
        		} else { // время повторов вышло, больше не проверять
        			pOnOff->timeStamp = 0;
	        		bind_tbl->used = 1;
	        	}
	        }
	    }
	}
}
#endif

/**********************************************************************
 * FUNCTIONS
 */
void remoteCmdOnOff(u8 srcEp, u8 cmd) {
    if (zb_isDeviceJoinedNwk()) {
#if USE_REMOTE_ONOFF
    	zcl_onOffAttr_t *pOnOff = zcl_onoffAttrGet();
		pOnOff->remoteOnOff = cmd;
#endif
#if USE_RETRY_ONOFF
    	int send_flg = 0;
    	for (int j = 0; j < APS_BINDING_TABLE_NUM; j++) {
    		aps_binding_entry_t *bind_tbl = &g_apsBindingTbl[j];
            if (bind_tbl->used
             && bind_tbl->clusterId == ZCL_CLUSTER_GEN_ON_OFF
    		 && bind_tbl->srcEp == srcEp) {
            	bind_tbl->used = 1; // restart On/Off
            	send_flg = 1;
            }
        }
    	if(send_flg)
#endif
    	{
            /* command 0x00 - off, 0x01 - on, 0x02 - toggle */
            sws_printf("remoteCmdOnOff: %02x\n", cmd);
            epInfo_t dstEpInfo;
            TL_SETSTRUCTCONTENT(dstEpInfo, 0);
            dstEpInfo.profileId = HA_PROFILE_ID;
            dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
#if USE_RETRY_ONOFF
		    dstEpInfo.txOptions = APS_TX_OPT_ACK_TX;
    		pOnOff->timeStamp = rtcSeconds;
#endif
    	    zcl_sendCmd(srcEp, &dstEpInfo, ZCL_CLUSTER_GEN_ON_OFF,
    	    	cmd, TRUE,
    	    	ZCL_FRAME_CLIENT_SERVER_DIR,
    			FALSE, 0, ZCL_SEQ_NUM, 0, NULL);
    	}
    }
}

/*******************************************************************
 * @brief	cmdOnOff_set
 */
void cmdOnOff_set(bool status) {
	sws_printf("cmdOnOff_set(%d)\n", status);
	zcl_onOffAttr_t *pOnOff = zcl_onoffAttrGet();
	if(pOnOff->onOff != status) {
		pOnOff->onOff = status;
		if(pOnOff->startUpOnOff >= ZCL_START_UP_ONOFF_SET_ONOFF_TOGGLE) {
	      	zcl_onoffConfig_save();
		}
#if USE_REMOTE_ONOFF && (DEV_SERVICES & SERVICE_PIR)
		if(status == ZCL_ONOFF_STATUS_OFF) {
			if(pOnOff->remoteOnOff) {
				remoteCmdOnOff(SENSOR_DEVICE_ENDPOINT, ZCL_CMD_ONOFF_OFF);
			}
		}
#endif
    }
}

/*******************************************************************
 * @brief	onOffCb
 */
status_t app_onOffCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload) {
    sws_printf("app_onOffCb(%d#%04x)\n", pAddrInfo->dstEp, cmdId);
    if(pAddrInfo->dstEp == SENSOR_DEVICE_ENDPOINT) {
    	switch(cmdId){
    		case ZCL_CMD_ONOFF_ON:
    			cmdOnOff_set(ZCL_ONOFF_STATUS_ON);
                break;
    		case ZCL_CMD_ONOFF_OFF:
    			cmdOnOff_set(ZCL_ONOFF_STATUS_OFF);
    			break;
    		case ZCL_CMD_ONOFF_TOGGLE:
    			cmdOnOff_set(!g_zcl_onOffAttrs.onOff);
    			break;
            default:
            	break;
    	}
    }
    return ZCL_STA_SUCCESS;
}

#endif // ZCL_ON_OFF
