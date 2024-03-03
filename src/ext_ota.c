/*
 * ext_ota.c
 *
 *  Created on: 7 нояб. 2023 г.
 *      Author: pvvx
 */
#include "tl_common.h"


#define ID_BOOTABLE 0x544c4e4b

#define OTA1_FADDR 0x00000
#define OTA2_FADDR 0x20000
#define BIG_OTA1_FADDR 0x00000 // Big OTA1
#define BIG_OTA2_FADDR 0x40000 // Big OTA2

extern unsigned char *_bin_size_;

// Current OTA header:
static const u32 head_id[3] = {
			0x00008058 | ((FILE_VERSION & 0xffff)<<16), // asm("tj __reset")
			0x025d0000 | ((FILE_VERSION >> 16) & 0xffff), // id OTA ver
			ID_BOOTABLE};

#if ZIGBEE_TUYA_OTA

/* Reformat low OTA to big OTA, Zigbee BootLoader to Zigbee no BootLoader */
_attribute_ram_code_
void tuya_zigbee_ota(void) {
	// find the real FW flash address
	u32 id = 0;
	u32 size;
	u32 faddrr = OTA2_FADDR;
	u32 faddrx = 0;
	u32 faddrw = BIG_OTA2_FADDR;
	u32 buf_blk[64];
	flash_write_status(0, 0);
	flash_read_page(faddrr, 0x20, (unsigned char *) &buf_blk);
	if(memcmp(&buf_blk, &head_id, sizeof(head_id))) {
		faddrr = 0x8000;
		flash_read_page(faddrr, 0x20, (unsigned char *) &buf_blk);
		if(memcmp(&buf_blk, &head_id, sizeof(head_id)) == 0)
			faddrx = faddrr;
	} else
		faddrx = faddrr;

	if(faddrx) {
		// calculate size OTA
		size = (u32)(&_bin_size_);
		size += 15;
		size &= ~15;
		size += 4;
		if(buf_blk[6] == size) { // OTA bin size
			flash_erase_sector(faddrw); // 45 ms, 4 mA
			flash_read_page(faddrr, sizeof(buf_blk), (unsigned char *) &buf_blk);
			buf_blk[2] &= 0xffffffff; // clear id "bootable"
			faddrr += sizeof(buf_blk);
			flash_write_page(faddrw, sizeof(buf_blk), (unsigned char *) &buf_blk);
			size += faddrw;
			faddrw += sizeof(buf_blk);
			while(faddrw < size) {
				if((faddrw & (FLASH_SECTOR_SIZE - 1)) == 0)
					flash_erase_sector(faddrw); // 45 ms, 4 mA
				// rd-wr 4kB - 20 ms, 4 mA
				flash_read_page(faddrr, sizeof(buf_blk), (unsigned char *) &buf_blk);
				faddrr += sizeof(buf_blk);
				flash_write_page(faddrw, sizeof(buf_blk), (unsigned char *) &buf_blk);
				faddrw += sizeof(buf_blk);
			}
			// set id "bootable" to new segment
			id = head_id[2]; // = "KNLT"
			flash_write_page(BIG_OTA2_FADDR+8, sizeof(id), (unsigned char *) &id);
			// clear the "bootable" identifier on the current OTA segment
			id = 0;
			flash_write_page(faddrx + 8, 1, (unsigned char *) &id);
			if(faddrx == 0x8000)
				flash_write_page(8, 1, (unsigned char *) &id);
//			flash_erase_sector(CFG_ADR_BIND); // Pair & Security info
			while(1)
				SYSTEM_RESET();
		}
	}
}

#else

void tuya_zigbee_ota(void) {
	// find the real FW flash address
	u32 id;
	u32 size;
	u32 faddrr = OTA2_FADDR;
	u32 faddrx;
	u32 faddrw = BIG_OTA2_FADDR;
	u32 buf_blk[64];
	flash_write_status(0, 0);
	flash_read_page(faddrr, 0x20, (unsigned char *) &buf_blk);
	if(memcmp(&buf_blk, &head_id, sizeof(head_id)) == 0) {
		faddrx = faddrr + 8;
		// calculate size OTA
		size = (u32)(&_bin_size_);
		size += 15;
		size &= ~15;
		size += 4;
		if(buf_blk[6] == size) { // OTA bin size
			flash_erase_sector(faddrw); // 45 ms, 4 mA
			flash_read_page(faddrr, sizeof(buf_blk), (unsigned char *) &buf_blk);
			buf_blk[2] &= 0xffffffff; // clear id "bootable"
			faddrr += sizeof(buf_blk);
			flash_write_page(faddrw, sizeof(buf_blk), (unsigned char *) &buf_blk);
			size += faddrw;
			faddrw += sizeof(buf_blk);
			while(faddrw < size) {
				if((faddrw & (FLASH_SECTOR_SIZE - 1)) == 0)
					flash_erase_sector(faddrw); // 45 ms, 4 mA
				// rd-wr 4kB - 20 ms, 4 mA
				flash_read_page(faddrr, sizeof(buf_blk), (unsigned char *) &buf_blk);
				faddrr += sizeof(buf_blk);
				flash_write_page(faddrw, sizeof(buf_blk), (unsigned char *) &buf_blk);
				faddrw += sizeof(buf_blk);
			}
			// set id "bootable" to new segment
			id = head_id[2]; // = "KNLT"
			flash_write_page(BIG_OTA2_FADDR+8, sizeof(id), (unsigned char *) &id);
			// clear the "bootable" identifier on the current OTA segment
			id = 0;
			flash_write_page(faddrx, 1, (unsigned char *) &id);
//			flash_erase_sector(CFG_ADR_BIND); // Pair & Security info
			while(1)
				SYSTEM_RESET();
		}
	}
}

#endif
