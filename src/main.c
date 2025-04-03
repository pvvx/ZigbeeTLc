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

/*
 * main:
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


