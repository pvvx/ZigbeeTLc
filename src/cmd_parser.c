#include "tl_common.h"
#include "device.h"
#include "stack/ble/ble.h"
#include "ble_cfg.h"
#include "flash.h"
#include "i2c_drv.h"
#include "device.h"
#include "sensors.h"
#include "cmd_parser.h"
#include "zcl_thermostat_ui_cfg.h"

//#define _flash_read(faddr,len,pbuf) flash_read_page(FLASH_BASE_ADDR + (u32)faddr, len, (u8 *)pbuf)

__attribute__((optimize("-Os")))
int cmd_parser(void * p) {
	u8 send_buf[ATT_MTU_SIZE];
	rf_packet_att_data_t *req = (rf_packet_att_data_t*) p;
	u32 len = req->l2cap - 3;
	if (len) {
		len--;
		u8 cmd = req->dat[0];
		send_buf[0] = cmd;
		send_buf[1] = 0; // no err
		u32 olen = 0;
		if (cmd == CMD_ID_DEV_ID) { // Get DEV_ID
			memcpy(send_buf, &ble_attr.id, sizeof(dev_id_t));
			olen = sizeof(dev_id_t) + 1;
		} else if (cmd == CMD_ID_DEV_MAC) { // Get/Set mac
			if (len == 1 && req->dat[1] == 0) { // default MAC
				flash_erase_sector(CFG_ADR_MAC);
				blc_initMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);
				ble_attr.connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
			} else if (len == sizeof(mac_public)+1 && req->dat[1] == sizeof(mac_public)) {
				if (memcmp(mac_public, &req->dat[2], sizeof(mac_public))) {
					memcpy(mac_public, &req->dat[2], sizeof(mac_public));
					mac_random_static[0] = mac_public[0];
					mac_random_static[1] = mac_public[1];
					mac_random_static[2] = mac_public[2];
					generateRandomNum(2, &mac_random_static[3]);
					mac_random_static[5] = 0xC0; 			//for random static
					blc_newMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);
					ble_attr.connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
				}
			} else	if (len == sizeof(mac_public)+1+2 && req->dat[1] == sizeof(mac_public)+2) {
				if (memcmp(mac_public, &req->dat[2], sizeof(mac_public))
						|| mac_random_static[3] != req->dat[2+6]
						|| mac_random_static[4] != req->dat[2+7] ) {
					memcpy(mac_public, &req->dat[2], sizeof(mac_public));
					mac_random_static[0] = mac_public[0];
					mac_random_static[1] = mac_public[1];
					mac_random_static[2] = mac_public[2];
					mac_random_static[3] = req->dat[2+6];
					mac_random_static[4] = req->dat[2+7];
					mac_random_static[5] = 0xC0; 			//for random static
					blc_newMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);
					ble_attr.connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
				}
			}
			send_buf[1] = 8;
			flash_read_page(CFG_ADR_MAC, 8, &send_buf[2]); // MAC[6] + mac_random[2]
			olen = 8 + 2;
#if USE_CHG_NAME
		} else if (cmd == CMD_ID_ZB_NAME) { // Get/Set zb device name
			if(len && req->dat[1] >= ' ') {
				if(len >= ZCL_BASIC_MAX_LENGTH)
					len = ZCL_BASIC_MAX_LENGTH - 1;
				memcpy(&g_zcl_basicAttrs.modelId[1], &req->dat[1], len);
				g_zcl_basicAttrs.modelId[0] = len;
				save_dev_name(ZCL_ATTRID_BASIC_MODEL_ID);
			}
			olen = min(g_zcl_basicAttrs.modelId[0], (ATT_MTU_SIZE - 4));
			memcpy(&send_buf[1], &g_zcl_basicAttrs.modelId[1], olen);
			olen++;
		} else if (cmd == CMD_ID_ZB_MANU) { // Get/Set zb device man.name
			if(len && req->dat[1] >= ' ') {
				if(len >= ZCL_BASIC_MAX_LENGTH)
					len = ZCL_BASIC_MAX_LENGTH - 1;
				memcpy(&g_zcl_basicAttrs.modelId[1], &req->dat[1], len);
				g_zcl_basicAttrs.manuName[0] = len;
				save_dev_name(ZCL_ATTRID_BASIC_MFR_NAME);
			}
			olen = min(g_zcl_basicAttrs.manuName[0], (ATT_MTU_SIZE - 4));
			memcpy(&send_buf[1], &g_zcl_basicAttrs.manuName[1], olen);
			olen++;
#endif
#ifdef ZCL_THERMOSTAT_UI_CFG
		} else if (cmd == CMD_ID_ZB_CFG) {
			if(len) {
				if(len > sizeof(g_zcl_thermostatUICfgAttrs))
					len = sizeof(g_zcl_thermostatUICfgAttrs);
				memcpy(&g_zcl_thermostatUICfgAttrs, &req->dat[1], len);
				zcl_thermostatConfig_save();
			}
			memcpy(&send_buf[1], &g_zcl_thermostatUICfgAttrs, sizeof(g_zcl_thermostatUICfgAttrs));
			olen = sizeof(g_zcl_thermostatUICfgAttrs) + 1;
		} else if (cmd == CMD_ID_ZB_DEF) {
			memcpy(&g_zcl_thermostatUICfgAttrs, &g_zcl_thermostatUICfgDefault, sizeof(g_zcl_thermostatUICfgAttrs));
			zcl_thermostatConfig_save();
#endif
#if (DEV_SERVICES & SERVICE_PINCODE)
		} else if (cmd == CMD_ID_PINCODE && len >= 4) { // Set new pinCode 0..999999
			u32 old_pincode = pincode;
			u32 new_pincode = req->dat[1] | (req->dat[2]<<8) | (req->dat[3]<<16) | (req->dat[4]<<24);
			if (pincode != new_pincode) {
				pincode = new_pincode;
				if (flash_write_cfg(&pincode, EEP_ID_PCD, sizeof(pincode))) {
					if ((pincode != 0) ^ (old_pincode != 0)) {
						bls_smp_eraseAllParingInformation();
						wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
					}
					send_buf[1] = 1;
				} else	send_buf[1] = 3;
			} //else send_buf[1] = 0;
			olen = 2;
#endif
#if defined(I2C_GROUP) || defined(I2C_SDA)
		} else if (cmd == CMD_ID_I2C_SCAN) {   // Universal I2C/SMBUS read-write
			len = 0;
			olen = 1;
			while(len < 0x100 && olen < SEND_BUFFER_SIZE) {
				send_buf[olen] = (u8)scan_i2c_addr(len);
				if(send_buf[olen])
					olen++;
				len += 2;
			}
		} else if (cmd == CMD_ID_I2C_UTR) {   // Universal I2C/SMBUS read-write
			i2c_utr_t * pbufi = (i2c_utr_t *)&req->dat[1];
			olen = pbufi->rdlen & 0x7f;
			if(len >= sizeof(i2c_utr_t)
				&& olen <= SEND_BUFFER_SIZE - 3 // = 17
				&& I2CBusUtr(&send_buf[3],
						pbufi,
						len - sizeof(i2c_utr_t)) == 0 // wrlen: - addr
						)  {
				send_buf[1] = len - sizeof(i2c_utr_t); // write data len
				send_buf[2] = pbufi->wrdata[0]; // i2c addr
				olen += 3;
			} else {
				send_buf[1] = 0xff; // Error cmd
				olen = 2;
			}
#endif
		} else if (cmd == CMD_ID_FLASH_ID) { // Get Flash JEDEC ID
			extern void flash_mspi_read_ram(unsigned char cmd, unsigned long addr, unsigned char addr_en, unsigned char dummy_cnt, unsigned char *data, unsigned long data_len);
			flash_mspi_read_ram(FLASH_GET_JEDEC_ID, 0, 0, 0, (unsigned char*)(&send_buf[1]), 3);
			olen = 1 + 3;

		// Debug commands (unsupported in different versions!):

		} else if (cmd == CMD_ID_DEBUG && len >= 3) { // test/debug
			flash_read_page((req->dat[1] | (req->dat[2]<<8) | (req->dat[3]<<16)), 18, &send_buf[4]);
			memcpy(send_buf, &req->dat, 4);
			olen = 18+4;
		} else {
			send_buf[1] = 0xff; // Error cmd
			olen = 2;
		}
		if (olen)
			bls_att_pushNotifyData(RxTx_CMD_Data, send_buf, olen);
	}
	return 0;
}
