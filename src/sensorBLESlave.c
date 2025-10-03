/********************************************************************************************************
 * @file    sensorBLESlave.c
 *
 * @brief   This is the source file for sensorBLESlave
 *
 *******************************************************************************************************/

#include "tl_common.h"
#include "device.h"
#include "stack/ble/ble.h"
#include "ble_cfg.h"
#include "flash.h"
#include "rf_drv.h"

#include "bthome_beacon.h"
#include "zigbee_ble_switch.h"
#include "cmd_parser.h"
#include "sensors.h"
#include "lcd.h"
#include "battery.h"

//#include "vendor/common/blt_led.h"
//#include "vendor/common/blt_common.h"

#define RX_FIFO_SIZE	                   64
#define RX_FIFO_NUM		                   8

#define TX_FIFO_SIZE	                   40
#define TX_FIFO_NUM		                   16

typedef struct{
	/** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
	u16 intervalMin;
	/** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
	u16 intervalMax;
	/** Number of LL latency connection events (0x0000 - 0x03e8) */
	u16 latency;
	/** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
	u16 timeout;
} gap_periConnectParams_t;

typedef struct _tbl_scanRsp_t {
	u8 size;
	u8 id;
	u8 name[13];
} tbl_scanRsp_t;

u8  mac_public[6];
u8  mac_random_static[6];

tbl_scanRsp_t tbl_scanRsp;

#if USE_BLE_OTA
static int app_bleOtaWrite(void *p);
#endif

/* various */
const u16 clientCharacterCfgUUID = GATT_UUID_CLIENT_CHAR_CFG;
const u16 characterPresentFormatUUID = GATT_UUID_CHAR_PRESENT_FORMAT;
const u16 my_primaryServiceUUID = GATT_UUID_PRIMARY_SERVICE;
static const u16 my_characterUUID = GATT_UUID_CHARACTER;
const u16 my_devServiceUUID = SERVICE_UUID_DEVICE_INFORMATION;
const u16 my_PnPUUID = CHARACTERISTIC_UUID_PNP_ID;
const u16 my_devNameUUID = GATT_UUID_DEVICE_NAME;

//device information
const u16 my_gapServiceUUID = SERVICE_UUID_GENERIC_ACCESS;
// Appearance Characteristic Properties
const u16 my_appearanceUIID = 0x2a01;
const u16 my_periConnParamUUID = 0x2a04;
u16 my_appearance = GAP_APPEARE_UNKNOWN;
const gap_periConnectParams_t my_periConnParameters = {DEF_CON_PAR_UPDATE}; // 20, 20, 39, 400 ?

#if USE_DEVICE_INFO_CHR_UUID

//#define CHARACTERISTIC_UUID_SYSTEM_ID			0x2A23 // System ID
#define CHARACTERISTIC_UUID_MODEL_NUMBER		0x2A24 // Model Number String: LYWSD03MMC
#define CHARACTERISTIC_UUID_SERIAL_NUMBER		0x2A25 // Serial Number String: F1.0-CFMK-LB-ZCXTJ--
#define CHARACTERISTIC_UUID_FIRMWARE_REV		0x2A26 // Firmware Revision String: 1.0.0_0109
#define CHARACTERISTIC_UUID_HARDWARE_REV		0x2A27 // Hardware Revision String: B1.4
#define CHARACTERISTIC_UUID_SOFTWARE_REV		0x2A28 // Software Revision String: 0x109
#define CHARACTERISTIC_UUID_MANUFACTURER_NAME	0x2A29 // Manufacturer Name String: miaomiaoce.com

//// device Information  attribute values
//static const u16 my_UUID_SYSTEM_ID		    = CHARACTERISTIC_UUID_SYSTEM_ID;
static const u16 my_UUID_MODEL_NUMBER	    = CHARACTERISTIC_UUID_MODEL_NUMBER;
static const u16 my_UUID_SERIAL_NUMBER	    = CHARACTERISTIC_UUID_SERIAL_NUMBER;
static const u16 my_UUID_FIRMWARE_REV	    = CHARACTERISTIC_UUID_FIRMWARE_REV;
static const u16 my_UUID_HARDWARE_REV	    = CHARACTERISTIC_UUID_HARDWARE_REV;
static const u16 my_UUID_SOFTWARE_REV	    = CHARACTERISTIC_UUID_SOFTWARE_REV;
static const u16 my_UUID_MANUFACTURER_NAME  = CHARACTERISTIC_UUID_MANUFACTURER_NAME;
static const u8 my_ModCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_HardRev_DP_H), U16_HI(DeviceInformation_HardRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HARDWARE_REV), U16_HI(CHARACTERISTIC_UUID_HARDWARE_REV)
};
static const u8 my_SerialCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_FirmRev_DP_H), U16_HI(DeviceInformation_FirmRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_SERIAL_NUMBER), U16_HI(CHARACTERISTIC_UUID_SERIAL_NUMBER)
};
static const u8 my_FirmCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_FirmRev_DP_H), U16_HI(DeviceInformation_FirmRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_FIRMWARE_REV), U16_HI(CHARACTERISTIC_UUID_FIRMWARE_REV)
};
static const u8 my_HardCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_HardRev_DP_H), U16_HI(DeviceInformation_HardRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HARDWARE_REV), U16_HI(CHARACTERISTIC_UUID_HARDWARE_REV)
};
static const u8 my_SoftCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_SoftRev_DP_H), U16_HI(DeviceInformation_SoftRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_SOFTWARE_REV), U16_HI(CHARACTERISTIC_UUID_SOFTWARE_REV)
};
static const u8 my_ManCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_ManName_DP_H), U16_HI(DeviceInformation_ManName_DP_H),
	U16_LO(CHARACTERISTIC_UUID_MANUFACTURER_NAME), U16_HI(CHARACTERISTIC_UUID_MANUFACTURER_NAME)
};
static const u8 my_FirmStr[] = {"github.com/pvvx"};
static const u8 my_SoftStr[] = {'Z', 'B', '0'+(APP_RELEASE>>4),'.','0'+(APP_RELEASE&0x0f),'.','0'+(APP_BUILD>>4),'.','0'+(APP_BUILD&0x0f)}; // "0.1.1.2"

#if USE_FLASH_SERIAL_UID
u8 my_SerialStr[20]; // "556202-C86013-012345"
#else
static const u8 my_SerialStr[] = {"0001"};
#endif
#if !defined(BLE_MODEL_STR) || !defined(BLE_MODEL_STR)
#error "defined BLE_MODEL_STR & BLE_MAN_STR !"
#endif
static const u8 my_ModelStr[] = {BLE_MODEL_STR};
static const u8 my_ManStr[] = {BLE_MAN_STR};
//------------------
#endif // USE_DEVICE_INFO_CHR_UUID


const u16 my_gattServiceUUID = SERVICE_UUID_GENERIC_ATTRIBUTE;
const u16 serviceChangeUIID = GATT_UUID_SERVICE_CHANGE;
u16 serviceChangeVal[2] = {0};
static u8 serviceChangeCCC[2]={0,0};


const u8 PROP_READ = CHAR_PROP_READ;
const u8 PROP_WRITE = CHAR_PROP_WRITE;
const u8 PROP_INDICATE = CHAR_PROP_INDICATE;
const u8 PROP_WRITE_NORSP = CHAR_PROP_WRITE_WITHOUT_RSP;
const u8 PROP_READ_NOTIFY = CHAR_PROP_READ | CHAR_PROP_NOTIFY;
const u8 PROP_READ_WRITE_NORSP = CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP;
const u8 PROP_READ_WRITE_WRITENORSP = CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RSP;
const u8 PROP_READ_WRITE = CHAR_PROP_READ | CHAR_PROP_WRITE;
const u8 PROP_READ_WRITE_NORSP_NOTIFY = CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP | CHAR_PROP_NOTIFY;


//////////////////////////////////////////////////////////////////////////////
//	 Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////

/*
 * battery
 * */
const u16 my_batServiceUUID   	= SERVICE_UUID_BATTERY;
const u16 my_batCharUUID        = CHARACTERISTIC_UUID_BATTERY_LEVEL;

#include "rf_drv.h"

ble_attr_t ble_attr = {
		.my_batVal = 99,
		.batteryValueInCCC = 0,
		.g_ble_txPowerSet = BLE_DEFAULT_TX_POWER
};

/*
 * ota
 * */
const u8 my_OtaUUID[16]		= TELINK_SPP_DATA_OTA;
const u8 my_OtaServiceUUID[16]		= TELINK_OTA_UUID_SERVICE;
const u16 userdesc_UUID		= GATT_UUID_CHAR_USER_DESC;
const u8  my_OtaName[]      = {'O', 'T', 'A'};
u8	my_OtaData;

//// RxTx attribute values
/////////////// RxTx/CMD Char ///////////////
#define  COMMAND_UUID16_SERVICE 0xFCD2
#define  COMMAND_UUID16_CHARACTERISTIC 0xFFF4

static const  u16 my_RxTxUUID				= COMMAND_UUID16_CHARACTERISTIC;
static const  u16 my_RxTx_ServiceUUID		= COMMAND_UUID16_SERVICE;
static const  u8 my_RxTxName[] = {'C', 'M', 'D'};

static const u8 my_RxTxCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(RxTx_CMD_Data), U16_HI(RxTx_CMD_Data),
	U16_LO(COMMAND_UUID16_CHARACTERISTIC), U16_HI(COMMAND_UUID16_CHARACTERISTIC)
};

// TM : to modify
const attribute_t my_Attributes[] = {

	{ATT_END_H - 1, 0,0,0,0,0},	// total num of attribute


	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&PROP_READ_NOTIFY), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(tbl_scanRsp.name), (u8*)(&my_devNameUUID), (u8*)(tbl_scanRsp.name), 0},
		{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&PROP_READ), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
		{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&PROP_READ), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},


	// 0008 - 000b gatt
	{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gattServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&PROP_INDICATE), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof (serviceChangeVal), (u8*)(&serviceChangeUIID), 	(u8*)(&serviceChangeVal), 0},
		{0,ATT_PERMISSIONS_RDWR,2,sizeof (serviceChangeCCC),(u8*)(&clientCharacterCfgUUID), (u8*)(serviceChangeCCC), 0},


#if USE_DEVICE_INFO_CHR_UUID
	// 000c - 0018 Device Information Service
	{13,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID),(u8*)(&my_devServiceUUID), 0},

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_ModCharVal),(u8*)(&my_characterUUID),(u8*)(my_ModCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_ModelStr),(u8*)(&my_UUID_MODEL_NUMBER),(u8*)(my_ModelStr), 0},

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_SerialCharVal),(u8*)(&my_characterUUID),(u8*)(my_SerialCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_SerialStr),(u8*)(&my_UUID_SERIAL_NUMBER),(u8*)(my_SerialStr), 0},

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_FirmCharVal),(u8*)(&my_characterUUID),(u8*)(my_FirmCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_FirmStr),(u8*)(&my_UUID_FIRMWARE_REV),(u8*)(my_FirmStr), 0},

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_HardCharVal),(u8*)(&my_characterUUID),(u8*)(my_HardCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(ble_attr.my_HardStr),(u8*)(&my_UUID_HARDWARE_REV),(u8*)(ble_attr.my_HardStr), 0},

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_SoftCharVal),(u8*)(&my_characterUUID),(u8*)(my_SoftCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_SoftStr),(u8*)(&my_UUID_SOFTWARE_REV),(u8*)(my_SoftStr), 0},

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_ManCharVal),(u8*)(&my_characterUUID),(u8*)(my_ManCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_ManStr),(u8*)(&my_UUID_MANUFACTURER_NAME),(u8*)(my_ManStr), 0},
#endif

	////////////////////////////////////// Battery Service /////////////////////////////////////////////////////
	// 0019 - 001C
	{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_batServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&PROP_READ_NOTIFY), 0},				//prop
		{0,ATT_PERMISSIONS_READ,2,sizeof(ble_attr.my_batVal),(u8*)(&my_batCharUUID), 	(u8*)(&ble_attr.my_batVal), 0},	//value
		{0,ATT_PERMISSIONS_RDWR,2,sizeof(ble_attr.batteryValueInCCC),(u8*)(&clientCharacterCfgUUID), 	(u8*)(&ble_attr.batteryValueInCCC), 0},	//value

	////////////////////////////////////// RxTx ////////////////////////////////////////////////////
	// 001D - 0021 RxTx Communication
	{5,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_RxTx_ServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ, 2,sizeof(my_RxTxCharVal),(u8*)(&my_characterUUID),	(u8*)(my_RxTxCharVal), 0},				//prop
		{0,ATT_PERMISSIONS_RDWR, 2,sizeof(ble_attr.id),(u8*)(&my_RxTxUUID), (u8*)&ble_attr.id, &cmd_parser, 0},
		{0,ATT_PERMISSIONS_RDWR, 2,sizeof(ble_attr.RxTxValueInCCC),(u8*)(&clientCharacterCfgUUID), 	(u8*)(&ble_attr.RxTxValueInCCC), 0},	//value
		{0,ATT_PERMISSIONS_READ, 2,sizeof(my_RxTxName),(u8*)(&userdesc_UUID), (u8*)(my_RxTxName), 0},

#if USE_BLE_OTA
	////////////////////////////////////// OTA /////////////////////////////////////////////////////
	// 0021 - 0024
	{4,ATT_PERMISSIONS_READ, 2,16,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_OtaServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ, 2, 1,(u8*)(&my_characterUUID), 		(u8*)(&PROP_READ_WRITE_NORSP), 0},				//prop
		{0,ATT_PERMISSIONS_RDWR,16,sizeof(my_OtaData),(u8*)(&my_OtaUUID),	(u8 *)(&my_OtaData), &app_bleOtaWrite, &otaRead},
		{0,ATT_PERMISSIONS_READ, 2,sizeof (my_OtaName),(u8*)(&userdesc_UUID), (u8*)(my_OtaName), 0},
#endif
};


_attribute_data_retention_  u8 		 	blt_rxfifo_b[RX_FIFO_SIZE * RX_FIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	blt_rxfifo = {
												RX_FIFO_SIZE,
												RX_FIFO_NUM,
												0,
												0,
												blt_rxfifo_b,};


_attribute_data_retention_  u8 		 	blt_txfifo_b[TX_FIFO_SIZE * TX_FIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	blt_txfifo = {
												TX_FIFO_SIZE,
												TX_FIFO_NUM,
												0,
												0,
												blt_txfifo_b,};


/*
 *  functions
 *
 *
 */


#if USE_BLE_OTA

void app_enter_ota_mode(void) {
	ble_attr.ota_is_working = 1;
	bls_pm_setManualLatency(0);
	bls_ota_setTimeout(16 * 1000000); // set OTA timeout  16 seconds
#if USE_DISPLAY
	show_ble_ota();
#endif
}

//extern u32 blt_ota_start_tick; // in "stack/ble/service/ble_ll_ota.h"
static int app_bleOtaWrite(void * p) {
	blt_ota_start_tick = clock_time() | 1;
	return otaWrite(p);
}

#endif


unsigned char * str_bin2hex(unsigned char *d, unsigned char *s, int len) {
	static const char* hex_ascii = { "0123456789ABCDEF" };
	while(len--) {
		*d++ = hex_ascii[(*s >> 4) & 0xf];
		*d++ = hex_ascii[(*s++ >> 0) & 0xf];
	}
	return d;
}

static void my_att_init(void){
#if USE_FLASH_SERIAL_UID
	u8 buf[16];
	u32 mid;
	u8 *p = my_SerialStr;
	// Read SoC ID, version
	buf[0] = REG_ADDR8(0x7f);
	buf[1] = REG_ADDR8(0x7e);
	buf[2] = REG_ADDR8(0x7d);
	p = str_bin2hex(p, buf, 3);
	*p++ = '-';
	memset(buf, 0, sizeof(buf));
	// Read flash ID and UID
	flash_read_mid_uid_with_check(&mid, buf);
	p = str_bin2hex(p, (unsigned char *)&mid, 3);
	*p++ = '-';
	memcpy(p, buf, 6);
	//ser_uid_txt(p, &buf[4], 7);
#endif
    bls_att_setAttributeTable ((u8 *)my_Attributes);
}


void app_switch_to_indirect_adv(u8 e, u8 *p, int n){

	bls_ll_setAdvParam( DEF_ADV_INTERVAL, DEF_ADV_INTERVAL+10,
						ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
						0,  NULL,
						DEF_APP_ADV_CHANNEL,
						ADV_FP_NONE);

	bls_ll_setAdvEnable(BLC_ADV_ENABLE);  //must: set adv enable
}


void 	task_terminate(u8 e,u8 *p, int n){ //*p is terminate reason

	ble_attr.connected = 0;

	if(*p == HCI_ERR_CONN_TIMEOUT){

	}
	else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

	}
	else if(*p == HCI_ERR_CONN_TERM_MIC_FAILURE){

	}
	else{

	}

#if 0 //(0 || BLE_APP_PM_ENABLE)
	 //user has push terminate pkt to ble TX buffer before deepsleep
	if(ble_attr.sendTerminate_before_enterDeep == 1){
		ble_attr.sendTerminate_before_enterDeep = 2;
	}
#endif
	// отключить BLE после соединения если g_zcl_basicAttrs.deviceEnable = fasle
	if(!g_zcl_basicAttrs.deviceEnable) {
		g_zcl_basicAttrs.deviceEnable = TRUE;
		ble_task_stop();
	}// else adv_buf.adv_restore_count = 1;
}

#if 0
void 	ble_exception_data_abandom(u8 e,u8 *p, int n){
	T_bleDataAbandom++;
}
#endif

//_attribute_ram_code_
void	user_set_rf_power (u8 e, u8 *p, int n){
	rf_set_power_level_index(ble_attr.g_ble_txPowerSet);
}



void task_connect(u8 e, u8 *p, int n){

	bls_l2cap_requestConnParamUpdate (DEF_CON_PAR_UPDATE);

	ble_attr.connected = BIT(CONNECTED_FLG_ENABLE);
//	device_in_connection_state = 1;//

//	interval_update_tick = clock_time() | 1; //none zero
}

// bls_app_registerEventCallback (BLT_EV_FLAG_CONN_PARA_REQ, &task_conn_update_req);
//void	task_conn_update_req (u8 e, u8 *p, int n){}

// blc_l2cap_registerConnUpdateRspCb(ble_conn_param_update_response);
int ble_conn_param_update_response(u8 id, u16  result) {
	if (result == CONN_PARAM_UPDATE_ACCEPT)
		ble_attr.connected |= BIT(CONNECTED_FLG_PAR_UPDATE);
	else if (result == CONN_PARAM_UPDATE_REJECT) {
		// TODO: необходимо подбирать другие параметры соединения если внешний адаптер не согласен или плюнуть и послать,
		// что является лучшим решением для OTA, т.к. использовать плохие и сверх медленные адаптеры типа ESP32 для OTA нет смысла.
		bls_l2cap_requestConnParamUpdate(my_periConnParameters.intervalMin, my_periConnParameters.intervalMax, my_periConnParameters.latency, my_periConnParameters.timeout);
	}
	return 0;
}

void blc_newMacAddress(u32 flash_addr, u8 *mac_pub, u8 *mac_rand) {
	u8 mac_flash[8];
	flash_erase_sector(flash_addr);
	memcpy(&mac_flash, mac_pub, 6);
	mac_flash[6] = mac_rand[3];
	mac_flash[7] = mac_rand[4];
	flash_write_page(flash_addr, sizeof(mac_flash), mac_flash);
}

void blc_initMacAddress(u32 flash_addr, u8 *mac_public, u8 *mac_random_static){
	u8 mac_read[8];
	u8 value_rand[5];
	const u8 ff_six_byte[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	flash_read_page(flash_addr, 8, mac_read);
	if (!memcmp(mac_read, ff_six_byte, sizeof(mac_read))) {
		//no public address on flash
		generateRandomNum(sizeof(value_rand), value_rand);
		mac_read[0] = value_rand[0];
		mac_read[1] = value_rand[1];
		mac_read[2] = value_rand[2];
		mac_read[3] = 0x38;             //company id: 0xA4C138
		mac_read[4] = 0xC1;
		mac_read[5] = 0xA4;
		mac_read[6] = value_rand[3];
		mac_read[7] = value_rand[4];
		flash_write_page(flash_addr, sizeof(mac_read), mac_read);
	}

	memcpy(mac_public, mac_read, 6);  //copy public address from flash
	mac_random_static[0] = mac_read[0];
	mac_random_static[1] = mac_read[1];
	mac_random_static[2] = mac_read[2];
	mac_random_static[3] = mac_read[6];
	mac_random_static[4] = mac_read[7];
	mac_random_static[5] = 0xC0; 			//for random static
}

/*
bool ble_connection_doing(void){
	return ble_attr.g_bleConnDoing;
}
*/

int app_host_event_callback (u32 h, u8 *para, int n){
	u8 event = h & 0xFF;

	switch(event)
	{
		case GAP_EVT_SMP_PARING_BEAGIN:
		{
			ble_attr.g_bleConnDoing = 1;
		}
		break;

		case GAP_EVT_SMP_PARING_SUCCESS:
		{
			ble_attr.g_bleConnDoing = 0;
		}
		break;

		case GAP_EVT_SMP_PARING_FAIL:
		{
			ble_attr.g_bleConnDoing = 0;
		}
		break;

#if defined(MTU_SIZE_SETTING)
		case GAP_EVT_SMP_CONN_ENCRYPTION_DONE:
		{
			if(!(ble_attr.connected & BIT(CONNECTED_FLG_MTU_EXCHANGE))) {  //master do not send MTU exchange request in time
				blc_att_requestMtuSizeExchange(BLS_CONN_HANDLE, MTU_SIZE_SETTING);
			}
		}
		break;

		case GAP_EVT_ATT_EXCHANGE_MTU:
		{
			gap_gatt_mtuSizeExchangeEvt_t *pEvt = (gap_gatt_mtuSizeExchangeEvt_t *)para;
//?			T_final_MTU_size = pEvt->effective_MTU;
			ble_attr.connected |= BIT(CONNECTED_FLG_MTU_EXCHANGE);   //set MTU size exchange flag here
		}
		break;
#endif

		default:
		break;
	}

	return 0;
}

#if DEBUG_ENABLE
void app_debug_ota_result(int result) {
	show_small_number((s16)result, 0);
	update_lcd();
#ifdef USE_EPD
	while(task_lcd())
		sleep_us(USE_EPD*1000);
#endif
	sleep_ms(3*1000);
}
#endif

void user_ble_init(bool isRetention){
	ble_attr.sendTerminate_before_enterDeep = 0;
	if(isRetention){
		blc_ll_initBasicMCU();   //mandatory
		rf_set_power_level_index(ble_attr.g_ble_txPowerSet);
		blc_ll_recoverDeepRetention();
	}else {
		bls_smp_configParingSecurityInfoStorageAddr(CFG_NV_START_FOR_BLE);
		 //blc_app_loadCustomizedParameters();  //load customized freq_offset cap value

	////////////////// BLE stack initialization ////////////////////////////////////
//		blc_initMacAddress(CFG_MAC_ADDRESS, mac_public, mac_random_static);

#if(BLE_DEVICE_ADDRESS_TYPE == BLE_DEVICE_ADDRESS_RANDOM_STATIC)
		blc_ll_setRandomAddr(mac_random_static);
#endif

		////// Controller Initialization  //////////
		blc_ll_initBasicMCU();                      //mandatory
		blc_ll_initStandby_module(mac_public);				//mandatory
		blc_ll_initAdvertising_module(mac_public); 	//adv module: 		 mandatory for BLE slave,
		blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
		blc_ll_initPowerManagement_module();        //pm module:      	 optional

#if(BLE_APP_PM_ENABLE)
		blc_ll_initPowerManagement_module();

#if (PM_DEEPSLEEP_RETENTION_ENABLE)
		bls_pm_setSuspendMask(SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
		blc_pm_setDeepsleepRetentionThreshold(40, 18); // ATC_BLE 40, 18
		blc_pm_setDeepsleepRetentionEarlyWakeupTiming(250); // 250 us
		blc_pm_setDeepsleepRetentionType(DEEPSLEEP_MODE_RET_SRAM_LOW32K);
#else
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
#endif // PM_DEEPSLEEP_RETENTION_ENABLE
//		bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_ENTER, &ble_remote_set_sleep_wakeup);
#else
		bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif // BLE_APP_PM_ENABLE
		////// Host Initialization  //////////
		blc_gap_peripheral_init();    //gap initialization
		my_att_init(); //gatt initialization
		blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization

		//Smp Initialization may involve flash write/erase(when one sector stores too much information,
		//   is about to exceed the sector threshold, this sector must be erased, and all useful information
		//   should re_stored) , so it must be done after battery check
#if (APP_SECURITY_ENABLE)
		blc_smp_peripheral_init();
#else
		blc_smp_setSecurityLevel(No_Security);
#endif

		blc_gap_registerHostEventHandler( app_host_event_callback );
		blc_gap_setEventMask( GAP_EVT_MASK_SMP_PARING_BEAGIN
							  | GAP_EVT_MASK_SMP_PARING_SUCCESS
							  | GAP_EVT_MASK_SMP_PARING_FAIL
							  | GAP_EVT_MASK_SMP_CONN_ENCRYPTION_DONE
#if defined(MTU_SIZE_SETTING)
							  | GAP_EVT_MASK_ATT_EXCHANGE_MTU
#endif
							  );



	///////////////////// USER application initialization ///////////////////


		////////////////// config adv packet /////////////////////

		tbl_scanRsp.size = sizeof(tbl_scanRsp.name) + sizeof(tbl_scanRsp.id) ;
		tbl_scanRsp.id = GAP_ADTYPE_LOCAL_NAME_COMPLETE;
#if BOARD == BOARD_MHO_C401
		tbl_scanRsp.name[0] = 'M';
		tbl_scanRsp.name[1] = 'H';
		tbl_scanRsp.name[2] = 'O';
#elif BOARD == BOARD_CGG1
		tbl_scanRsp.name[0] = 'C';
		tbl_scanRsp.name[1] = 'G';
		tbl_scanRsp.name[2] = 'G';
#elif BOARD == BOARD_CGG1N
		tbl_scanRsp.name[0] = 'C';
		tbl_scanRsp.name[1] = 'G';
		tbl_scanRsp.name[2] = 'N';
#elif BOARD == BOARD_CGDK2
		tbl_scanRsp.name[0] = 'C';
		tbl_scanRsp.name[1] = 'G';
		tbl_scanRsp.name[2] = 'D';
#elif BOARD == BOARD_CGGN
		tbl_scanRsp.name[0] = 'C';
		tbl_scanRsp.name[1] = 'G';
		tbl_scanRsp.name[2] = 'G';
#elif BOARD == BOARD_MHO_C401N
		tbl_scanRsp.name[0] = 'M';
		tbl_scanRsp.name[1] = 'H';
		tbl_scanRsp.name[2] = 'O';
#elif BOARD == BOARD_MJWSD05MMC
		tbl_scanRsp.name[0] = 'B';
		tbl_scanRsp.name[1] = 'T';
		tbl_scanRsp.name[2] = 'H';
#elif BOARD == BOARD_LYWSD03MMC
		tbl_scanRsp.name[0] = 'A';
		tbl_scanRsp.name[1] = 'T';
		tbl_scanRsp.name[2] = 'C';
#elif BOARD == BOARD_MHO_C122
		tbl_scanRsp.name[0] = 'M';
		tbl_scanRsp.name[1] = 'H';
		tbl_scanRsp.name[2] = 'O';
#elif BOARD == BOARD_TS0201_TZ3000
		tbl_scanRsp.name[0] = 'T';
		tbl_scanRsp.name[1] = 'S';
		tbl_scanRsp.name[2] = '0';
#elif BOARD == BOARD_TH03Z
		tbl_scanRsp.name[0] = 'T';
		tbl_scanRsp.name[1] = 'H';
		tbl_scanRsp.name[2] = '0';
#elif BOARD == BOARD_ZTH01
		tbl_scanRsp.name[0] = 'Z';
		tbl_scanRsp.name[1] = 'T';
		tbl_scanRsp.name[2] = 'H';
#elif BOARD == BOARD_ZTH02
		tbl_scanRsp.name[0] = 'Z';
		tbl_scanRsp.name[1] = 'T';
		tbl_scanRsp.name[2] = 'H';
#elif BOARD == BOARD_ZTH03
		tbl_scanRsp.name[0] = 'T';
		tbl_scanRsp.name[1] = 'H';
		tbl_scanRsp.name[2] = '3';
#elif BOARD == BOARD_LKTMZL02
		tbl_scanRsp.name[0] = 'Z';
		tbl_scanRsp.name[1] = 'L';
		tbl_scanRsp.name[2] = '2';
#elif BOARD == BOARD_CBS3
		tbl_scanRsp.name[0] = 'C';
		tbl_scanRsp.name[1] = 'B';
		tbl_scanRsp.name[2] = 'S';
#elif BOARD == BOARD_HS09
		tbl_scanRsp.name[0] = 'H';
		tbl_scanRsp.name[1] = 'S';
		tbl_scanRsp.name[2] = '9';
#elif BOARD == BOARD_ZTH05
		tbl_scanRsp.name[0] = 'T';
		tbl_scanRsp.name[1] = 'H';
		tbl_scanRsp.name[2] = '5';
#elif BOARD == BOARD_ZYZTH02
		tbl_scanRsp.name[0] = 'Z';
		tbl_scanRsp.name[1] = 'Y';
		tbl_scanRsp.name[2] = '2';
#elif BOARD == BOARD_ZYZTH01
		tbl_scanRsp.name[0] = 'Z';
		tbl_scanRsp.name[1] = 'Y';
		tbl_scanRsp.name[2] = 'P';
#elif BOARD == BOARD_ZG_227Z
		tbl_scanRsp.name[0] = 'Z';
		tbl_scanRsp.name[1] = 'G';
		tbl_scanRsp.name[2] = '2';
#elif BOARD == BOARD_MJWSD06MMC
		tbl_scanRsp.name[0] = 'M';
		tbl_scanRsp.name[1] = 'J';
		tbl_scanRsp.name[2] = '6';
#elif BOARD == BOARD_ZG303Z
		tbl_scanRsp.name[0] = 'Z';
		tbl_scanRsp.name[1] = 'G';
		tbl_scanRsp.name[2] = '3';
#else
#error "DEVICE_TYPE = ?"
#endif

		tbl_scanRsp.name[3] = '-';
		tbl_scanRsp.name[4] = 'z';
		tbl_scanRsp.name[5] = 'b';
		tbl_scanRsp.name[6] = '-';
		tbl_scanRsp.name[7] = int_to_hex(mac_public[2] >> 4);
		tbl_scanRsp.name[8] = int_to_hex(mac_public[2] & 0x0f);
		tbl_scanRsp.name[9] = int_to_hex(mac_public[1] >> 4);
		tbl_scanRsp.name[10] = int_to_hex(mac_public[1] & 0x0f);
		tbl_scanRsp.name[11] = int_to_hex(mac_public[0] >> 4);
		tbl_scanRsp.name[12] = int_to_hex(mac_public[0] & 0x0f);

		ble_attr.id.pid = CMD_ID_DEV_ID;
		ble_attr.id.revision = 0;
		ble_attr.id.hw_version = g_zcl_basicAttrs.hwVersion;
		ble_attr.id.sw_version = (APP_RELEASE<<8) | APP_BUILD;
		ble_attr.id.dev_spec_data = sensor_ht.sensor_type;
		ble_attr.id.services = DEV_SERVICES;

		bls_ll_setAdvEnable(BLC_ADV_DISABLE);

		bls_set_advertise_prepare(app_advertise_prepare_handler); // TODO: not work if EXTENDED_ADVERTISING

		bls_ll_setScanRspData((u8 *)&tbl_scanRsp, sizeof(tbl_scanRsp));

		bls_ll_setAdvData((u8 *)&adv_buf.data, 3);

		bls_ll_setAdvParam( DEF_ADV_INTERVAL, DEF_ADV_INTERVAL+10,
							ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
							0,  NULL,
							DEF_APP_ADV_CHANNEL,
							ADV_FP_NONE);

		//set rf power index, user must set it after every suspend wakeup, cause relative setting will be reset in suspend
		user_set_rf_power(0, 0, 0);


#if defined(STARTUP_IN_BLE) &&  STARTUP_IN_BLE // Startup in BLE ?
		if(zb_isDeviceFactoryNew())
			startAdvTime(CONNECT_ADV_COUNT, CONNECT_ADV_INTERVAL);
#endif
		//ble event call back
		bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_EXIT, &user_set_rf_power);
		bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
		bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &task_terminate);
	//	bls_app_registerEventCallback (BLT_EV_FLAG_RX_DATA_ABANDOM, &ble_exception_data_abandom);
	//	bls_app_registerEventCallback (BLT_EV_FLAG_CONN_PARA_REQ, &task_conn_update_req);
		blc_l2cap_registerConnUpdateRspCb(ble_conn_param_update_response); //	bls_app_registerEventCallback (BLT_EV_FLAG_CONN_PARA_UPDATE, &task_conn_update_done);

#if USE_BLE_OTA
		////////////////// OTA relative ////////////////////////
		extern u32 mcuBootAddrGet(void);
		extern u32 ota_program_bootAddr;
		// ota_program_offset = ota boot [00844b6c] ota_set_flag 0x4b
		// ota_program_bootAddr = current boot [00843fc8] ota_clr_flag 0
		// ota_firmware_size_k = [00843fd8]
		ota_program_bootAddr = mcuBootAddrGet();
		ota_program_offset = (ota_program_bootAddr) ? 0 : FLASH_ADDR_OF_OTA_IMAGE;
		ota_firmware_size_k = (FLASH_OTA_IMAGE_MAX_SIZE) >> 10;
/*		bls_ota_set_fwSize_and_fwBootAddr(
				((FLASH_OTA_IMAGE_MAX_SIZE) >> 10),
				ota_program_offset); */
		bls_ota_clearNewFwDataArea();
		//ota_program_bootAddr = mcuBootAddrGet();
		bls_ota_registerStartCmdCb(app_enter_ota_mode);
#if DEBUG_ENABLE
		bls_ota_registerResultIndicateCb(app_debug_ota_result);  //debug
#endif
#endif

		///////////////////// Power Management initialization///////////////////
#if(BLE_APP_PM_ENABLE)
		bls_pm_setSuspendMask(SUSPEND_ADV | SUSPEND_CONN);
		//bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &ble_remote_set_sleep_wakeup);
#else
		bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif
	//	advertise_begin_tick = clock_time();
	}
}


