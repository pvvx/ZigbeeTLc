/*
 * ble_cfg.h
 *
 *  Created on: 29 нояб. 2023 г.
 *      Author: pvvx
 */

#ifndef SRC_BLE_CFG_H_
#define SRC_BLE_CFG_H_

//#define MTU_SIZE_SETTING 	64

#define CONNECT_ADV_INTERVAL	ADV_INTERVAL_1S	// 1000 ms * 1.6
#define CONNECT_ADV_COUNT		80

#define DEF_ADV_INTERVAL		8000	// 5000 ms * 1.6
#define DEF_APP_ADV_CHANNEL		BLT_ENABLE_ADV_ALL

/* interval: 	n*1.25 ms
 * lantency:	(n+1)*interval*1.25 ms
 * timeout:     n*10 ms
 * */
//#define DEF_CON_PAR_UPDATE			8, 8, 99, 400 // 10 ms, 10 ms, 1000 ms, 4000 ms
#define DEF_CON_PAR_UPDATE				20, 20, 39, 400 // 25 ms, 25 ms, 1000 ms, 4000 ms

#define	BLE_DEVICE_ADDRESS_TYPE 			BLE_DEVICE_ADDRESS_PUBLIC

typedef enum{
	ATT_H_START = 0,

	//// Gap ////
	/**********************************************************************************************/
	GenericAccess_PS_H, 					//UUID: 2800, 	VALUE: uuid 1800
	GenericAccess_DeviceName_CD_H,			//UUID: 2803, 	VALUE: Prop: Read | Notify
	GenericAccess_DeviceName_DP_H,			//UUID: 2A00,   VALUE: device name
	GenericAccess_Appearance_CD_H,			//UUID: 2803, 	VALUE: Prop: Read
	GenericAccess_Appearance_DP_H,			//UUID: 2A01,	VALUE: appearance
	CONN_PARAM_CD_H,						//UUID: 2803, 	VALUE: Prop: Read
	CONN_PARAM_DP_H,						//UUID: 2A04,   VALUE: connParameter


	//// gatt ////
	/**********************************************************************************************/
	GenericAttribute_PS_H,					//UUID: 2800, 	VALUE: uuid 1801
	GenericAttribute_ServiceChanged_CD_H,	//UUID: 2803, 	VALUE:  			Prop: Indicate
	GenericAttribute_ServiceChanged_DP_H,   //UUID:	2A05,	VALUE: service change
	GenericAttribute_ServiceChanged_CCB_H,	//UUID: 2902,	VALUE: serviceChangeCCC

#if USE_DEVICE_INFO_CHR_UUID
	//// device information ////
	/**********************************************************************************************/
	DeviceInformation_PS_H,					//UUID: 2800, 	VALUE: uuid 180A
	DeviceInformation_ModName_CD_H,			//UUID: 2803, 	VALUE: Prop: Read
	DeviceInformation_ModName_DP_H,			//UUID: 2A24,	VALUE: Model Number String
	DeviceInformation_SerialN_CD_H,			//UUID: 2803, 	VALUE: Prop: Read
	DeviceInformation_SerialN_DP_H,			//UUID: 2A25,	VALUE: Serial Number String
	DeviceInformation_FirmRev_CD_H,			//UUID: 2803, 	VALUE: Prop: Read
	DeviceInformation_FirmRev_DP_H,			//UUID: 2A26,	VALUE: Firmware Revision String
	DeviceInformation_HardRev_CD_H,			//UUID: 2803, 	VALUE: Prop: Read
	DeviceInformation_HardRev_DP_H,			//UUID: 2A27,	VALUE: Hardware Revision String
	DeviceInformation_SoftRev_CD_H,			//UUID: 2803, 	VALUE: Prop: Read
	DeviceInformation_SoftRev_DP_H,			//UUID: 2A28,	VALUE: Software Revision String
	DeviceInformation_ManName_CD_H,			//UUID: 2803, 	VALUE: Prop: Read
	DeviceInformation_ManName_DP_H,			//UUID: 2A29,	VALUE: Manufacturer Name String
#endif

	//// battery service ////
	/**********************************************************************************************/
	BATT_PS_H, 								//UUID: 2800, 	VALUE: uuid 180f
	BATT_LEVEL_INPUT_CD_H,					//UUID: 2803, 	VALUE: Prop: Read | Notify
	BATT_LEVEL_INPUT_DP_H,					//UUID: 2A19 	VALUE: batVal
	BATT_LEVEL_INPUT_CCB_H,					//UUID: 2902, 	VALUE: batValCCC

	//// Custom RxTx ////
	/**********************************************************************************************/
	RxTx_PS_H, 								//UUID: 2800, 	VALUE: BTHome (0xFCD2) RxTx service uuid
	RxTx_CMD_CD_H,							//UUID: 2803, 	VALUE: Prop: read | write_without_rsp
	RxTx_CMD_Data,							//UUID: 1F1F,   VALUE: RxTxData
	RxTx_CMD_Name,							//UUID: 2901, 	VALUE: RxTxName
	RxTx_CMD_DESC_H,						//UUID: 2902, 	VALUE: RxTxValueInCCC

#if USE_BLE_OTA
	//// Ota ////
	/**********************************************************************************************/
	OTA_PS_H, 								//UUID: 2800, 	VALUE: telink ota service uuid
	OTA_CMD_OUT_CD_H,						//UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
	OTA_CMD_OUT_DP_H,						//UUID: telink ota uuid,  VALUE: otaData
	OTA_CMD_OUT_DESC_H,						//UUID: 2901, 	VALUE: otaName
#endif

	ATT_END_H

} ATT_HANDLE;

// bits: ble_attr.connected
enum {
	CONNECTED_FLG_ENABLE = 0,
	CONNECTED_FLG_PAR_UPDATE = 1,
	CONNECTED_FLG_BONDING = 2,
	CONNECTED_FLG_MTU_EXCHANGE = 3,
	CONNECTED_FLG_RESET_OF_DISCONNECT = 7
} CONNECTED_FLG_BITS_e;

#define SEND_BUFFER_SIZE	(ATT_MTU_SIZE-3) // = 20

// CMD_ID_DEV_ID
typedef struct _dev_id_t{
	u8 pid;			// packet identifier = CMD_ID_DEVID
	u8 revision;	// protocol version/revision
	u16 hw_version;	// hardware version
	u16 sw_version;	// software version (BCD)
	u16 dev_spec_data;	// device-specific data (bit0..3: sensor_type)
	u32 services;		// supported services by the device
} dev_id_t, * pdev_id_t;

/* BLE data*/
typedef struct {
	u16 batteryValueInCCC;
	u16 RxTxValueInCCC;
	u8 my_HardStr[4];
	u8 my_batVal;
	u8 sendTerminate_before_enterDeep;
	u8 g_ble_txPowerSet; // = BLE_DEFAULT_TX_POWER_IDX;
	volatile u8 g_bleConnDoing;
	u8 connected;
#if USE_BLE_OTA
	volatile u8 ota_is_working;
#endif
#if defined(MTU_SIZE_SETTING)
	u8  mtuExchange_started_flg;
#endif
	dev_id_t  id;
} ble_attr_t;

extern ble_attr_t ble_attr; // ble_attr.


void startAdvTime(u8 count, u16 interval);

#endif /* SRC_BLE_CFG_H_ */
