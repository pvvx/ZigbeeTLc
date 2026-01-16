/********************************************************************************************************
 * @file    drv_hw.c
 *
 * @brief   This is the source file for drv_hw
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/

#include "tl_common.h"
#include "sensors.h"
#include "battery.h"
#include "ext_ota.h"

extern void drv_calib_adc_verf(void);
/*
 * system clock configuration
 */
#if(CLOCK_SYS_CLOCK_HZ == 24000000)
	#define SYS_CLOCK_VALUE		SYS_CLK_24M_Crystal
#elif(CLOCK_SYS_CLOCK_HZ == 16000000)
	#define SYS_CLOCK_VALUE		SYS_CLK_16M_Crystal
#elif(CLOCK_SYS_CLOCK_HZ == 48000000)
	#define SYS_CLOCK_VALUE		SYS_CLK_48M_Crystal
#else
	#error please config system clock
#endif

//system ticks per US
u32 sysTimerPerUs;
/*********************************************************************
 * @fn      internalFlashSizeCheck
 *
 * @brief   This function is provided to get and update to the correct flash address
 * 			where are stored the right MAC address and pre-configured parameters.
 * 			NOTE: It should be called before ZB_RADIO_INIT().
 *
 * @param   None
 *
 * @return  None
 */
static void internalFlashSizeCheck(void){
	u32 mid = flash_read_mid();
	u8 *pMid = (u8 *)&mid;

	if( (pMid[2] < FLASH_SIZE_512K) || \
		((g_u32MacFlashAddr == FLASH_ADDR_OF_MAC_ADDR_1M) && (pMid[2] < FLASH_SIZE_1M)) || \
		((g_u32MacFlashAddr == FLASH_ADDR_OF_MAC_ADDR_2M) && (pMid[2] < FLASH_SIZE_2M)) || \
		((g_u32MacFlashAddr == FLASH_ADDR_OF_MAC_ADDR_4M) && (pMid[2] < FLASH_SIZE_4M))){
		/* Flash space not matched. */
		while(1);
	}

	switch(pMid[2]){
		case FLASH_SIZE_1M:
			g_u32MacFlashAddr = FLASH_ADDR_OF_MAC_ADDR_1M;
			g_u32CfgFlashAddr = FLASH_ADDR_OF_F_CFG_INFO_1M;
			break;
		case FLASH_SIZE_2M:
			g_u32MacFlashAddr = FLASH_ADDR_OF_MAC_ADDR_2M;
			g_u32CfgFlashAddr = FLASH_ADDR_OF_F_CFG_INFO_2M;
			break;
		case FLASH_SIZE_4M:
			g_u32MacFlashAddr = FLASH_ADDR_OF_MAC_ADDR_4M;
			g_u32CfgFlashAddr = FLASH_ADDR_OF_F_CFG_INFO_4M;
			break;
		default:
			break;
	}
}

#if 0
static startup_state_e platform_wakeup_init(void)
{
	cpu_wakeup_init();
	return (startup_state_e)pm_get_mcu_status();
}
#endif

void _gpio_init(int anaRes_init_en) {
	//PA group
	reg_gpio_pa_setting1 =
		(PA0_INPUT_ENABLE<<8) 	| (PA1_INPUT_ENABLE<<9)	| (PA2_INPUT_ENABLE<<10)	| (PA3_INPUT_ENABLE<<11) |
		(PA4_INPUT_ENABLE<<12)	| (PA5_INPUT_ENABLE<<13)	| (PA6_INPUT_ENABLE<<14)	| (PA7_INPUT_ENABLE<<15) |
		((PA0_OUTPUT_ENABLE?0:1)<<16)	| ((PA1_OUTPUT_ENABLE?0:1)<<17) | ((PA2_OUTPUT_ENABLE?0:1)<<18)	| ((PA3_OUTPUT_ENABLE?0:1)<<19) |
		((PA4_OUTPUT_ENABLE?0:1)<<20)	| ((PA5_OUTPUT_ENABLE?0:1)<<21) | ((PA6_OUTPUT_ENABLE?0:1)<<22)	| ((PA7_OUTPUT_ENABLE?0:1)<<23) |
		(PA0_DATA_OUT<<24)	| (PA1_DATA_OUT<<25)	| (PA2_DATA_OUT<<26)	| (PA3_DATA_OUT<<27) |
		(PA4_DATA_OUT<<28)	| (PA5_DATA_OUT<<29)	| (PA6_DATA_OUT<<30)	| (PA7_DATA_OUT<<31) ;
	reg_gpio_pa_setting2 =
		(PA0_DATA_STRENGTH<<8)		| (PA1_DATA_STRENGTH<<9)| (PA2_DATA_STRENGTH<<10)	| (PA3_DATA_STRENGTH<<11) |
		(PA4_DATA_STRENGTH<<12)	| (PA5_DATA_STRENGTH<<13)	| (PA6_DATA_STRENGTH<<14)	| (PA7_DATA_STRENGTH<<15) |
		(PA0_FUNC==AS_GPIO ? BIT(16):0)	| (PA1_FUNC==AS_GPIO ? BIT(17):0)| (PA2_FUNC==AS_GPIO ? BIT(18):0)| (PA3_FUNC==AS_GPIO ? BIT(19):0) |
		(PA4_FUNC==AS_GPIO ? BIT(20):0)	| (PA5_FUNC==AS_GPIO ? BIT(21):0)| (PA6_FUNC==AS_GPIO ? BIT(22):0)| (PA7_FUNC==AS_GPIO ? BIT(23):0);

	//PB group
	//ie
	analog_write(areg_gpio_pb_ie, 	(PB0_INPUT_ENABLE<<0) 	| (PB1_INPUT_ENABLE<<1)	| (PB2_INPUT_ENABLE<<2)	| (PB3_INPUT_ENABLE<<3) |
									(PB4_INPUT_ENABLE<<4)	| (PB5_INPUT_ENABLE<<5) | (PB6_INPUT_ENABLE<<6)	| (PB7_INPUT_ENABLE<<7) );
	//dataO
	reg_gpio_pb_out =
		(PB0_DATA_OUT<<0)	| (PB1_DATA_OUT<<1)	| (PB2_DATA_OUT<<2)	| (PB3_DATA_OUT<<3) |
		(PB4_DATA_OUT<<4)	| (PB5_DATA_OUT<<5)	| (PB6_DATA_OUT<<6)	| (PB7_DATA_OUT<<7) ;
	//oen
	reg_gpio_pb_oen =
		((PB0_OUTPUT_ENABLE?0:1)<<0)	| ((PB1_OUTPUT_ENABLE?0:1)<<1) | ((PB2_OUTPUT_ENABLE?0:1)<<2)	| ((PB3_OUTPUT_ENABLE?0:1)<<3) |
		((PB4_OUTPUT_ENABLE?0:1)<<4)	| ((PB5_OUTPUT_ENABLE?0:1)<<5) | ((PB6_OUTPUT_ENABLE?0:1)<<6)	| ((PB7_OUTPUT_ENABLE?0:1)<<7);
	//ds
	analog_write(areg_gpio_pb_ds, 	(PB0_DATA_STRENGTH<<0) 	| (PB1_DATA_STRENGTH<<1)  | (PB2_DATA_STRENGTH<<2)	| (PB3_DATA_STRENGTH<<3) |
									(PB4_DATA_STRENGTH<<4)	| (PB5_DATA_STRENGTH<<5)  | (PB6_DATA_STRENGTH<<6)	| (PB7_DATA_STRENGTH<<7) );
	//func
	reg_gpio_pb_gpio =
		(PB0_FUNC==AS_GPIO ? BIT(0):0)	| (PB1_FUNC==AS_GPIO ? BIT(1):0)| (PB2_FUNC==AS_GPIO ? BIT(2):0)| (PB3_FUNC==AS_GPIO ? BIT(3):0) |
		(PB4_FUNC==AS_GPIO ? BIT(4):0)	| (PB5_FUNC==AS_GPIO ? BIT(5):0)| (PB6_FUNC==AS_GPIO ? BIT(6):0)| (PB7_FUNC==AS_GPIO ? BIT(7):0);

	//PC group
	//ie
	analog_write(areg_gpio_pc_ie, 	(PC0_INPUT_ENABLE<<0) 	| (PC1_INPUT_ENABLE<<1)	| (PC2_INPUT_ENABLE<<2)	| (PC3_INPUT_ENABLE<<3) |
									(PC4_INPUT_ENABLE<<4)	| (PC5_INPUT_ENABLE<<5) | (PC6_INPUT_ENABLE<<6)	| (PC7_INPUT_ENABLE<<7) );
	//dataO
	reg_gpio_pc_out =
		(PC0_DATA_OUT<<0)	| (PC1_DATA_OUT<<1)	| (PC2_DATA_OUT<<2)	| (PC3_DATA_OUT<<3) |
		(PC4_DATA_OUT<<4)	| (PC5_DATA_OUT<<5)	| (PC6_DATA_OUT<<6)	| (PC7_DATA_OUT<<7) ;
	//oen
	reg_gpio_pc_oen =
		((PC0_OUTPUT_ENABLE?0:1)<<0)	| ((PC1_OUTPUT_ENABLE?0:1)<<1) | ((PC2_OUTPUT_ENABLE?0:1)<<2)	| ((PC3_OUTPUT_ENABLE?0:1)<<3) |
		((PC4_OUTPUT_ENABLE?0:1)<<4)	| ((PC5_OUTPUT_ENABLE?0:1)<<5) | ((PC6_OUTPUT_ENABLE?0:1)<<6)	| ((PC7_OUTPUT_ENABLE?0:1)<<7);
	//ds
	analog_write(areg_gpio_pc_ds, 	(PC0_DATA_STRENGTH<<0) 	| (PC1_DATA_STRENGTH<<1)  | (PC2_DATA_STRENGTH<<2)	| (PC3_DATA_STRENGTH<<3) |
									(PC4_DATA_STRENGTH<<4)	| (PC5_DATA_STRENGTH<<5)  | (PC6_DATA_STRENGTH<<6)	| (PC7_DATA_STRENGTH<<7) );
	reg_gpio_pc_gpio =
		(PC0_FUNC==AS_GPIO ? BIT(0):0)	| (PC1_FUNC==AS_GPIO ? BIT(1):0)| (PC2_FUNC==AS_GPIO ? BIT(2):0)| (PC3_FUNC==AS_GPIO ? BIT(3):0) |
		(PC4_FUNC==AS_GPIO ? BIT(4):0)	| (PC5_FUNC==AS_GPIO ? BIT(5):0)| (PC6_FUNC==AS_GPIO ? BIT(6):0)| (PC7_FUNC==AS_GPIO ? BIT(7):0);

	//PD group
	reg_gpio_pd_setting1 =
		(PD0_INPUT_ENABLE<<8) 	| (PD1_INPUT_ENABLE<<9)	| (PD2_INPUT_ENABLE<<10)	| (PD3_INPUT_ENABLE<<11) |
		(PD4_INPUT_ENABLE<<12)	| (PD5_INPUT_ENABLE<<13)| (PD6_INPUT_ENABLE<<14)	| (PD7_INPUT_ENABLE<<15) |
		((PD0_OUTPUT_ENABLE?0:1)<<16)	| ((PD1_OUTPUT_ENABLE?0:1)<<17) | ((PD2_OUTPUT_ENABLE?0:1)<<18)	| ((PD3_OUTPUT_ENABLE?0:1)<<19) |
		((PD4_OUTPUT_ENABLE?0:1)<<20)	| ((PD5_OUTPUT_ENABLE?0:1)<<21) | ((PD6_OUTPUT_ENABLE?0:1)<<22)	| ((PD7_OUTPUT_ENABLE?0:1)<<23) |
		(PD0_DATA_OUT<<24)	| (PD1_DATA_OUT<<25)	| (PD2_DATA_OUT<<26)	| (PD3_DATA_OUT<<27) |
		(PD4_DATA_OUT<<28)	| (PD5_DATA_OUT<<29)	| (PD6_DATA_OUT<<30)	| (PD7_DATA_OUT<<31) ;
	reg_gpio_pd_setting2 =
		(PD0_DATA_STRENGTH<<8)	| (PD1_DATA_STRENGTH<<9)	| (PD2_DATA_STRENGTH<<10)	| (PD3_DATA_STRENGTH<<11) |
		(PD4_DATA_STRENGTH<<12)	| (PD5_DATA_STRENGTH<<13)	| (PD6_DATA_STRENGTH<<14)	| (PD7_DATA_STRENGTH<<15) |
		(PD0_FUNC==AS_GPIO ? BIT(16):0)	| (PD1_FUNC==AS_GPIO ? BIT(17):0)| (PD2_FUNC==AS_GPIO ? BIT(18):0)| (PD3_FUNC==AS_GPIO ? BIT(19):0) |
		(PD4_FUNC==AS_GPIO ? BIT(20):0)	| (PD5_FUNC==AS_GPIO ? BIT(21):0)| (PD6_FUNC==AS_GPIO ? BIT(22):0)| (PD7_FUNC==AS_GPIO ? BIT(23):0);

	//PE group
	reg_gpio_pe_ie = (PE0_INPUT_ENABLE<<0)	| (PE1_INPUT_ENABLE<<1)| (PE2_INPUT_ENABLE<<2)	| (PE3_INPUT_ENABLE<<3);
	reg_gpio_pe_out = (PE0_DATA_OUT<<0)	| (PE1_DATA_OUT<<1)	| (PE2_DATA_OUT<<2)	| (PE3_DATA_OUT<<3);
	reg_gpio_pe_oen = ((PE0_OUTPUT_ENABLE?0:1)<<0)	| ((PE1_OUTPUT_ENABLE?0:1)<<1) | ((PE2_OUTPUT_ENABLE?0:1)<<2)	| ((PE3_OUTPUT_ENABLE?0:1)<<3);
	reg_gpio_pe_ds = (PE0_DATA_STRENGTH<<0)	| (PE1_DATA_STRENGTH<<1)	| (PE2_DATA_STRENGTH<<2)	| (PE3_DATA_STRENGTH<<3);
	reg_gpio_pe_gpio = (PE0_FUNC==AS_GPIO ? BIT(0):0)	| (PE1_FUNC==AS_GPIO ? BIT(1):0)| (PE2_FUNC==AS_GPIO ? BIT(2):0)| (PE3_FUNC==AS_GPIO ? BIT(3):0);

	if (anaRes_init_en)	{
		analog_write (0x0e,  PULL_WAKEUP_SRC_PA0 |
							(PULL_WAKEUP_SRC_PA1<<2) |
							(PULL_WAKEUP_SRC_PA2<<4) |
							(PULL_WAKEUP_SRC_PA3<<6));

		analog_write (0x0f,  PULL_WAKEUP_SRC_PA4 |
							(PULL_WAKEUP_SRC_PA5<<2) |
							(PULL_WAKEUP_SRC_PA6<<4) |
							(PULL_WAKEUP_SRC_PA7<<6));


		analog_write (0x10,  PULL_WAKEUP_SRC_PB0 |
							(PULL_WAKEUP_SRC_PB1<<2) |
							(PULL_WAKEUP_SRC_PB2<<4) |
							(PULL_WAKEUP_SRC_PB3<<6));

		analog_write (0x11,  PULL_WAKEUP_SRC_PB4 |
							(PULL_WAKEUP_SRC_PB5<<2) |
							(PULL_WAKEUP_SRC_PB6<<4) |
							(PULL_WAKEUP_SRC_PB7<<6));


		analog_write (0x12,  PULL_WAKEUP_SRC_PC0 |
							(PULL_WAKEUP_SRC_PC1<<2) |
							(PULL_WAKEUP_SRC_PC2<<4) |
							(PULL_WAKEUP_SRC_PC3<<6));

		analog_write (0x13,  PULL_WAKEUP_SRC_PC4 |
							(PULL_WAKEUP_SRC_PC5<<2) |
							(PULL_WAKEUP_SRC_PC6<<4) |
							(PULL_WAKEUP_SRC_PC7<<6));


		analog_write (0x14,  PULL_WAKEUP_SRC_PD0 |
							(PULL_WAKEUP_SRC_PD1<<2) |
							(PULL_WAKEUP_SRC_PD2<<4) |
							(PULL_WAKEUP_SRC_PD3<<6));

		analog_write (0x15,  PULL_WAKEUP_SRC_PD4 |
							(PULL_WAKEUP_SRC_PD5<<2) |
							(PULL_WAKEUP_SRC_PD6<<4) |
							(PULL_WAKEUP_SRC_PD7<<6));
	}
}
/****************************************************************************************************
* @brief 		platform initialization function
*
* @param[in] 	none
*
* @return	  	startup_state_e.
*/
startup_state_e drv_platform_init(void)
{
	cpu_wakeup_init();

	startup_state_e state = (startup_state_e)pm_get_mcu_status();

	clock_init(SYS_CLOCK_VALUE);
	/* Get system ticks per US, must be after the clock is initialized. */
	sysTimerPerUs = sys_tick_per_us;

	_gpio_init(TRUE);

#if UART_PRINTF_MODE
	DEBUG_TX_PIN_INIT();
#endif

	/* Get calibration info to improve performance */
	if(state != SYSTEM_DEEP_RETENTION){
		// Проверка на старт по 0x20000
		tuya_zigbee_ota();
#if PM_ENABLE
		/* Initialize 32K for timer wakeup. */
		clock_32k_init(CLK_32K_RC);
		rc_32k_cal();
		pm_select_internal_32k_rc();
#endif
		drv_calib_adc_verf();
		battery_detect(1);
		random_generator_init();
		internalFlashSizeCheck();
	} else {
#if PM_ENABLE
		drv_pm_wakeupTimeUpdate();
#endif
	}

	/* ADC */
#if VOLTAGE_DETECT_ENABLE
	voltage_detect_init(VOLTAGE_DETECT_ADC_PIN);
    static u32 tick = clock_time();
	if(clock_time_exceed(tick, 500 * 1000)) { // 500 ms
		voltage_detect(0);
		tick = clock_time();
	}
#endif

	/* RF */
	ZB_RADIO_INIT();
	ZB_TIMER_INIT();

	return state;
}

void drv_enable_irq(void)
{
	irq_enable();
}

u32 drv_disable_irq(void)
{
	return (u32)irq_disable();
}

u32 drv_restore_irq(u32 en)
{
	irq_restore((u8)en);
	return 0;
}

void drv_wd_setInterval(u32 ms)
{
	wd_set_interval_ms(ms);
}

void drv_wd_start(void)
{
	wd_start();
}

void drv_wd_clear(void)
{
	wd_clear();
}

u32 drv_u32Rand(void)
{
	return rand();
}

void drv_generateRandomData(u8 *pData, u8 len)
{
	u32 randNums = 0;
	/* if len is odd */
	for(u8 i = 0; i < len; i++){
		if((i & 3) == 0){
			randNums = rand();
		}

		pData[i] = randNums & 0xff;
		randNums >>= 8;
	}
}


bool drv_get_primary_ieee_addr(u8 *addr)
{
    /* no primary IEEE */
    return FALSE;
}
