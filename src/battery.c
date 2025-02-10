/*
 * battery.c
 *
 *  Created on: 18 нояб. 2023 г.
 *      Author: pvvx
 */

#include "tl_common.h"
#include "device.h"
#include "sensors.h"
#include "battery.h"

measured_battery_t measured_battery;

#define _BAT_SPEED_CODE_SEC_ _attribute_ram_code_sec_ // for speed


#define BAT_AVERAGE_COUNT	32

struct {
	u32 summ;
	u16 count;
	u16 battery_mv;
} bat_average;

_BAT_SPEED_CODE_SEC_
__attribute__((optimize("-Os")))
static u16 battery_average(u16 battery_mv) {
	u16 bat_battery_mv;

	bat_average.summ += battery_mv;
	bat_average.count++;

	bat_battery_mv = bat_average.summ / bat_average.count;

	if(bat_average.count >= BAT_AVERAGE_COUNT) {
		bat_average.summ >>= 1;
		bat_average.count >>= 1;
	}
	return bat_battery_mv;
}

_BAT_SPEED_CODE_SEC_
void battery_detect(void)
{
	adc_channel_init(SHL_ADC_VBAT);
	measured_battery.mv = get_adc_mv();
	if(measured_battery.mv < BATTERY_SAFETY_THRESHOLD){
#if PM_ENABLE
		sensor_go_sleep();
		drv_pm_sleep(PM_SLEEP_MODE_DEEPSLEEP, PM_WAKEUP_SRC_TIMER, 60*1000);
#else
		SYSTEM_RESET();
#endif
	}
	measured_battery.average_mv = battery_average(measured_battery.mv);
	measured_battery.level = (measured_battery.average_mv - BATTERY_SAFETY_THRESHOLD)/4;
    if(measured_battery.level > 200)
    	measured_battery.level = 200;
}
