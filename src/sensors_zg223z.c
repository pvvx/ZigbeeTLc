/*
 * sensors_zg223z.c
 *
 *  Created on: 16.08.2026
 *      Author: pvvx
 */

#include "tl_common.h"
#if (BOARD == BOARD_ZG223Z) // USE_SENSOR_RND
#include "app_main.h"
#include "battery.h"
#include "zcl_illuminance_level_sensing.h"
#include "sensor_lx.h"
#ifdef USE_REMOTE_ONOFF
#include "app_onoff.h"
#endif


extern u64 mul32x32_64(u32 a, u32 b); // hard function (in div_mod.S)

//#define USE_ILLUMI_AVERAGE_SHL 	2 // 2

#ifdef USE_ILLUMI_AVERAGE_SHL
struct {
	u32 summ;
	u16 cnt;
} illumi_summ;
#endif

#if USE_SOC_TEMP_SENSOR
/*
 * 0.9(V)/32768 = 0.0000274658203125 V in 1 adc
 * 0.00051 V : 1 C -> 18.56853333 adc
 * 0.130 V : -40 C -> 4733.155 adc
 */
#define TEMP_AVERAGE_COUNT_SHL	4
#define TEMP_DEF_COEF	11029 // 4096*100*0.0000274658203125/0.00051/2 = 11029.41
// Soc1: +26C = ~5500 adc, (5500*22059)>>12 = 29620, 29620 - 2600 = 27020
// Soc2: +24C = ~5400 adc, (5400*22059)>>12 = 29081, 29081 - 2600 = 26481
#define TEMP_DEF_ZERO	26600

#if TEMP_AVERAGE_COUNT_SHL
typedef struct {
	u32 summ;
	u16 cnt;
	s16 average;
} temperature_t;
#endif

temperature_t soc_temperature;

s16 calk_soc_temp(void) {
	adc_channel_init(TEMPERATURE_SENSOR_P);
	u32 adc = get_adc_mv(1);
	adc += get_adc_mv(1);
	// disable temperature sensor
	analog_write(0x07, (analog_read(0x07) | 0x10));
#if TEMP_AVERAGE_COUNT_SHL
	soc_temperature.summ += (((s32)adc * (u32)TEMP_DEF_COEF) >> 12) - (u32)TEMP_DEF_ZERO;
	soc_temperature.cnt++;
	if(soc_temperature.cnt >= (1 << TEMP_AVERAGE_COUNT_SHL)) {
		soc_temperature.average = soc_temperature.summ >> TEMP_AVERAGE_COUNT_SHL;
		soc_temperature.summ -= soc_temperature.average;
		soc_temperature.cnt--;
	} else {
		soc_temperature.average = soc_temperature.summ / soc_temperature.cnt;
	}
	sws_printf("Temp: %d C, ADC: %d\n", soc_temperature.average, adc);
	return (s16)soc_temperature.average;
#else
	return (s16)((((s32)adc * (u32)temperature.cfg.coef) >> 12) - (u32)temperature.cfg.zero);
#endif
}

#endif // USE_SOC_TEMP_SENSOR

void read_sensors(void) {   // 1 ms
	u32 adcvbat, adcrn, adclx;
	// Enable pullup 1 MOm Raindrop sensor and +Vbat
	gpio_setup_up_down_resistor(GPIO_RNDS, PM_PIN_PULLUP_1M);
	// Turning on the lx sensor power
	gpio_write(GPIO_ILLUMI_ON, ILLUMI_POWER_ON);
	gpio_set_output_en(GPIO_ILLUMI_ON, 1);
	// TODO: lx sensor - wait 3 ms !

	// Measuring Ubat
	battery_detect(0);
	adcvbat = adc_average;
	// Measuring Raindrop
	adc_channel_init(SHL_ADC_RND);
	adcrn = get_adc_mv(1);// adc value x4

	g_zcl_temperatureAttrs.measuredValue = calk_soc_temp() + g_zcl_thermostatUICfgAttrs.temp_offset;

	// Measuring illuminance
	adc_channel_init(SHL_ADC_ILLUMI);
	adclx = get_adc_mv(1);// adc value x4
	// Turning off the lx sensor power
	gpio_write(GPIO_ILLUMI_ON, !(ILLUMI_POWER_ON));
	gpio_set_output_en(GPIO_ILLUMI_ON, 0);
	// Return GPIO_RNDS to GPIO
	gpio_set_func(GPIO_RNDS, AS_GPIO);

 	sws_printf("Sensors bat, rn, t, lx: %d, %d, %d, %d\n", adcvbat, adcrn, g_zcl_temperatureAttrs.measuredValue, adclx);

	if(adcvbat > adcrn) {
		adcrn = adcvbat - adcrn;
		adcrn <<= 16;
		adcrn /= adcvbat; // Ub/Ur = 0..65535
		adcrn *= 10500;
		adcrn >>= 16;
	} else {
		adcrn = 0;
	}
	adcrn += g_zcl_thermostatUICfgAttrs.humi_offset;
	if(adcrn > 9999)
		adcrn = 9999;
	g_zcl_relHumidityAttrs.measuredValue = adcrn;
	sws_printf("RH: %d\n", adcrn);

	// calculation of illuminance Lx & Zigbee Lx values
	if(adcvbat > adclx) {
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
		adclx = g_zcl_illuminanceAttrs.cfg.k;
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
	sws_printf("Sensor lx: %d, %d\n", adclx, il_status);
#else
	sws_printf("Sensor lx: %dzlx\n", adclx);
#endif // ZCL_ILLUMINANCE_LEVEL_SENSING
#ifdef ZCL_DIHUMIDIFICATION_CONTROL
	zcl_dhumCfgAttr.rh = (adcrn + 50)/100;
	if(!zcl_dhumCfgAttr.lockout) {
		if(zcl_dhumCfgAttr.cooling) {
			// send Off
#if USE_REMOTE_ONOFF
			remoteCmdOnOff(SENSOR_DEVICE_ENDPOINT, ZCL_CMD_ONOFF_OFF);
#endif
			zcl_dhumCfgAttr.cooling = 0;
		}
	} else {
		s32 rhp = zcl_dhumCfgAttr.setpoint * 100;
		s32 hstr = zcl_dhumCfgAttr.hysteresis * 100;
		if(zcl_dhumCfgAttr.cooling) {
			if((s32)adcrn <= rhp - hstr ) {
				// send Off
#if USE_REMOTE_ONOFF
				remoteCmdOnOff(SENSOR_DEVICE_ENDPOINT, ZCL_CMD_ONOFF_OFF);
#endif
				zcl_dhumCfgAttr.cooling = 0;
			}
		} else if(adcrn >= rhp + hstr) {
			// send On
#if USE_REMOTE_ONOFF
			remoteCmdOnOff(SENSOR_DEVICE_ENDPOINT, ZCL_CMD_ONOFF_ON);
#endif
			zcl_dhumCfgAttr.cooling = zcl_dhumCfgAttr.maxcool;
		}
	}
#endif
	g_sensorAppCtx.reportFlg = FLG_CHECK_REPORT; // check report table
}

void init_sensor(void) {
	read_sensors();
/*
	if(GPIO_RNDS) {

	}
*/
}


#endif // USE_SENSOR_RND
