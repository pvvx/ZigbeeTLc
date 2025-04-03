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

	gpio_init(TRUE);

#if UART_PRINTF_MODE
	DEBUG_TX_PIN_INIT();
#endif

	/* Get calibration info to improve performance */
	if(state != SYSTEM_DEEP_RETENTION){
		tuya_zigbee_ota();
#if PM_ENABLE
		/* Initialize 32K for timer wakeup. */
		clock_32k_init(CLK_32K_RC);
		rc_32k_cal();
		pm_select_internal_32k_rc();
#endif
		drv_calib_adc_verf();
		battery_detect();
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
