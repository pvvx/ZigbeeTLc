/********************************************************************************************************
 * @file     random.c
 *
 * @brief    This is the source file for TLSR8258
 *
 *******************************************************************************************************/
#include "tl_common.h"

struct {
	u32 w;
	u32 z;
} rnd_m;

/**
 * @brief  This function serves to read random number.
 * @param[in]  none
 * @return the result of read random number
 */
static inline unsigned short _RNG_Read(void)
{
	return (ReadAnalogReg(areg_adc_rng_h) << 8 | ReadAnalogReg(areg_adc_rng_l));
}

void random_generator_init(void)
{
	adc_reset_adc_module();
	/******enable signal of 24M clock to sar adc********/
	analog_write(areg_clk_setting, analog_read(areg_clk_setting) | FLD_CLK_24M_TO_SAR_EN);
	/******set adc clk as 4MHz******/
	analog_write(areg_adc_sampling_clk_div, 5);
	//set R_max_mc,R_max_c,R_max_s
	adc_set_state_length(40, 0, 0);     //max_mc  40:f=600K T=1.67us
	//set total length for sampling state machine and channel
	adc_set_chn_enable_and_max_state_cnt(ADC_RNS_CHN, 0);
	//set channel Vref
	adc_set_ref_voltage(ADC_MISC_CHN, ADC_VREF_0P6V);
	////set RNG src
	RNG_Set(SAR_ADC_RNG_MODE, CLOCLK_UPDATA);

	adc_power_on_sar_adc(1);//After setting the ADC parameters, turn on the ADC power supply control bit

	rnd_m.w = (_RNG_Read()<<16) | _RNG_Read();
	rnd_m.z = (_RNG_Read()<<16) | _RNG_Read();

	adc_power_on_sar_adc(0);		//After setting the ADC parameters, turn on the ADC power supply control bit
}


/**
 * @brief     This function performs to get one random number
 * @param[in] none.
 * @return    the value of one random number.
 */
unsigned int rand(void)  //16M clock, code in flash 23us, code in sram 4us
{
	rnd_m.w = 18000 * (rnd_m.w & 0xffff) + (rnd_m.w >> 16);
	rnd_m.z = 36969 * (rnd_m.z & 0xffff) + (rnd_m.z >> 16);
	unsigned int result = (rnd_m.z << 16) + rnd_m.w;

	return (unsigned int)(result ^ clock_time() );
}


/**
 * @brief      This function performs to get a serial of random number.
 * @param[in]  len- the length of random number
 * @param[in]  data - the first address of buffer store random number in
 * @return     the result of a serial of random number..
 */
void generateRandomNum(int len, unsigned char *data)
{
	int i;
	unsigned int randNums = 0;
    /* if len is odd */
	for (i=0; i<len; i++ ) {
		if( (i & 3) == 0 ){
			randNums = rand();
		}

		data[i] = randNums & 0xff;
		randNums >>=8;
	}
}

