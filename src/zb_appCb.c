/********************************************************************************************************
 * @file    zb_appCb.c
 *
 * @brief   This is the source file for zb_appCb
 *
 *
 *******************************************************************************************************/


/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "ota.h"
#include "app_main.h"
#include "app_ui.h"
#include "lcd.h"
#if USE_BLE
#include "zigbee_ble_switch.h"
#include "stack/ble/ble.h"
#include "ble_cfg.h"
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
void zb_bdbInitCb(u8 status, u8 joinedNetwork);
void zb_bdbCommissioningCb(u8 status, void *arg);
void zb_bdbIdentifyCb(u8 endpoint, u16 srcAddr, u16 identifyTime);
void zb_bdbFindBindSuccessCb(findBindDst_t *pDstInfo);

/**********************************************************************
 * LOCAL VARIABLES
 */
bdb_appCb_t g_zbDemoBdbCb =
{
	zb_bdbInitCb,
	zb_bdbCommissioningCb,
	zb_bdbIdentifyCb,
	zb_bdbFindBindSuccessCb
};

#ifdef ZCL_OTA
ota_callBack_t sensorDevice_otaCb =
{
	sensorDevice_otaProcessMsgHandler,
};
#endif

/**********************************************************************
 * FUNCTIONS
 */
s32 sensorDevice_bdbNetworkSteerStart(void *arg){

	bdb_networkSteerStart();

	g_sensorAppCtx.timerSteerEvt = NULL;
	return -1;
}

#if FIND_AND_BIND_SUPPORT
s32 sensorSwitch_bdbFindAndBindStart(void *arg){
	BDB_ATTR_GROUP_ID_SET(0x1234);//only for initiator
	bdb_findAndBindStart(BDB_COMMISSIONING_ROLE_INITIATOR);

	g_sensorAppCtx.bdbFBTimerEvt = NULL;
	return -1;
}
#endif

#if REJOIN_FAILURE_TIMER

#define REJOIN_FAILURE_COUNT  4 // рестарт с 2-x периодов по 10 sec
/*
 * Первые 6 периодов по 10 секунд (~1 минута)
 * Далее 25 периодов по 55 секунды (~22 минуты)
 * Далее 96 пероиодов с увеличением на 2 секунды от 65 секунды до 262 секунд
 * (этот цикл длится ~261 минута, 4.4 часа)
 * Далее бесконечно с периодом в 262 секунды (4.4 минуты)
 */
inline void sensorDevice_rejoin_faillure_timer_set(void) {
	u32 period;
	if(g_sensorAppCtx.timerSteerEvt) {
		TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerSteerEvt);
	}
	if(++g_sensorAppCtx.rejoin_cnt > 128) // max 128*2 = 256 sec
		g_sensorAppCtx.rejoin_cnt = 128;

	if(g_sensorAppCtx.rejoin_cnt < 7)
		period = 10 << 10; // ~10 sec
	else if(g_sensorAppCtx.rejoin_cnt < (7+25))
		period = 55 << 10; // (ms), ~55 sec
	else // g_sensorAppCtx.rejoin_cnt > 32 ...
		period = g_sensorAppCtx.rejoin_cnt << 11; //(ms), 32*2048..128*2048, 65..262 sec
	g_sensorAppCtx.timerSteerEvt = TL_ZB_TIMER_SCHEDULE(sensorDevice_bdbNetworkSteerStart, NULL, period);
}

static s32 sensorDevice_rejoinBackoff(void *arg){
#if REJOIN_FAILURE_TIMER // & OLD_SDK
	if(zb_isDeviceFactoryNew()){
		g_sensorAppCtx.timerRejoinBackoffEvt = NULL;
		return -1;
	}

    zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
    return 0;
#else
    static bool rejoinMode = REJOIN_SECURITY;

    if (zb_isDeviceFactoryNew()) {
    	g_sensorAppCtx.timerRejoinBackoffEvt = NULL;
        return -1;
    }

    //printf("rejoin mode = %d\n", rejoinMode);

    zb_rejoinSecModeSet(rejoinMode);
    zb_rejoinReq(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);

    rejoinMode = !rejoinMode;
#endif
}

#endif

/*********************************************************************
 * @fn      zbdemo_bdbInitCb
 *
 * @brief   application callback for bdb initiation
 *
 * @param   status - the status of bdb init BDB_INIT_STATUS_SUCCESS or BDB_INIT_STATUS_FAILURE
 *
 * @param   joinedNetwork  - 1: node is on a network, 0: node isn't on a network
 *
 * @return  None
 */
void zb_bdbInitCb(u8 status, u8 joinedNetwork){
	sws_printf("BDB: InitCb: %02x\n", status);
	if(status == BDB_INIT_STATUS_SUCCESS){
		/*
		 * for non-factory-new device:
		 * 		load zcl data from NV, start poll rate, start ota query, bdb_networkSteerStart
		 *
		 * for factory-new device:
		 * 		steer a network
		 *
		 */
		if(joinedNetwork){

			set_PollRate(); // zb_setPollRate(DEFAULT_POLL_RATE);

#ifdef ZCL_OTA
			ota_queryStart(OTA_PERIODIC_QUERY_INTERVAL); // 15 * 60);	// 15 m
#endif

#ifdef ZCL_POLL_CTRL
			sensorDevice_zclCheckInStart();
#endif
		} else {
			u16 jitter = 0;
			do {
				jitter = zb_random() % 0x0fff;
			}while(jitter == 0);

			if(g_sensorAppCtx.timerSteerEvt){
				TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerSteerEvt);
			}
			///time_soff = 0;
			g_sensorAppCtx.timerSteerEvt = TL_ZB_TIMER_SCHEDULE(sensorDevice_bdbNetworkSteerStart, NULL, jitter);
#if USE_BLE
			g_dualModeInfo.bleStart = 1;
#endif
		}
	}
#if REJOIN_FAILURE_TIMER
	else
	{
		if(joinedNetwork) {
#if 1	// OLD_SDK
			zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
			if(!g_sensorAppCtx.timerRejoinBackoffEvt){
				TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerRejoinBackoffEvt);
			}
#else
			if(!g_sensorAppCtx.timerRejoinBackoffEvt){
				g_sensorAppCtx.timerRejoinBackoffEvt = TL_ZB_TIMER_SCHEDULE(sensorDevice_rejoinBackoff, NULL, 60 * 1000);
			}
#endif
		}
#if USE_BLE
			g_dualModeInfo.bleStart = 1;
#endif
	}
#endif // REJOIN_FAILURE_TIMER
}


/*********************************************************************
 * @fn      set_PollRate
 *
 * @brief   set timer PollRate
 *   (g_zcl_thermostatUICfgAttrs.measureInterval * (4 * POLL_RATE_QUARTERSECONDS)))
 *
 * @param   none
 *
 * @return  None
 */
void set_PollRate(void) {
#if	!USE_BLE && USE_DISPLAY
	if(g_sensorAppCtx.timerTaskEvt) {
		TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerTaskEvt);
	}
#endif
	zb_setPollRate(g_zcl_pollCtrlAttrs.longPollInterval * POLL_RATE_QUARTERSECONDS);
	sws_printf("set_PollRate: %dqt\n", g_zcl_pollCtrlAttrs.longPollInterval);
}

/*********************************************************************
 * @fn      zbdemo_bdbCommissioningCb
 *
 * @brief   application callback for bdb commissioning
 *
 * @param   status - the status of bdb commissioning
 *
 * @param   arg
 *
 * @return  None
 */
void zb_bdbCommissioningCb(u8 status, void *arg){
	sws_printf("BDB Status: %02x\n", status);
	switch(status){
		case BDB_COMMISSION_STA_SUCCESS:
			sws_puts("BDB: join\n");
#if	USE_BLE
			ble_task_stop();	// BLE off
#endif
			if(g_sensorAppCtx.timerSteerEvt){
				TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerSteerEvt);
			}
			if(g_sensorAppCtx.timerRejoinBackoffEvt){
				TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerRejoinBackoffEvt);
			}

			g_sensorAppCtx.rejoin_cnt = REJOIN_FAILURE_COUNT;

			light_blink_start(10, 500, 500);

			//zb_setPollRate(POLL_RATE * 3);
			set_PollRate();

#ifdef ZCL_POLL_CTRL
		    sensorDevice_zclCheckInStart();
#endif
#ifdef ZCL_OTA
			ota_queryStart(OTA_PERIODIC_QUERY_INTERVAL);
#endif
#if	USE_DISPLAY
			show_connected_symbol(true);
#endif
#if FIND_AND_BIND_SUPPORT
			//start Finding & Binding
			if (!g_switchAppCtx.bdbFBTimerEvt) {
				g_switchAppCtx.bdbFBTimerEvt = TL_ZB_TIMER_SCHEDULE(sampleSwitch_bdbFindAndBindStart, NULL, 50);
			}
#endif
			break;
		case BDB_COMMISSION_STA_IN_PROGRESS:
			break;
		case BDB_COMMISSION_STA_NOT_AA_CAPABLE:
			break;
		case BDB_COMMISSION_STA_NO_NETWORK:
		case BDB_COMMISSION_STA_TCLK_EX_FAILURE:
		case BDB_COMMISSION_STA_TARGET_FAILURE:
			sws_puts("BDB: join\n");
#if REJOIN_FAILURE_TIMER
			sensorDevice_rejoin_faillure_timer_set();
#else
			u16 jitter = 0;
			do {
				jitter = zb_random() % 0x0fff;
			} while(jitter == 0);

			if(g_sensorAppCtx.timerSteerEvt) {
				TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerSteerEvt);
			}
			g_sensorAppCtx.timerSteerEvt = TL_ZB_TIMER_SCHEDULE(sensorDevice_bdbNetworkSteerStart, NULL, jitter);
#endif
#if	USE_DISPLAY
			show_connected_symbol(false);
#endif
#if	USE_BLE
			g_dualModeInfo.bleStart = 1;
#else
#if	USE_DISPLAY
			if(!g_sensorAppCtx.timerTaskEvt)
				g_sensorAppCtx.timerTaskEvt = TL_ZB_TIMER_SCHEDULE(sensors_task, NULL, READ_SENSOR_TIMER_MS);
#endif
#endif
			break;
		case BDB_COMMISSION_STA_FORMATION_FAILURE:
			break;
		case BDB_COMMISSION_STA_NO_IDENTIFY_QUERY_RESPONSE:
			break;
		case BDB_COMMISSION_STA_BINDING_TABLE_FULL:
			break;
		case BDB_COMMISSION_STA_NOT_PERMITTED:
			break;
		case BDB_COMMISSION_STA_NO_SCAN_RESPONSE:
		case BDB_COMMISSION_STA_PARENT_LOST:
			sws_puts("BDB: no response or parent lost\n");
#if REJOIN_FAILURE_TIMER
			sensorDevice_rejoinBackoff(NULL);
#else
			zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
#endif
#if	USE_DISPLAY
				show_connected_symbol(false);
#endif
#if	USE_BLE
				g_dualModeInfo.bleStart = 1;
#else
#if	USE_DISPLAY
				if(!g_sensorAppCtx.timerTaskEvt)
					g_sensorAppCtx.timerTaskEvt = TL_ZB_TIMER_SCHEDULE(sensors_task, NULL, READ_SENSOR_TIMER_MS);
#endif
#endif
			break;
		case BDB_COMMISSION_STA_REJOIN_FAILURE:
			sws_puts("BDB: rejoin failure\n");
			if(!zb_isDeviceFactoryNew()){
#if REJOIN_FAILURE_TIMER
                // sleep for 6 minutes before reconnect if rejoin failed
				if(g_sensorAppCtx.timerRejoinBackoffEvt) {
					TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerRejoinBackoffEvt);
				}
                g_sensorAppCtx.timerRejoinBackoffEvt = TL_ZB_TIMER_SCHEDULE(sensorDevice_rejoinBackoff, NULL, 360 * 1000);
#else
				zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
#endif
			}
#if	USE_DISPLAY
				show_connected_symbol(false);
#endif
#if	USE_BLE
				g_dualModeInfo.bleStart = 1;
#else
#if	USE_DISPLAY
				if(!g_sensorAppCtx.timerTaskEvt)
					g_sensorAppCtx.timerTaskEvt = TL_ZB_TIMER_SCHEDULE(sensors_task, NULL, READ_SENSOR_TIMER_MS);
#endif
#endif
			break;
		default:
			break;
	}
}


extern void sensorDevice_zclIdentifyCmdHandler(u8 endpoint, u16 srcAddr, u16 identifyTime);
void zb_bdbIdentifyCb(u8 endpoint, u16 srcAddr, u16 identifyTime){
#if FIND_AND_BIND_SUPPORT
	sensorDevice_zclIdentifyCmdHandler(endpoint, srcAddr, identifyTime);
#endif
}

/*********************************************************************
 * @fn      zbdemo_bdbFindBindSuccessCb
 *
 * @brief   application callback for finding & binding
 *
 * @param   pDstInfo
 *
 * @return  None
 */
void zb_bdbFindBindSuccessCb(findBindDst_t *pDstInfo){
#if FIND_AND_BIND_SUPPORT
	epInfo_t dstEpInfo;
	TL_SETSTRUCTCONTENT(dstEpInfo, 0);

	dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
	dstEpInfo.dstAddr.shortAddr = pDstInfo->addr;
	dstEpInfo.dstEp = pDstInfo->endpoint;
	dstEpInfo.profileId = HA_PROFILE_ID;

	zcl_identify_identifyCmd(SENSOR_DEVICE_ENDPOINT, &dstEpInfo, FALSE, 0, 0);
#endif
}


#ifdef ZCL_OTA
void sensorDevice_otaProcessMsgHandler(u8 evt, u8 status)
{
	if(evt == OTA_EVT_START){
		if(status == ZCL_STA_SUCCESS){
			zb_setPollRate(QUEUE_POLL_RATE);
		}else{

		}
	}else if(evt == OTA_EVT_COMPLETE){
		set_PollRate(); // zb_setPollRate(DEFAULT_POLL_RATE);

		if(status == ZCL_STA_SUCCESS){
			ota_mcuReboot();
		}else{
			ota_queryStart(OTA_PERIODIC_QUERY_INTERVAL);
		}
	}else if(evt == OTA_EVT_IMAGE_DONE){
		set_PollRate(); // zb_setPollRate(DEFAULT_POLL_RATE);
	}
}
#endif

/*********************************************************************
 * @fn      sensorDevice_leaveCnfHandler
 *
 * @brief   Handler for ZDO Leave Confirm message.
 *
 * @param   pRsp - parameter of leave confirm
 *
 * @return  None
 */
void sensorDevice_leaveCnfHandler(nlme_leave_cnf_t *pLeaveCnf)
{
    if(pLeaveCnf->status == SUCCESS){
/*
		if(g_sensorAppCtx.timerRejoinBackoffEvt){
			TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerRejoinBackoffEvt);
		}
*/
    	SYSTEM_RESET();
    }
}

/*********************************************************************
 * @fn      sensorDevice_leaveIndHandler
 *
 * @brief   Handler for ZDO leave indication message.
 *
 * @param   pInd - parameter of leave indication
 *
 * @return  None
 */
void sensorDevice_leaveIndHandler(nlme_leave_ind_t *pLeaveInd)
{
    sws_printf("leaveIndHandler, rejoin = %d, %8p\n", pLeaveInd->rejoin, pLeaveInd->deviceAddr);
}

#if !USE_BLE
/*********************************************************************
 * @fn      sensorDevice_nwkStatusIndHandler
 *
 * @brief   Handler for NWK status indication message.
 *
 * @param   pInd - parameter of NWK status indication
 *
 * @return  None
 */
void sensorDevice_nwkStatusIndHandler(zdo_nwk_status_ind_t *pNwkStatusInd)
{
    sws_printf("nwkStatusIndHandler: addr = %x, status = %x\n", pNwkStatusInd->shortAddr, pNwkStatusInd->status);

    if (pNwkStatusInd->status == NWK_COMMAND_STATUS_BAD_FRAME_COUNTER) {
        tl_zb_normal_neighbor_entry_t *nbe = nwk_neTblGetByShortAddr(pNwkStatusInd->shortAddr);
        if (nbe) {
            sws_printf("curFC = %d, rcvFC = %d, failCnt = %d\n", nbe->incomingFrameCnt, nbe->receivedFrameCnt, nbe->frameCounterFailCnt);
        }
    } else if (pNwkStatusInd->status == NWK_COMMAND_STATUS_BAD_KEY_SEQUENCE_NUMBER) {
        zb_rejoinSecModeSet(REJOIN_INSECURITY);
        zb_rejoinReq(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
    }
}
#endif

