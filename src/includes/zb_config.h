/********************************************************************************************************
 * @file    zb_config.h
 *
 * @brief   This is the header file for zb_config
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

#ifndef ZB_CONFIG_H
#define ZB_CONFIG_H


#include "tl_common.h"



/**********************************************************************
 * for zigbee stack common config
 */

//Stack profile
#define ZB_STACK_PROFILE 							2

#define ZB_FRAME_TYPE_MASK							0x03

#define ZB_PROTOCOL_VER_MASK						0x3c
#define ZB_PROTOCOL_VER_POS							2

//Protocol version
#define ZB_PROTOCOL_VERSION 						2

#define ZB_PROTOCOL_VERSION_GP						3


//Default value for mac frame version subfield
#define MAC_FRAME_VERSION 							MAC_FRAME_IEEE_802_15_4_2003

//Address assign (NOTE: We only use stochastic address assign for ZigBee PRO)
//If defined, use distributed address assign for tree and for mesh routing (ZigBee 2007).
#ifndef ZB_NWK_DISTRIBUTED_ADDRESS_ASSIGN
//If defined, use stochastic address assign (ZigBee PRO).
  #define ZB_NWK_STOCHASTIC_ADDRESS_ASSING
#endif

/* Some defaults for ZDO startup */
/********************************************************************
 * @brief	ZDO layer external configure items
 */

/*******************************ZOO Default Configuration Attribute Definitions***********************************************/
//Permit join duration, 0x00 - disable join, 0xff - join is allowed forever
#define ZDO_PERMIT_JOIN_DURATION					0

#define POLL_RATE_QUARTERSECONDS					250 // 1 qs = 250 ms

#define POLL_NO_DATA_MAX_COUNT						3

//Integer value representing the number of scan attempts to make before
//the NWK layer decides which ZigBee coordinator or router to associate with.
//This attribute has default value of 5 and valid values between 1 and 255.
#define	ZDO_NWK_SCAN_ATTEMPTS						1

//Integer value representing the time duration between each NWK discovery attempt described by :Config_NWK_Scan_Attempts.
//This attribute has a default value 0xc35 (100 milliseconds) and valid values between 1 and 65535 milliseconds.
#define ZDO_NWK_TIME_BTWN_SCANS 					100

//The value in milliseconds, for the device to request indirect transmission messages from the parent.
#define ZDO_NWK_INDIRECT_POLL_RATE					4 * POLL_RATE_QUARTERSECONDS //1000 ms

//Contents of the link retry threshold for parent link.
#define	ZDO_MAX_PARENT_THRESHOLD_RETRY				5

//Contents of the rejoin interval in seconds
//The number of rejoin attempts during the fast rejoin.
#define ZDO_REJOIN_TIMES							5
//The amount of time between each rejoin attempt while the device is in Fast Rejoin mode, in seconds.
//If 0, config_rejoin_times will be ignored and only one Fast Rejoin will be performed.
#define	ZDO_REJOIN_DURATION							6
//The amount of time to sleep after the Fast Rejoin attempts before performing the next attempt, in seconds.
//If 0 means no Rejoin backoff/retry.
#define ZDO_REJOIN_BACKOFF_TIME						30
//Upper limit of the config_rejoin_backoff_time.
#define ZDO_MAX_REJOIN_BACKOFF_TIME					90
//The number of iterations of the Fast Rejoin backoff, in times.
//If 0 means do not reset the backoff duration.
#define ZDO_REJOIN_BACKOFF_ITERATION				8
/******************************************************************************************************************************/

#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278) || defined(MCU_CORE_B91)
#if ZB_ROUTER_ROLE
#if ZB_COORDINATOR_ROLE
	#define NWK_ROUTE_RECORD_TABLE_NUM				127//300
#endif
	#define TL_ZB_NWK_ADDR_MAP_NUM					128//301
	#define ROUTING_TABLE_NUM						48//250
#endif
#elif defined (MCU_CORE_826x)
#if ZB_ROUTER_ROLE
#if ZB_COORDINATOR_ROLE
	#define NWK_ROUTE_RECORD_TABLE_NUM				64
#endif
	#define TL_ZB_NWK_ADDR_MAP_NUM					64
	#define ROUTING_TABLE_NUM						24
#endif
#endif

/*************************************************************************************/
#define POLL_RATE									(4 * POLL_RATE_QUARTERSECONDS)//4 qs = 1s
#define RESPONSE_POLL_RATE							POLL_RATE_QUARTERSECONDS//250ms
#define QUEUE_POLL_RATE								POLL_RATE_QUARTERSECONDS//250ms
#define REJOIN_POLL_RATE							(2 * POLL_RATE_QUARTERSECONDS)//500ms
/*************************************************************************************/

//CCM key size. Hard-coded
#define	CCM_KEY_SIZE								16

//NLS5 - All devices shall maintain at least 2 NWK keys with the frame
//counters consistent with the security mode of the network (Standard or High).
#define	SECUR_N_SECUR_MATERIAL						2

//If defined, 2007 stack profile is implemented
#define ZB_STACK_PROFILE_2007

//SECUR: CCM M parameter. Fixed to 4 for security level 5
#define	ZB_CCM_M									4

//if defined, enable APS security
#define APS_FRAME_SECURITY

#define ZB_SECURITY									1

#if ZB_ROUTER_ROLE
	#define GP_SUPPORT_ENABLE					  	1
#endif

#endif	/* ZB_CONFIG_H */
