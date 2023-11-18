#include "tl_common.h"
#include "sensors.h"

#define USE_READ_ADC_CALIBRATION	0
#define ADC_CALIBRATION_VREF		1175

#define ADC_BUF_COUNT	8

//ADC reference voltage cfg
typedef struct {
	u16 vref; //default: 1175 mV
	s8  offset;
} adc_vref_ctr_t;

_attribute_data_retention_
adc_vref_ctr_t adc_vref_cfg; /* = {
	.vref 		= ADC_CALIBRATION_VREF, //default ADC ref voltage (unit:mV)
	.offset		= 0 	//default disable
}; */

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

void drv_calib_adc_verf(void)
{
#if USE_READ_ADC_CALIBRATION
	u8 adc_vref_calib_value[7] = {0};

	flash_read(CFG_ADC_CALIBRATION, 7, adc_vref_calib_value);
#if BOARD == BOARD_MJWSD05MMC
#warning "Calculate adc_vref!"
#endif
#endif
	adc_vref_cfg.vref = ADC_CALIBRATION_VREF;
	adc_vref_cfg.offset = 0;

#if USE_READ_ADC_CALIBRATION
	//Check the two-point gpio calibration value whether is exist
	if((adc_vref_calib_value[4] != 0xff) &&
	   (adc_vref_calib_value[4] <= 0x7f) &&
	   (adc_vref_calib_value[5] != 0xff) &&
	   (adc_vref_calib_value[6] != 0xff)){
		/****** Method of calculating two-point gpio calibration Flash_gain and Flash_offset value: ********/
		/****** Vref = [(Seven_Byte << 8) + Six_Byte + 1000]mv ********/
		/****** offset = [Five_Byte - 20] mv. ********/
		adc_vref_cfg.vref = (adc_vref_calib_value[6] << 8) + adc_vref_calib_value[5] + 1000;
		adc_vref_cfg.offset = adc_vref_calib_value[4] - 20;
	} else if(adc_vref_calib_value[0] != 0xff && adc_vref_calib_value[1] != 0xff) {
		/****** If flash do not exist the two-point gpio calibration value,use the one-point gpio calibration value ********/
		/****** Method of calculating one-point gpio calibration Flash_gpio_Vref value: ********/
		/****** Vref = [1175 +First_Byte-255+Second_Byte] mV = [920 + First_Byte + Second_Byte] mV ********/
		adc_vref_cfg.vref = 920 + adc_vref_calib_value[0] + adc_vref_calib_value[1];
		/****** Check the one-point calibration value whether is correct ********/
		if(adc_vref_cfg.vref < ADC_CALIBRATION_VREF - 128 || adc_vref_cfg.vref > ADC_CALIBRATION_VREF - 127)
			adc_vref_cfg.vref = ADC_CALIBRATION_VREF;
	}
#endif
}

/*
 * libdrivers_8258.a(pm.o): In function `cpu_wakeup_no_deepretn_back_init':
 * pm.c:(.text.cpu_wakeup_no_deepretn_back_init+0x30): undefined reference to `adc_set_gpio_calib_vref'
 */
void adc_set_gpio_calib_vref(u16 x) {

}

//==============================================================================

// Process takes about 120 μs at CPU CLK 24Mhz.
static void adc_channel_init(ADC_InputPchTypeDef p_ain) {
#if 0 // gpio set in app_config.h ?
	if(p_ain == SHL_ADC_VBAT) {
		// Set missing pin on case TLSR8251F512ET24/TLSR8253F512ET32
		gpio_set_output_en(GPIO_VBAT, 1);
		gpio_set_input_en(GPIO_VBAT, 0);
		gpio_write(GPIO_VBAT, 1);
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

// Process takes about 260 μs at CPU CLK 24Mhz.
u16 get_adc_mv(void) { // ADC_InputPchTypeDef
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
	adc_average = (adc_sample[2] + adc_sample[3] + adc_sample[4]
			+ adc_sample[5]) / 4;
#if BOARD == BOARD_MJWSD05MMC
	return ((adc_average + adc_vref_cfg.offset) * 1686) >> 10; // adc_vref default: 1175 (mV)
#else
	return ((adc_average + adc_vref_cfg.offset) * adc_vref_cfg.vref) >> 10; // adc_vref default: 1175 (mV)
#endif
}


void battery_detect(void)
{
	adc_channel_init(SHL_ADC_VBAT);
	measured_data.battery_mv = get_adc_mv();
	//printf("VDD: %d\n", voltage);
	if(measured_data.battery_mv < BATTERY_SAFETY_THRESHOLD){
#if PM_ENABLE
		drv_pm_sleep(PM_SLEEP_MODE_DEEPSLEEP, 0, 60*1000);
#else
		SYSTEM_RESET();
#endif
	}
}

