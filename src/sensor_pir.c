/*
 * sensor_pir.c
 *
 *  Created on: 3 апр. 2026 г.
 *      Author: pvvx
 */

#include "tl_common.h"
#if (DEV_SERVICES & SERVICE_PIR)
#include "app_main.h"
#if USE_SENSOR_LX
#include "zcl_illuminance_level_sensing.h"
#include "sensor_lx.h"
#endif
#ifdef ZCL_ON_OFF
#include "app_onoff.h"
#endif

#define ONOFF_REMOTE_RETRY	1

#if ONOFF_REMOTE_RETRY

ev_timer_event_t *retryOnOfftimerEvt;

/*******************************************************************
 * @brief	 Remote Retry OnOff timer callback
 */
static s32 remoteRetryOnOffTimerCb(void *arg) {
	g_sensorAppCtx.reportFlg = 1;

	remoteCmdOnOff(SENSOR_DEVICE_ENDPOINT, zcl_occupAttr.occupancy);
	retryOnOfftimerEvt = NULL;
	return -1;
}

/*******************************************************************
 * @brief	 Remote Retry OnOff start timer
 */
static void remoteOnOff(u8 cmd) {
	remoteCmdOnOff(SENSOR_DEVICE_ENDPOINT, cmd);
	if (zb_isDeviceJoinedNwk()) {
		if(retryOnOfftimerEvt) {
			TL_ZB_TIMER_CANCEL(&retryOnOfftimerEvt);
		}
		retryOnOfftimerEvt = TL_ZB_TIMER_SCHEDULE(remoteRetryOnOffTimerCb, NULL, 8000);
	}
}
#endif

/*******************************************************************
 * @brief	Occupancy timer callback
 */
static s32 occupancyTimerCb(void *arg) {
	if(gpio_read(GPIO_PIR)? PIR_ON : !PIR_ON) {
		// occupancy
		zcl_occupAttr.occupancy = 1;
		return 0;
	}
	// occupancy timer off
	zcl_occupAttr.occupancy = 0;
	g_sensorAppCtx.reportFlg = 1;
#if USE_ONOFF
	//if(g_zcl_onOffAttrs.onOff)
#if ONOFF_REMOTE_RETRY
	remoteOnOff(ZCL_CMD_ONOFF_OFF);
#else
	remoteCmdOnOff(SENSOR_DEVICE_ENDPOINT, ZCL_CMD_ONOFF_OFF);
#endif
#endif
	zcl_occupAttr.timerEvt = NULL;
	return -1;
}

/*******************************************************************
 * @brief	Task PIR
 */
void task_pir(void) {
	u8 pir_on = gpio_read(GPIO_PIR)? PIR_ON : !PIR_ON;
	if(zcl_occupAttr.delay) {
		if(pir_on) {
			if(!zcl_occupAttr.state) {
#ifdef  GPIO_LED
				gpio_write(GPIO_LED, LED_ON);
#endif
				if(!zcl_occupAttr.occupancy) {
					// event occupancy
#ifdef ZCL_ILLUMINANCE_LEVEL_SENSING
					read_illumi_sensor();
					if(g_zcl_illuminanceAttrs.levelStatus != ILSC_ABOVE_TARGET)
						// illuminance < minlevel
#endif
					{
						zcl_occupAttr.occupancy = 1;
						g_sensorAppCtx.reportFlg = 1; // report enable
#if USE_ONOFF
						if(g_zcl_onOffAttrs.onOff) {
							// remote On/Off enable
#if ONOFF_REMOTE_RETRY
							remoteOnOff(ZCL_CMD_ONOFF_ON);
#else
							remoteCmdOnOff(SENSOR_DEVICE_ENDPOINT, ZCL_CMD_ONOFF_ON);
#endif
						}
#endif
					}
				}
			}
			if(zcl_occupAttr.timerEvt) {
				TL_ZB_TIMER_CANCEL(&zcl_occupAttr.timerEvt);
			}
			zcl_occupAttr.timerEvt = TL_ZB_TIMER_SCHEDULE(occupancyTimerCb, NULL, zcl_occupAttr.delay * 1000);
		}
	}
	zcl_occupAttr.state = pir_on;
#if PM_ENABLE
#if (!PIR_ON)
	cpu_set_gpio_wakeup(GPIO_PIR, pir_on, 1); // pir_on: 0: low-level wakeup, 1: high-level wakeup
#else
	cpu_set_gpio_wakeup(GPIO_PIR, !pir_on, 1); // !pir_on: 0: low-level wakeup, 1: high-level wakeup
#endif
#endif // PM_ENABLE
}


#endif


