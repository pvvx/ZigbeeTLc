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

//--- check battery
#define BAT_AVERAGE_SHL		4 // 16*16 = 256 ( 256*10/60 = 42.7 min)
#define BAT_AVERAGE_COUNT	(1 << BAT_AVERAGE_SHL) // 16
struct {
	u32 buf2[BAT_AVERAGE_COUNT];
	u16 buf1[BAT_AVERAGE_COUNT];
	u8 index1;
	u8 index2;
} bat_average;

_BAT_SPEED_CODE_SEC_
__attribute__((optimize("-Os")))
static u16 battery_average(u16 battery_mv) {
	u32 i;
	u32 summ = 0;
	bat_average.index1++;
	bat_average.index1 &= BAT_AVERAGE_COUNT - 1;
	if(bat_average.index1 == 0) {
		bat_average.index2++;
		bat_average.index2 &= BAT_AVERAGE_COUNT - 1;
	}
	bat_average.buf1[bat_average.index1] = battery_mv;
	for(i = 0; i < BAT_AVERAGE_COUNT; i++)
		summ += bat_average.buf1[i];
	bat_average.buf2[bat_average.index2] = summ;
	summ = 0;
	for(i = 0; i < BAT_AVERAGE_COUNT; i++)
		summ += bat_average.buf2[i];
	return summ >> (2*BAT_AVERAGE_SHL);
}

__attribute__((optimize("-Os")))
static void battery_average_init(u16 battery_mv) {
	int i;
	for(i = 0; i < BAT_AVERAGE_COUNT; i++)
		bat_average.buf1[i] = battery_mv;
	battery_mv <<= BAT_AVERAGE_SHL;
	for(i = 0; i < BAT_AVERAGE_COUNT; i++)
		bat_average.buf2[i] = battery_mv;
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
	if(!measured_battery.average_mv)
		battery_average_init(measured_battery.mv);
	measured_battery.average_mv = battery_average(measured_battery.mv);
	measured_battery.level = (measured_battery.average_mv - BATTERY_SAFETY_THRESHOLD)/4;
    if(measured_battery.level > 200)
    	measured_battery.level = 200;
}
