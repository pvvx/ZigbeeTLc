#include "tl_common.h"
#if defined(SENSOR_TYPE) && SENSOR_TYPE == SENSOR_SHT30
#include "chip_8258/timer.h"

#include "i2c_drv.h"
#include "device.h"
#include "sensors.h"

#define _SENSOR_SPEED_CODE_SEC_ _attribute_ram_code_sec_

#define SHT30_POWER_TIMEOUT_us	1500	// time us
#define SHT30_SOFT_RESET_us		1500	// time us
#define SHT30_HI_MEASURE_us		15500	// time us
#define SHT30_LO_MEASURE_us		4500	// time us

#define SHT30_I2C_ADDR_A	0x44
#define SHT30_I2C_ADDR_B	0x45
#define SHT30_SOFT_RESET	0xA230 // Soft reset command
#define SHT30_HIMEASURE		0x0024 // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
#define SHT30_HIMEASURE_CS	0x062C // Measurement commands, Clock Stretching, Normal Mode, Read T First
#define SHT30_LPMEASURE		0x1624 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First
#define SHT30_LPMEASURE_CS	0x102C // Measurement commands, Clock Stretching, Low Power Mode, Read T First

#define CRC_POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

#ifndef USE_SENSOR_ID
#error "Define USE_SENSOR_ID!"
#endif

u8 sensor_i2c_addr;
//u8 sensor_version;
#if USE_SENSOR_ID
u32 sensor_id;
#endif

measured_data_t measured_data;

static void send_sensor_word(u16 cmd) {
	unsigned char r = irq_disable();
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	reg_i2c_id = sensor_i2c_addr;
	reg_i2c_adr_dat = cmd;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_STOP;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	irq_restore(r);
}

_SENSOR_SPEED_CODE_SEC_
static u8 sensor_crc(u8 crc) {
	int i;
	for(i = 8; i > 0; i--) {
		if (crc & 0x80)
			crc = (crc << 1) ^ (CRC_POLYNOMIAL & 0xff);
		else
			crc = (crc << 1);
	}
	return crc;
}

static void check_sensor(void) {
	sensor_i2c_addr = (u8) scan_i2c_addr(SHT30_I2C_ADDR_A << 1);
	if(!sensor_i2c_addr)
		sensor_i2c_addr = (u8) scan_i2c_addr(SHT30_I2C_ADDR_B << 1);
	if(sensor_i2c_addr) {
#if USE_SENSOR_ID
		sensor_id = (0x30<<16) | sensor_i2c_addr;
#endif
		send_sensor_word(SHT30_SOFT_RESET); // Soft reset command
		sleep_us(SHT30_SOFT_RESET_us);
		send_sensor_word(SHT30_HIMEASURE); // start measure T/H
	}
}

void init_sensor(void) {
	send_i2c_byte(0, 0x06); // Reset command using the general call address
	sleep_us(SHT30_POWER_TIMEOUT_us);
	check_sensor();
}

_SENSOR_SPEED_CODE_SEC_
__attribute__((optimize("-Os")))
int read_sensor_cb(void) {
	u16 _temp;
	u16 _humi;
	u8 data, crc; // calculated checksum
	int i;
	if (sensor_i2c_addr == 0) {
		check_sensor();
		return 0;
	}
	reg_i2c_id = sensor_i2c_addr | FLD_I2C_WRITE_READ_BIT;
	i = 128;
	do {
		unsigned char r = irq_disable();
		if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
		reg_i2c_ctrl = FLD_I2C_CMD_ID | FLD_I2C_CMD_START;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		if (reg_i2c_status & FLD_I2C_NAK) {
			reg_i2c_ctrl = FLD_I2C_CMD_STOP;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		} else { // ACK ok
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			_temp = data << 8;
			crc = sensor_crc(data ^ 0xff);
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			_temp |= data;
			crc = sensor_crc(crc ^ data);
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			_humi = reg_i2c_di << 8;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID | FLD_I2C_CMD_ACK;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			_humi |= reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_STOP;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			if (crc == data && _temp != 0xffff) {
				irq_restore(r);
				measured_data.temp = ((s32)(17500 * _temp) >> 16) - 4500 + g_zcl_thermostatUICfgAttrs.temp_offset * 10; // x 0.01 C
				measured_data.humi = ((u32)(10000 * _humi) >> 16) + g_zcl_thermostatUICfgAttrs.humi_offset * 10; // x 0.01 %
				if (measured_data.humi < 0) measured_data.humi = 0;
				else if (measured_data.humi > 9999) measured_data.humi = 9999;
				//measured_data.count++;
				send_sensor_word(SHT30_HIMEASURE); // start measure T/H
				return 1;
			}
		}
		irq_restore(r);
	} while (i--);
	check_sensor();
	return 0;
}

int read_sensor(void) {
	int re = 0;
	if (sensor_i2c_addr) {
		re = read_sensor_cb();
		battery_detect();
	} else
		init_sensor();
	return re;
}


#endif //  defined(SENSOR_TYPE) && SENSOR_TYPE == SENSOR_SHTC3_4X
