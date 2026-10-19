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
#if USE_SENSOR_RC2417
#include "sensor_rc2417.h"
#endif
#include "app_ui.h"


#if !USE_SENSOR_XBR818
/*******************************************************************
 * @brief	Occupancy timer callback
 */
static s32 occupancyTimerCb(void *arg) {
#if USE_SENSOR_RC2417
	u8 pir_on = gpio_read(GPIO_PIR)? PIR_ON : !PIR_ON;
	u8 radar_on = rc2417_read_out();
	if(radar_on || pir_on) {
		// occupancy maintained by mmWave radar or PIR
		u8 occ = 0x01; // Bit 0: overall occupied
		if(pir_on) occ |= 0x02; // Bit 1: PIR
		if(radar_on) occ |= 0x04; // Bit 2: mmWave radar
		if(zcl_occupAttr.occupancy != occ) {
			zcl_occupAttr.occupancy = occ;
			g_sensorAppCtx.reportFlg = FLG_CHECK_REPORT; // check report table
		}
		return 0;
	}
	// occupancy timer off -> shut down radar to conserve power
	rc2417_go_sleep();
#else
	if(gpio_read(GPIO_PIR)? PIR_ON : !PIR_ON) {
		// occupancy
		zcl_occupAttr.occupancy = 1;
		return 0;
	}
#endif
	// occupancy timer off
	zcl_occupAttr.occupancy = 0;
	g_sensorAppCtx.reportFlg = FLG_CHECK_REPORT; // check report table
#if USE_BLINK_LED
	light_off();
#endif
#if USE_REMOTE_ONOFF // USE_ONOFF
	if(g_zcl_onOffAttrs.remoteOnOff) {
		// old cmd = ZCL_CMD_ONOFF_ON -> ZCL_CMD_ONOFF_OFF
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
#if USE_SENSOR_XBR818
	/* XBR818 handles the hold time in hardware; delay==0 is valid. */
#else
	if(zcl_occupAttr.delay)
#endif
	{
		if(pir_on) {
			// PIR On
			if(!zcl_occupAttr.state) {
				// event On
#if USE_SENSOR_RC2417
				rc2417_wake_and_start();
				u8 occ = 0x01 | 0x02; // Bit 0: overall occupied, Bit 1: PIR motion
				if(rc2417_read_out()) occ |= 0x04; // Bit 2: mmWave radar
#else
				u8 occ = 1;
#endif
				if(zcl_occupAttr.occupancy != occ) {
					// event occupancy
					// occupancy on
					zcl_occupAttr.occupancy = occ;
					g_sensorAppCtx.reportFlg = FLG_CHECK_REPORT; // check report table
#ifdef ZCL_ILLUMINANCE_LEVEL_SENSING
#if !USE_SENSOR_XBR818
#ifdef  GPIO_LED
					gpio_write(GPIO_LED, LED_OFF);
#endif
#endif
					read_illumi_sensor();
					if(g_zcl_illuminanceAttrs.levelStatus != ILSC_ABOVE_TARGET)
					// if illuminance < minlevel
#endif
					{
						// occupancy on
//						zcl_occupAttr.occupancy = 1;
//						g_sensorAppCtx.reportFlg = FLG_CHECK_REPORT; // check report table
#if USE_REMOTE_ONOFF
						if(pOnOff->onOff) {
							// remote On/Off enable
							remoteCmdOnOff(SENSOR_DEVICE_ENDPOINT, ZCL_CMD_ONOFF_ON);
						}
#endif // USE_REMOTE_ONOFF
					}
				}
#if USE_BLINK_LED
				light_blink_start(1, 50, 50);
#endif
#if !USE_SENSOR_XBR818
				// set timer occupancy off
				if(zcl_occupAttr.timerEvt) {
					TL_ZB_TIMER_CANCEL(&zcl_occupAttr.timerEvt);
				}
				zcl_occupAttr.timerEvt = TL_ZB_TIMER_SCHEDULE(occupancyTimerCb, NULL, zcl_occupAttr.delay * 1000);
#endif
			}
		}
#if USE_SENSOR_RC2417
		if(rc2417_is_powered()) {
			u8 radar_on = rc2417_read_out();
			u8 current_radar = (zcl_occupAttr.occupancy & 0x04) ? 1 : 0;
			u8 current_pir = (zcl_occupAttr.occupancy & 0x02) ? 1 : 0;
			if(radar_on != current_radar || pir_on != current_pir) {
				u8 occ = zcl_occupAttr.occupancy & ~0x06;
				if(pir_on) occ |= 0x01 | 0x02;
				if(radar_on) {
					occ |= 0x01 | 0x04;
					// Re-arm hold timer while presence is maintained
					if(zcl_occupAttr.timerEvt) {
						TL_ZB_TIMER_CANCEL(&zcl_occupAttr.timerEvt);
					}
					zcl_occupAttr.timerEvt = TL_ZB_TIMER_SCHEDULE(occupancyTimerCb, NULL, zcl_occupAttr.delay * 1000);
				}
				if(zcl_occupAttr.occupancy != occ) {
					zcl_occupAttr.occupancy = occ;
					g_sensorAppCtx.reportFlg = FLG_CHECK_REPORT; // check report table
				}
			}
		}
#endif
#if USE_SENSOR_XBR818
	    else if(zcl_occupAttr.state) { // PIR Off, state = On?
			// event Off
			if(zcl_occupAttr.occupancy) {
				// occupancy off
				zcl_occupAttr.occupancy = 0;
				g_sensorAppCtx.reportFlg = FLG_CHECK_REPORT; // check report table
#if USE_REMOTE_ONOFF
				if(pOnOff->onOff && pOnOff->remoteOnOff) {
					// remote On/Off enable, old cmd = ZCL_CMD_ONOFF_ON -> ZCL_CMD_ONOFF_OFF
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
#if USE_SENSOR_RC2417
	if(rc2417_is_powered()) {
		u8 radar_lvl = rc2417_read_out();
		cpu_set_gpio_wakeup(RC2417_OUT_PIN, !radar_lvl, 1); // Wakeup on level change
	} else {
		cpu_set_gpio_wakeup(RC2417_OUT_PIN, 0, 0); // Disable radar wakeup when radar is unpowered
	}
#endif
#endif // PM_ENABLE
}


#endif


