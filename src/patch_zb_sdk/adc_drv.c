#include "tl_common.h"
//#include "sensors.h"

#ifndef ADC_BAT_VREF_MV
#define ADC_BAT_VREF_MV		1175
#endif

#define ADC_BUF_COUNT	8

/**
 * @brief This function serves to set the channel reference voltage.
 * @param[in]   ch_n - enum variable of ADC input channel.
 * @param[in]   v_ref - enum variable of ADC reference voltage.
 * @return none
 */
void adc_set_ref_voltage(ADC_ChTypeDef ch_n, ADC_RefVolTypeDef v_ref)
{
	//any two channel can not be used at the same time
	adc_set_vref(v_ref, v_ref, v_ref);

	if(v_ref == ADC_VREF_1P2V)
	{
		//Vref buffer bias current trimming: 		150%
		//Comparator preamp bias current trimming:  100%
		analog_write( areg_ain_scale  , (analog_read( areg_ain_scale  )&(0xC0)) | 0x3d );
	}
	else
	{
		//Vref buffer bias current trimming: 		100%
		//Comparator preamp bias current trimming:  100%
		analog_write( areg_ain_scale  , (analog_read( areg_ain_scale  )&(0xC0)) | 0x15 );
	}
}

/**
 * @brief This function serves to set pre_scaling.
 * @param[in]  v_scl - enum variable of ADC pre_scaling factor.
 * notice: when adc reference voltage is ADC_VREF_0P6V/ADC_VREF_0P9V, pre_scaling factor(v_scl ) must be ADC_PRESCALER_1.
 * @return none
 */
void adc_set_ain_pre_scaler(ADC_PreScalingTypeDef v_scl)
{
	analog_write( areg_ain_scale  , (analog_read( areg_ain_scale  )&(~FLD_SEL_AIN_SCALE)) | (v_scl<<6) );
	//setting adc_sel_atb ,if stat is 0,clear adc_sel_atb,else set adc_sel_atb[0]if(stat)
	unsigned char tmp;
	if(v_scl)
	{
		//ana_F9<4> must be 1
		tmp = analog_read(0xF9);
		tmp = tmp|0x10;                    //open tmp = tmp|0x10;
		analog_write (0xF9, tmp);
	}
	else
	{
		//ana_F9 <4> <5> must be 0
		tmp = analog_read(0xF9);
		tmp = tmp&0xcf;
		analog_write (0xF9, tmp);
	}
	//adc_pre_scale = 1<<(unsigned char)v_scl;
}


/*
 * libdrivers_8258.a(pm.o): In function `cpu_wakeup_no_deepretn_back_init':
 * pm.c:(.text.cpu_wakeup_no_deepretn_back_init+0x30): undefined reference to `adc_set_gpio_calib_vref'  */
void adc_set_gpio_calib_vref(u16 x) {
	(void) x;
}


//==============================================================================

#define ADC_BUF_COUNT	8

_attribute_ram_code_sec_
void adc_channel_init(ADC_InputPchTypeDef p_ain) {
#ifndef USE_RC_VBAT
	if(p_ain == SHL_ADC_VBAT) {
		// Set missing pin on case TLSR8251F512ET24/TLSR8253F512ET32
		//gpio_set_input_en(GPIO_VBAT, 0);
		gpio_write(GPIO_VBAT, 1);
		gpio_set_output_en(GPIO_VBAT, 1);
	}
#endif
	adc_power_on_sar_adc(0);
	adc_reset_adc_module(); // reset whole digital adc module
	/* enable signal of 24M clock to sar adc */
	analog_write(areg_clk_setting, analog_read(areg_clk_setting) | FLD_CLK_24M_TO_SAR_EN);
	adc_set_sample_clk(5); // set adc clk as 4MHz
	pga_left_chn_power_on(0);
	pga_right_chn_power_on(0);
	adc_set_left_right_gain_bias(GAIN_STAGE_BIAS_PER100, GAIN_STAGE_BIAS_PER100);
	// ADC_SAMPLE_RATE_96K
	adc_set_state_length(240, 0, 10); //set R_max_mc=240,R_max_s=10
	adc_set_chn_enable_and_max_state_cnt(ADC_MISC_CHN, 2);
	analog_write(anareg_adc_res_m, RES14 | FLD_ADC_EN_DIFF_CHN_M);
	adc_set_ain_chn_misc(p_ain, GND);
	/* adc set ref voltage ADC_MISC_CHN  ADC_VREF_1P2V */
	adc_set_vref(ADC_VREF_1P2V, ADC_VREF_1P2V, ADC_VREF_1P2V);
	analog_write(areg_ain_scale, (analog_read(areg_ain_scale)&(0xC0)) | 0x3d);
	// Number of ADC clock cycles in sampling phase
	adc_set_tsample_cycle_chn_misc(SAMPLING_CYCLES_6);
	// set Analog input pre-scaling and
	adc_set_ain_pre_scaler(ADC_PRESCALER_1F8);
	//set NORMAL mode
	adc_set_mode(ADC_NORMAL_MODE);
}

_attribute_ram_code_sec_
u16 get_adc_mv(int flg) { // ADC_InputPchTypeDef
	volatile unsigned int adc_dat_buf[ADC_BUF_COUNT];
	u16 temp;
	int i, j;
	adc_power_on_sar_adc(1); // + 0.4 mA
	adc_reset_adc_module();
	u32 t0 = clock_time();
	u16 adc_sample[ADC_BUF_COUNT]; // = { 0 };
	u32 adc_average;
	for (i = 0; i < ADC_BUF_COUNT; i++) {
		adc_dat_buf[i] = 0;
	}
	while (!clock_time_exceed(t0, 25)); //wait at least 2 sample cycle(f = 96K, T = 10.4us)
	adc_config_misc_channel_buf((u16 *) adc_dat_buf, sizeof(adc_dat_buf));
	dfifo_enable_dfifo2();
	sleep_us(20);
	for (i = 0; i < ADC_BUF_COUNT; i++) {
		while (!adc_dat_buf[i]);
		if (adc_dat_buf[i] & BIT(13)) {
			adc_sample[i] = 0;
		} else {
			adc_sample[i] = ((u16) adc_dat_buf[i] & 0x1FFF);
		}
		if (i) {
			if (adc_sample[i] < adc_sample[i - 1]) {
				temp = adc_sample[i];
				adc_sample[i] = adc_sample[i - 1];
				for (j = i - 1; j >= 0 && adc_sample[j] > temp; j--) {
					adc_sample[j + 1] = adc_sample[j];
				}
				adc_sample[j + 1] = temp;
			}
		}
	}
	dfifo_disable_dfifo2();
	adc_power_on_sar_adc(0); // - 0.4 mA
	adc_average = adc_sample[2] + adc_sample[3] + adc_sample[4]
			+ adc_sample[5];
	if(flg)
		return adc_average;
	return ((adc_average) * ADC_BAT_VREF_MV) >> 12; // adc_vref default: 1175 (mV)
}

