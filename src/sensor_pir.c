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


#if !USE_SENSOR_XBR818
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
	g_sensorAppCtx.reportFlg = FLG_CHECK_REPORT; // check report table
#if USE_REMOTE_ONOFF // USE_ONOFF
	if(g_zcl_onOffAttrs.remoteOnOff) {
		remoteCmdOnOff(SENSOR_DEVICE_ENDPOINT, ZCL_CMD_ONOFF_OFF);
	}
#endif
	zcl_occupAttr.timerEvt = NULL;
	return -1;
}
#endif


/*******************************************************************
 * @brief	Task PIR
 */
void task_pir(void) {
	zcl_onOffAttr_t *pOnOff = zcl_onoffAttrGet();
	u8 pir_on = gpio_read(GPIO_PIR)? PIR_ON : !PIR_ON;
	if(zcl_occupAttr.delay) {
		if(pir_on) {
			// On
			if(!zcl_occupAttr.state) {
				// event On
				if(!zcl_occupAttr.occupancy) {
					// event occupancy
#ifdef ZCL_ILLUMINANCE_LEVEL_SENSING
					read_illumi_sensor();
					if(g_zcl_illuminanceAttrs.levelStatus != ILSC_ABOVE_TARGET)
					// illuminance < minlevel
#endif
					{
						// occupancy on
						zcl_occupAttr.occupancy = 1;
						g_sensorAppCtx.reportFlg = FLG_CHECK_REPORT; // check report table
#if USE_REMOTE_ONOFF
						if(pOnOff->onOff) {
							// remote On/Off enable
							remoteCmdOnOff(SENSOR_DEVICE_ENDPOINT, ZCL_CMD_ONOFF_ON);
						}
#endif // USE_REMOTE_ONOFF
					}
				}
#ifdef  GPIO_LED
				gpio_write(GPIO_LED, LED_ON);
#endif
			}
#if !USE_SENSOR_XBR818
			if(zcl_occupAttr.timerEvt) {
				TL_ZB_TIMER_CANCEL(&zcl_occupAttr.timerEvt);
			}
			zcl_occupAttr.timerEvt = TL_ZB_TIMER_SCHEDULE(occupancyTimerCb, NULL, zcl_occupAttr.delay * 1000);
#endif
		}
#if USE_SENSOR_XBR818
		else if(zcl_occupAttr.state) { // Off
			// event Off
			if(zcl_occupAttr.occupancy) {
					// occupancy off
				zcl_occupAttr.occupancy = 0;
				g_sensorAppCtx.reportFlg = FLG_CHECK_REPORT; // check report table
#if USE_REMOTE_ONOFF
				if(pOnOff->onOff) {
					remoteCmdOnOff(SENSOR_DEVICE_ENDPOINT, ZCL_CMD_ONOFF_OFF);
				}
#endif // USE_REMOTE_ONOFF
			}
		}
#endif // USE_SENSOR_XBR818
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


