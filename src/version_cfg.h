/********************************************************************************************************
 * @file    version_cfg.h
 *
 * @brief   This is the header file for version_cfg
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
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
#ifndef _VERSION_CFG_H_
#define _VERSION_CFG_H_

#ifndef USE_BLE
#define USE_BLE			0
#endif

#define BOOT_LOADER_MODE					0

/* Boot loader address. */
#define BOOT_LOADER_IMAGE_ADDR				0x0

/* APP image address. */
#if (BOOT_LOADER_MODE)
	#define APP_IMAGE_ADDR					0x8000
#else
	#define APP_IMAGE_ADDR					0x0
#endif

/* Board ID */
#define BOARD_LYWSD03MMC_B14		0 // number used for BLE firmware!
#define BOARD_MHO_C401				1 // SHTC3/SHT4X
#define BOARD_CGG1					2
#define BOARD_LYWSD03MMC_B19		3 // number used for BLE firmware!
//#define BOARD_LYWSD03MMC_DEVBIS	3 // ver https://github.com/devbis/z03mmc
#define BOARD_LYWSD03MMC_B16		4 // number used for BLE firmware!
//#define BOARD_WATERMETER			4 // ver https://github.com/slacky1965/watermeter_zed
#define BOARD_LYWSD03MMC_B20		5 // B2.0 or B1.7 number used for BLE firmware!
#define BOARD_CGDK2					6 //
#define BOARD_CGG1N					7 // 2022
#define BOARD_MHO_C401N				8 // 2022, SHTC3/SHT4X
//#define BOARD_MJWSD05MMC			9
#define BOARD_LYWSD03MMC_B15		10 // number used for BLE firmware!
#define BOARD_LYWSD03MMC			10
#define BOARD_MHO_C122				11
//#define BOARD_MJWSD06MMC_EN		12
#define BOARD_MJWSD06MMC			13
// 12..15 - number used for BLE firmware - https://github.com/pvvx/ATC_MiThermometer
//#define BOARD_TB03F				16 // DIY TB-03F-Kit (not yet published at the moment)
#define BOARD_TS0201_TZ3000			17 // ZigBee TS0201_TZ3000
//#define BOARD_TNKS				18 // Water tank controller (not yet published at the moment)
//#define BOARD_THB2				19 // https://github.com/pvvx/THB2
//#define BOARD_BTH01				20 // https://github.com/pvvx/THB2
//#define BOARD_TH05				21 // https://github.com/pvvx/THB2
#define BOARD_TH03Z   				22 // ZigBee TH03Z
//#define BOARD_THB1				23 // https://github.com/pvvx/THB2
//#define BOARD_TH05D				24 // TH05_V1.3 https://github.com/pvvx/THB2
//#define BOARD_TH05F				25 // TH05Y_V1.2 https://github.com/pvvx/THB2
//#define BOARD_THB3				26 // https://github.com/pvvx/THB2
#define BOARD_ZTH01   				27 // ZigBee ZTH01
#define BOARD_ZTH02   				28 // ZigBee ZTH02
//#define BOARD_PLM1 				29  // Tuya BLE Plant monitor ECF-SGS01-A rev1.3 (BT3L Tuya module)  (not yet published at the moment)
#define BOARD_ZTH03   				30 // ZigBee TH03 TS0201(_TZ3000_gohcgzj7, _TZ3000_bguser20) LCD
#define BOARD_LKTMZL02				31 // ZigBee LKTMZL02 LCD
//#define BOARD_KEY2 				32 // KEY2(iSearching) https://github.com/pvvx/THB2
#define BOARD_ZTH05					33 // Tuya ZigBee TS0601_TZE204
//#define BOARD_TH04				34 // https://github.com/pvvx/THB2
#define BOARD_CB3S					35  // development is not completed!Tuya ZigBee "Smart Button" TS0041_TZ3000_fa9mlvja
#define BOARD_HS09					36  // development is not completed! TS0201_TZ3000_1twfmkcc: Tuya ZigBee "Smart Humidity Sensor"
#define BOARD_ZYZTH02				37 // Tuya ZY-ZTH02 Zigbee, 2 x AAA, SHT30/CHT832x
#define BOARD_ZYZTH01				38  // Tuya ZY-ZTH02Pro Zigbee LCD, 2 x AAA, SHT30/CHT832x
#define BOARD_ZG_227Z				39  // Zigbee ZG-227Z
#define BOARD_TS0202_PIR1 			40 // Tuya TS0202_TZ3000_6ygjfyll PIR
#define BOARD_TS0202_PIR2 			41 // Tuya TS0202_TZ3040_bb6xaihh PIR
//#define BOARD_MINTAGF2			42 // https://github.com/pvvx/THB2


/* Board define */
#ifndef BOARD
#define BOARD					BOARD_LYWSD03MMC // BOARD_LYWSD03MMC or BOARD_CGDK2 or ...
#endif

/* Chip IDs */
#define TLSR_8267				0x00
#define TLSR_8269				0x01
#define TLSR_8258_512K			0x02
#define TLSR_8258_1M			0x03
#define TLSR_8278				0x04
#define TLSR_B91				0x05

#if defined(MCU_CORE_826x)
	#if (CHIP_8269)
		#define CHIP_TYPE		TLSR_8269
	#else
		#define CHIP_TYPE		TLSR_8267
	#endif
#elif defined(MCU_CORE_8258)
		#define CHIP_TYPE		TLSR_8258_512K	//TLSR_8258_1M
#elif defined(MCU_CORE_8278)
		#define CHIP_TYPE		TLSR_8278
#elif defined(MCU_CORE_B91)
		#define CHIP_TYPE		TLSR_B91
#endif

#if USE_BLE
#define APP_RELEASE				0x00	//BCD app release "0.0"
#define APP_BUILD				0x07	//BCD app build "0.4"
#else
#define APP_RELEASE				0x01	//BCD app release "0.1"
#define APP_BUILD				0x28	//BCD app build "2.5"
#endif
#define STACK_RELEASE			0x30	//BCD stack release 3.0
#define STACK_BUILD				0x01	//BCD stack build 01

/*********************************************************************************************
 * During OTA upgrade, the upgraded device will check the rules of the following three fields.
 * Refer to ZCL OTA specification for details.
 */
#define MANUFACTURER_CODE_TELINK           	0x1141	// Telink ID
#define	IMAGE_TYPE							((CHIP_TYPE << 8) | BOARD)
#define	FILE_VERSION					  	((APP_RELEASE << 24) | (APP_BUILD << 16) | (STACK_RELEASE << 8) | STACK_BUILD)

/* Pre-compiled link configuration. */
#define IS_BOOT_LOADER_IMAGE				0
#define RESV_FOR_APP_RAM_CODE_SIZE			0
#define IMAGE_OFFSET						APP_IMAGE_ADDR

#define	DEEPRETENTION_SECTION_USED          1  // (if BLE)

#endif // _VERSION_CFG_H_

