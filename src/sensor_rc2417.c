/**
 * @file sensor_rc2417.c
 * @brief Driver implementation for Skyrelay RC2417FR 24GHz mmWave radar
 *        Features power gating via A19T P-MOSFET and UART AT commands.
 */

#include "tl_common.h"
#if USE_SENSOR_RC2417
#include "app_main.h"
#include "sensor_rc2417.h"

static u8 s_rc2417_powered = 0;

static void rc2417_power_on_hw(void) {
	gpio_set_func(RC2417_PWR_PIN, AS_GPIO);
	gpio_set_output_en(RC2417_PWR_PIN, 1);
	gpio_set_input_en(RC2417_PWR_PIN, 0);
	gpio_write(RC2417_PWR_PIN, 0); // Active LOW: 0 = Radar ON
	s_rc2417_powered = 1;
	sleep_ms(15); // Wait for power stabilization & radar MCU boot
}

static void rc2417_power_off_hw(void) {
	gpio_set_func(RC2417_PWR_PIN, AS_GPIO);
	gpio_set_output_en(RC2417_PWR_PIN, 1);
	gpio_set_input_en(RC2417_PWR_PIN, 0);
	gpio_write(RC2417_PWR_PIN, 1); // Active LOW: 1 = Radar OFF
	s_rc2417_powered = 0;
}

static void rc2417_uart_init(void) {
	uart_gpio_set(UART_TX_PB1, UART_RX_PB7);
	uart_reset();
	// 115200 baud at 24MHz: div=12, bwpc=15 ((12+1)*(15+1) = 208 clock ticks)
	uart_init(12, 15, PARITY_NONE, STOP_BIT_ONE);
	uart_dma_enable(0, 0);
	uart_ndma_clear_tx_index();
}

static void rc2417_uart_deinit(void) {
	uart_reset();
	gpio_set_func(RC2417_UART_TX_PIN, AS_GPIO);
	gpio_set_output_en(RC2417_UART_TX_PIN, 0);
	gpio_set_input_en(RC2417_UART_TX_PIN, 1);
	gpio_set_func(RC2417_UART_RX_PIN, AS_GPIO);
	gpio_set_output_en(RC2417_UART_RX_PIN, 0);
	gpio_set_input_en(RC2417_UART_RX_PIN, 1);
}

static void rc2417_uart_send_byte(u8 byte) {
	uart_ndma_send_byte(byte);
	while(uart_tx_is_busy());
}

static void rc2417_uart_send_str(const char *str) {
	while(*str) {
		rc2417_uart_send_byte((u8)*str++);
	}
}

static void rc2417_uart_send_u32(u32 val) {
	char buf[12];
	char *p = buf + sizeof(buf) - 1;
	*p = '\0';
	if(val == 0) {
		*(--p) = '0';
	} else {
		while(val > 0) {
			*(--p) = '0' + (val % 10);
			val /= 10;
		}
	}
	rc2417_uart_send_str(p);
}

void rc2417_set_cfg(void) {
	if(!s_rc2417_powered)
		return;

	u32 hold = zcl_occupAttr.delay ? zcl_occupAttr.delay : DEF_OCCUPANCY_DELAY;
	u32 sens = zcl_occupAttr.thres ? zcl_occupAttr.thres : DEF_OCCUPANCY_THRES;
	if(sens > 10 && sens <= 255) {
		sens = ((sens * 10) + 128) / 255;
	}
	if(sens < 1) sens = 1;
	if(sens > 10) sens = 10;

	// Set detection hold time (seconds)
	rc2417_uart_send_str("AT+HOLD=");
	rc2417_uart_send_u32(hold);
	rc2417_uart_send_str("\n");

	// Motion threshold
	rc2417_uart_send_str("AT+MR1TH=1000\n");

	// Sensitivity thresholds
	rc2417_uart_send_str("AT+MR2TH=");
	rc2417_uart_send_u32(sens);
	rc2417_uart_send_str("\n");

	rc2417_uart_send_str("AT+R2=");
	rc2417_uart_send_u32(sens);
	rc2417_uart_send_str("\n");

	// Constant False Alarm Rate & filter timings
	rc2417_uart_send_str("AT+CFAR=15\n");
	rc2417_uart_send_str("AT+FTIME=500\n");
	rc2417_uart_send_str("AT+TRITH=1\n");
	rc2417_uart_send_str("AT+TAGOUT=0\n");
	rc2417_uart_send_str("AT+GPIO=0\n");

	// Start radar tracking
	rc2417_uart_send_str("AT+START\n");
}

void rc2417_wake_and_start(void) {
	if(!s_rc2417_powered) {
		rc2417_power_on_hw();
		rc2417_uart_init();
		rc2417_set_cfg();
	} else {
		rc2417_uart_send_str("AT+START\n");
	}
}

void rc2417_go_sleep(void) {
	if(s_rc2417_powered) {
		rc2417_uart_send_str("AT+SLEEP\n");
		sleep_ms(2);
		rc2417_uart_deinit();
		rc2417_power_off_hw();
	}
}

u8 rc2417_read_out(void) {
	if(!s_rc2417_powered)
		return 0;
	return gpio_read(RC2417_OUT_PIN) ? 1 : 0;
}

u8 rc2417_is_powered(void) {
	return s_rc2417_powered;
}

void rc2417_init(void) {
	// Configure power gate pin (PB4) as output, set HIGH (OFF)
	gpio_set_func(RC2417_PWR_PIN, AS_GPIO);
	gpio_set_input_en(RC2417_PWR_PIN, 0);
	gpio_set_output_en(RC2417_PWR_PIN, 1);
	gpio_write(RC2417_PWR_PIN, 1); // Radar unpowered at cold boot

	// Configure radar digital detection output pin (PB6) as input
	gpio_set_func(RC2417_OUT_PIN, AS_GPIO);
	gpio_set_output_en(RC2417_OUT_PIN, 0);
	gpio_set_input_en(RC2417_OUT_PIN, 1);

	// Configure UART pins into safe low-power state
	rc2417_uart_deinit();

	s_rc2417_powered = 0;
}

#endif // USE_SENSOR_RC2417
