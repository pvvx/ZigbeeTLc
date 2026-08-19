/*
 * senser_lx.c
 *
 *  Created on: 3 апр. 2026 г.
 *      Author: pvvx
 */


#include "tl_common.h"
#if (USE_SENSOR_LX == 1) || (USE_SENSOR_LX == 2)
#include "app_main.h"
#include "battery.h"
#include "zcl_illuminance_level_sensing.h"
#include "sensor_lx.h"

//#define USE_ILLUMI_AVERAGE_SHL 	2 // 2

#ifdef USE_ILLUMI_AVERAGE_SHL
struct {
	u32 summ;
	u16 cnt;
} illumi_summ;
#endif

//u32 old_lx;

int read_illumi_sensor(void) {
	u32 adcvbat, adclx;
	// turning on the sensor power
	gpio_write(GPIO_ILLUMI_ON, ILLUMI_POWER_ON);
	gpio_set_output_en(GPIO_ILLUMI_ON, 1);
#ifdef GPIO_ADC_PULL
	gpio_setup_up_down_resistor(GPIO_ILLUMI_ADC, GPIO_ADC_PULL);
#endif
	// sampling Ubat, Ux
	battery_detect(0);
	adcvbat = adc_average;
	adc_channel_init(SHL_ADC_ILLUMI);
	adclx = get_adc_mv(1);// adc value x4
	// turning off the sensor power
	gpio_write(GPIO_ILLUMI_ON, !ILLUMI_POWER_ON);
	gpio_set_output_en(GPIO_ILLUMI_ON, 0);
#ifdef GPIO_ADC_PULL
	gpio_setup_up_down_resistor(GPIO_ILLUMI_ADC, PM_PIN_UP_DOWN_FLOAT);
#endif
	// calculation of values
	if(adcvbat > adclx) {
#if USE_SENSOR_LX == 2 // =1 - ADC = Ur, =2 - ADC = Us
		adclx = adcvbat - adclx; // Ubat - Usense = Ur
#endif
		adclx <<= 16;
		adclx /= adcvbat; // Ub/Ur = 0..65535
		adclx *= adclx;
		adclx >>= 16;	 // 0..0xfffe
		adclx *= adclx;
		adclx >>= 16; // 0..0xfffc
		if(adclx < g_zcl_illuminanceAttrs.cfg.z) {
			adclx = 0;
		} else {
			adclx -= g_zcl_illuminanceAttrs.cfg.z;
			adclx *= g_zcl_illuminanceAttrs.cfg.k;
			adclx >>= 16;
		}
	} else {
#if USE_SENSOR_LX == 2 // =1 - ADC = Ur, =2 - ADC = Us
		adclx = 0;
#else
		adclx = g_zcl_illuminanceAttrs.cfg.k;
#endif
	}
#ifdef USE_ILLUMI_AVERAGE_SHL
	illumi_summ.summ += adclx;
	illumi_summ.cnt++;
	if(illumi_summ.cnt >= (1<<USE_ILLUMI_AVERAGE_SHL)) {
		adclx = illumi_summ.summ >> USE_ILLUMI_AVERAGE_SHL;
		illumi_summ.summ -= adclx;
		illumi_summ.cnt--;
	} else {
		adclx = illumi_summ.summ / illumi_summ.cnt;
	}
#else
/*	if(!old_lx)
		old_lx = adclx;
	old_lx += adclx;
	adclx = old_lx >> 1;
	old_lx -= adclx; */
#endif
	adclx = calk_10000_log10(adclx); // lx -> Zigbee lx
	g_zcl_illuminanceAttrs.measuredVal = adclx;
#ifdef ZCL_ILLUMINANCE_LEVEL_SENSING
	u8 il_status = ILSC_NONE;
	u32 min_lx = (u32)g_zcl_illuminanceAttrs.TargetLevelzLx;
	if(adclx != 0xffff && min_lx != 0xffff) {
		if(min_lx) {
			if(adclx < min_lx) {
				il_status = ILSC_BELOW_TARGET;
			} else if (adclx == min_lx) {
				il_status = ILSC_ON_TARGET;
			} else {
				il_status = ILSC_ABOVE_TARGET;
			}
		} else {
			il_status = ILSC_BELOW_TARGET;
		}
	} else {
		il_status = ILSC_NONE;
	}
	g_zcl_illuminanceAttrs.levelStatus = il_status;
	sws_printf("Sensor: %dzlx, %d\n", adclx, il_status);
#else
	sws_printf("Sensor: %dzlx\n", adclx);
#endif // ZCL_ILLUMINANCE_LEVEL_SENSING
	return 0;
}

#if USE_SENSOR_LX == 1 // =1 - ADC = Ur, =2 - ADC = Us
void init_sensor(void) {

}
#endif

#endif // USE_SENSOR_LX
