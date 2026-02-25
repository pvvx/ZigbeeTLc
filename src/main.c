/********************************************************************************************************
 * @file    main.c
 *
 *
 *******************************************************************************************************/
#include "tl_common.h"
#include "zb_common.h"
#include "device.h"
#include "ext_ota.h"
#if USE_BLE
#include "stack/ble/ble_config.h"
#include "stack/ble/ble_common.h"
#include "stack/ble/ble.h"
#include "zigbee_ble_switch.h"
#include "zcl_include.h"
#include "app_ui.h"
#include "ble_cfg.h"
#endif

#define ID_BOOTABLE 0x544c4e4b

#define OTA1_FADDR 				0x000000
#define OTA2_FADDR 				0x020000
#define BIG_OTA1_FADDR 			0x000000 // Big OTA1
#define BIG_OTA2_FADDR 			0x040000 // Big OTA2
#define ZIGBEE_BOOT_OTA_FADDR	0x008000
#define ZIGBEE_MAC_FADDR		0x0ff000
#define BIN_SIZE_MAX_OTA		0x036000
#define BLE_MAC_FADDR			0x076000

static const u32 flag_addr_ok = 0x33CC55AA;

/* Reformat low OTA to big OTA, Zigbee BootLoader to Zigbee no BootLoader */

_attribute_ram_code_
void tuya_zigbee_ota(void) {
	u32 id = ID_BOOTABLE;
	u32 size;
	u32 faddrr = ZIGBEE_BOOT_OTA_FADDR;	// 0x008000
	u32 faddrw = BIG_OTA1_FADDR;	// 0x000000
	u32 buf_blk[64];	// max 256 bytes
	flash_write_status(0, 0);
	// search for start firmware address 0x008000 or 0x020000 ?
	flash_read_page(faddrr, 16, (unsigned char *) &buf_blk);
	if(buf_blk[2] != id) { // 0x008000 != bootable
		faddrr = OTA2_FADDR;
		flash_read_page(faddrr, 16, (unsigned char *) &buf_blk);
		if(buf_blk[2] != id) // 0x020000 != bootable
			return;
	}
	// Run time: ~3700 ms
	// faddrr: 0x008000 == bootable || 0x020000 == bootable
	flash_read_page(faddrr, sizeof(buf_blk), (unsigned char *) &buf_blk);
	if(buf_blk[2] == id && buf_blk[6] > FLASH_SECTOR_SIZE && buf_blk[6] < BIN_SIZE_MAX_OTA) {
		buf_blk[2] &= 0xffffffff; // clear id "bootable"
		size = buf_blk[6];
		size += FLASH_SECTOR_SIZE - 1;
		size &= ~(FLASH_SECTOR_SIZE - 1);
		size += faddrw;
		flash_erase_sector(faddrw); // 45 ms, 4 mA
		flash_write_page(faddrw, sizeof(buf_blk), (unsigned char *) &buf_blk);
		faddrr += sizeof(buf_blk);
		// size += faddrw;
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
		flash_write_page(BIG_OTA1_FADDR+8, sizeof(id), (unsigned char *) &id);
		while(1)
			reg_pwdn_ctrl = BIT(5);
	}
}

/*
 * flash_main:
 * */
int flash_main(void)
{
	u8 isRetention = (drv_platform_init() == SYSTEM_DEEP_RETENTION) ? 1 : 0;

	os_init(isRetention);

	/* reduce power consumption, disable CLK disable CLK of unused peripherals*/

	reg_clk_en0 = 0 // FLD_CLK0_SPI_EN
#if UART_PRINTF_MODE
			| FLD_CLK0_UART_EN
#endif
			| FLD_CLK0_SWIRE_EN;
#if PA_ENABLE
	rf_paInit(PA_TX, PA_RX);
#endif

#if USE_BLE

	user_init(isRetention);
	ble_radio_init();
	user_ble_init(isRetention);

	if(CURRENT_SLOT_GET() == DUALMODE_SLOT_ZIGBEE){
		switch_to_zb_context();
	}

	irq_enable(); // drv_enable_irq();

#if (MODULE_WATCHDOG_ENABLE)
	drv_wd_setInterval(600);
    drv_wd_start();
#endif

    while(1) {
#if (MODULE_WATCHDOG_ENABLE)
		drv_wd_clear();
#endif

#if USE_BLE_OTA
		if(!ble_attr.ota_is_working)
#endif
			sensors_task(NULL);
		concurrent_mode_main_loop();
		task_keys();
#if PM_ENABLE
		app_pm_task();
#endif
	}

#else // USE_BLE

	user_init(isRetention);

	irq_enable(); // drv_enable_irq();

#if (MODULE_WATCHDOG_ENABLE)
	drv_wd_setInterval(600);
    drv_wd_start();
#endif

	while(1) {
    	ev_main();

#if (MODULE_WATCHDOG_ENABLE)
		drv_wd_clear();
#endif

		tl_zbTaskProcedure();

#if	(MODULE_WATCHDOG_ENABLE)
		drv_wd_clear();
#endif
	}
#endif // USE_BLE

	return 0;
}

/*
 * main:
 * */
_attribute_ram_code_
int main(void) {
	// Проверка на старт из Tuya boot_loder (старт с 0x20000 не проверяется)
	if(*(u32 *)(ZIGBEE_BOOT_OTA_FADDR + 8) == ID_BOOTABLE
	|| flag_addr_ok != 0x33CC55AA) {
		// clock_init(SYS_CLK_24M_Crystal);
		tuya_zigbee_ota();
	}
	return flash_main();
}

