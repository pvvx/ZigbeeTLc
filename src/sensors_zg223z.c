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

extern u64 mul32x32_64(u32 a, u32 b); // hard function (in div_mod.S)

#define USE_ILLUMI_AVERAGE_SHL 	2 // 2

#ifdef USE_ILLUMI_AVERAGE_SHL
struct {
	u32 summ;
	u16 cnt;
} illumi_summ;
#endif

// Коэффициенты полинома для 2^x, x∈[0,1) (масштаб 2^F)
#define A1      726817U
#define A2      251834U
#define A3      58191U
#define A4      10087U

/* lx = pow(10, x/10000) - 1 */
u32 pow10_fixed(u16 x) {
	if(x < 1760)
		return 0;
    // 1. t_fixed = x * K >> M
    u32 t_fixed = mul32x32_64(x, 356680) >> 10; // * round(log2(10)*2^(F+M)/10000)
    u32 int_part = t_fixed >> 20;
    u32 frac = t_fixed & ((1U << 20) - 1);

    // 2. 2^frac через полином
    u32 f2 = mul32x32_64(frac, frac) >> 20;
    u32 f3 = mul32x32_64(f2, frac) >> 20;
    u32 f4 = mul32x32_64(f3, frac) >> 20;

    u32 approx = (1U << 20)
    	+ (mul32x32_64(A1, frac) >> 20)
		+ (mul32x32_64(A2, f2) >> 20)
		+ (mul32x32_64(A3, f3) >> 20)
		+ (mul32x32_64(A4, f4) >> 20);

    // 3. Умножаем на 2^int_part и убираем масштаб
    u64 res = (u64)approx << int_part;
    return (u32)((res >> 20) + ((res >> 19) & 1)) - 1;
}

// Таблица 10000 * log10(i/10) для i = 10..100 (мантисса от 1.0 до 10.0 с шагом 0.1)
static const u16 log_table[91] = {
	   0,  414,	 792, 1139, 1461, 1761, 2041, 2304, 2553, 2788, // 1.0..1.9
	3010, 3222, 3424, 3617, 3802, 3979, 4150, 4314, 4472, 4624, // 2.0..2.9
	4771, 4914, 5052, 5185, 5315, 5441, 5563, 5682, 5798, 5911, // 3.0..3.9
	6021, 6128, 6232, 6335, 6435, 6532, 6628, 6721, 6812, 6902, // 4.0..4.9
	6990, 7076, 7160, 7243, 7324, 7404, 7482, 7559, 7634, 7709, // 5.0..5.9
	7782, 7853, 7924, 7993, 8062, 8129, 8195, 8261, 8325, 8388, // 6.0..6.9
	8451, 8513, 8573, 8633, 8692, 8751, 8808, 8865, 8921, 8976, // 7.0..7.9
	9031, 9085, 9138, 9191, 9243, 9294, 9345, 9395, 9445, 9494, // 8.0..8.9
	9542, 9590, 9638, 9685, 9731, 9777, 9823, 9868, 9912, 9956, // 9.0..9.9
   10000														  // 10.0
};

// Степени десяти: 10^0 .. 10^6 (последняя для проверки границы)
static const u32 powers[8] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};

/**
 * Вычисляет 10000 * log10(x) для 1 <= x <= 3576000
 * с использованием целочисленной арифметики и таблицы логарифмов.
 */
u16 calk_10000_log10(u32 x) {
	if (x < 1)
		return 0;
	else if(x >= 3576000)
		return 65534;

	// Определяем порядок B: 10^B <= x < 10^(B+1)
	u32 B = 0;
	while (powers[B + 1] <= x) {
		B++;
	}
	u32 p = powers[B];	// 10^B

	// Масштабируем x для получения мантиссы с одним знаком после запятой
	u32 tmp = x * 10;		// x * 10
	u32 a = (u32)(tmp / p);	// целая часть мантиссы *10 (от 10 до 99)
	u32 r = (u32)(tmp % p);	// остаток для интерполяции

	u32 index = a - 10;		// индекс в таблице (0..89)
	u32 base = B * 10000;	// вклад порядка

	// Линейная интерполяция между соседними значениями таблицы
	u32 diff = log_table[index + 1] - log_table[index];
	// Округление: добавляем половину знаменателя перед делением
	u32 add = (diff * r + (p >> 1)) / p;

	return (u16)(base + log_table[index] + add);
}

#if USE_SOC_TEMP_SENSOR
/*
 * 0.9(V)/32768 = 0.0000274658203125 V in 1 adc
 * 0.00051 V : 1 C -> 18.56853333 adc
 * 0.130 V : -40 C -> 4733.155 adc
 */
#define TEMP_AVERAGE_COUNT_SHL	3
#define TEMP_DEF_COEF	22059 // 4096*100*0.0000274658203125/0.00051 = 22058.8235294
//#define TEMP_DEF_ZERO	27020 // +26C = ~5500 adc, (5500*22059)>>12 = 29620, 29620 - 2600 = 27020
#define TEMP_DEF_ZERO	26500 // +24C = ~5400 adc, (5400*22059)>>12 = 29081, 29081 - 2600 = 26481

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
	u32 adc = get_adc_mv(1);// adc value x4
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
		adcrn *= 10240;
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
	u32 min_lx = (u32)g_zcl_illuminanceAttrs.minLevelLx;
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
}


void init_sensor(void) {
	read_sensors();
	if(GPIO_RNDS) {

	}
}


#endif // USE_SENSOR_RND
