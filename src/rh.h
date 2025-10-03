/*
 * rh.h
 *
 *  Created on: 24 июл. 2024 г.
 *      Author: pvvx
 */

#ifndef _RH_H_
#define _RH_H_

// R = 7.5 kOm, КД521 (BAV99), C 4.7 nF

/*********************************************************************************
    PWM0   :  PA2.  PC1.  PC2.	PD5
    PWM1   :  PA3.  PC3.
    PWM2   :  PA4.  PC4.
    PWM3   :  PB0.  PD2.
    PWM4   :  PB1.  PB4.
    PWM5   :  PB2.  PB5.
    PWM0_N :  PA0.  PB3.  PC4	PD5
    PWM1_N :  PC1.  PD3.
    PWM2_N :  PD4.
    PWM3_N :  PC5.
    PWM4_N :  PC0.  PC6.
    PWM5_N :  PC7.
 *********************************************************************************/

/*
// PWM
#define PWM_PIN		GPIO_PB4
#define AS_PWMx		AS_PWM4
#define PWM_ID		PWM4_ID
// ADC
#define GPIO_RHI	GPIO_PB5
#define CHNL_RHI	B5P
*/
#if (DEV_SERVICES & SERVICE_PLM)

typedef struct _sensor_rh_coef_t {
	u32 k;
	u16 z;
	u16 d;
} sensor_rh_coef_t;

typedef struct _sensor_rh_t {
	sensor_rh_coef_t coef;
	u16 adc_rh;
	u16 adc_d;
#ifdef USE_AVERAGE_RH_SHL
	u32 summ_rh;
	u16 cnt_summ;
#endif
	u16 rh;	// in 0.01 %
} sensor_rh_t;

extern sensor_rh_t sensor_rh;

void init_rh_sensor(void);
int	read_rh_sensor(void);

#endif // (DEV_SERVICES & SERVICE_PLM)


#endif /* RH_H_ */
