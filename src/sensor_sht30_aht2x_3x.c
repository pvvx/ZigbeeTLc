
#include "tl_common.h"




#if defined(SENSOR_TYPE) && SENSOR_TYPE == SENSOR_SHTAHT2X3X
#include "app_cfg.h"
#include "device.h"
#include "chip_8258/timer.h"

#include "i2c_drv.h"
#include "device.h"
#include "sensors.h"

#define _SENSOR_SPEED_CODE_SEC_ _attribute_ram_code_sec_

#ifndef USE_SENSOR_ID
#warning "SET USE_SENSOR_ID!"
#define USE_SENSOR_ID	0
#endif

u8 sht_sensor_i2c_addr;
u8 aht_sensor_i2c_addr;

u32 aht_sensor_id; //  = (0x20<<16) | (AHT2x_I2C_ADDR << 8);
u32 sht_sensor_id;

measured_data_t measured_data={
	.temp = 0x8000,  // in 0.01 C
	.temp2 = 0x8000,  // in 0.01 C
};

//_attribute_ram_code_
static u8 aht_sensor_crc(u8 * msg, int len) {
	int i,x;
	u8 crc = 0xff;
	for(x = 0; x < len; x++) {
		crc ^= msg[x];
		for(i = 8; i > 0; i--) {
			if (crc & 0x80)
				crc = (crc << 1) ^ (CRC_POLYNOMIAL & 0xff);
			else
				crc = (crc << 1);
		}
	}
	return crc;
}


//_attribute_ram_code_
int aht_start_measure_sensor(void) {
	const u8 data[3] = {AHT2x_CMD_TMS, AHT2x_DATA1_TMS, AHT2x_DATA2_TMS};
	return send_i2c_bytes(aht_sensor_i2c_addr, (u8 *)data, sizeof(data));
}



void aht_check_sensor(void) {
	aht_sensor_i2c_addr = scan_i2c_addr(AHT2x_I2C_ADDR << 1);
	if(aht_sensor_i2c_addr) {
		aht_start_measure_sensor();
	}
}



__attribute__((optimize("-Os"))) int aht_read_sensor(void) {
	u32 _temp;
	u8 data[7];
	//

	//printf("aht_read_sensor: aht_sensor_i2c_addr:%d\n",aht_sensor_i2c_addr);
	battery_detect();
	if (aht_sensor_i2c_addr != 0) {
		if (read_i2c_bytes(aht_sensor_i2c_addr, data, sizeof(data)) == 0
	    && (data[0] & 0x80) == 0
	    && aht_sensor_crc(data, sizeof(data)) == 0) {
			_temp = (data[3] & 0x0F) << 16 | (data[4] << 8) | data[5];
			measured_data.temp2 = ((u32)(_temp * 1250) >> 16) - 5000 + g_zcl_thermostatUICfgAttrs.temp_offset; // x 0.01 C
			/*measured_data.humi = ((u32)(_temp * 1250) >> 16) - 5000 + g_zcl_thermostatUICfgAttrs.temp_offset; // x 0.01 C
			_humi = (data[1] << 12) | (data[2] << 4) | (data[3] >> 4);
			measured_data.humi = ((u32)(_humi * 625) >> 16) + g_zcl_thermostatUICfgAttrs.humi_offset; // x 0.01 %
			if (measured_data.humi < 0)
				measured_data.humi = 0;
			else if (measured_data.humi > 9999)
				measured_data.humi = 9999;
			*/
			// measured_data.count++;
			if(!aht_start_measure_sensor())
				return 1;
		} else {
			send_i2c_byte(aht_sensor_i2c_addr, AHT2x_CMD_RST); // Soft reset command
			aht_sensor_i2c_addr = 0;
			return 0;
		}
	}
	aht_check_sensor();
	return 0;
}

void aht_init_sensor(void) {
	send_i2c_byte(0, 0x06); // Reset command using the general call address
	pm_wait_ms(AHT2x_SENSOR_POWER_TIMEOUT_ms);
	aht_sensor_i2c_addr = scan_i2c_addr(AHT2x_I2C_ADDR << 1);
	if(aht_sensor_i2c_addr) {
		//read_i2c_bytes(sensor_i2c_addr, (u8 *)&sensor_id, 1);
		send_i2c_byte(aht_sensor_i2c_addr, AHT2x_CMD_RST); // Soft reset command
		pm_wait_ms(AHT2x_SENSOR_RESET_TIMEOUT_ms);
		send_i2c_byte(aht_sensor_i2c_addr, AHT2x_CMD_RST); // Soft reset command
		pm_wait_ms(AHT2x_SENSOR_RESET_TIMEOUT_ms);
		if(!aht_start_measure_sensor()) {
			pm_wait_ms(AHT2x_SENSOR_MEASURING_TIMEOUT_ms);
			aht_read_sensor();
		} else {
			aht_sensor_i2c_addr = 0;
		}
	}
}

_SENSOR_SPEED_CODE_SEC_
static u8 sht_sensor_crc(u8 crc) {
	int i;
	for(i = 8; i > 0; i--) {
		if (crc & 0x80)
			crc = (crc << 1) ^ (CRC_POLYNOMIAL & 0xff);
		else
			crc = (crc << 1);
	}
	return crc;
}

static void sht_send_sensor_word(u16 cmd) {
	unsigned char r = irq_disable();
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	reg_i2c_id = sht_sensor_i2c_addr;
	reg_i2c_adr_dat = cmd;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_STOP;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	irq_restore(r);
}


static void sht_check_sensor(void) {
	sht_sensor_i2c_addr = (u8) scan_i2c_addr(SHT30_I2C_ADDR_A << 1);
	if(!sht_sensor_i2c_addr)
		sht_sensor_i2c_addr = (u8) scan_i2c_addr(SHT30_I2C_ADDR_B << 1);
	if(sht_sensor_i2c_addr) {
#if USE_SENSOR_ID
		sht_sensor_id = (0x30<<16) | sht_sensor_i2c_addr;
#endif
		sht_send_sensor_word(SHT30_SOFT_RESET); // Soft reset command
		sleep_us(SHT30_SOFT_RESET_us);
		sht_send_sensor_word(SHT30_HIMEASURE); // start measure T/H
	}
}

void sht_init_sensor(void) {
	send_i2c_byte(0, 0x06); // Reset command using the general call address
	sleep_us(SHT30_POWER_TIMEOUT_us);
	sht_check_sensor();
}




_SENSOR_SPEED_CODE_SEC_
__attribute__((optimize("-Os")))
int sht_read_sensor_cb(void) {
	u16 _temp;
	u16 _humi;
	u8 data, crc; // calculated checksum
	int i;
	if (sht_sensor_i2c_addr == 0) {
		sht_check_sensor();
		return 0;
	}
	reg_i2c_id = sht_sensor_i2c_addr | FLD_I2C_WRITE_READ_BIT;
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
			crc = sht_sensor_crc(data ^ 0xff);
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			_temp |= data;
			crc = sht_sensor_crc(crc ^ data);
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
				measured_data.temp = ((s32)(17500 * _temp) >> 16) - 4500 + g_zcl_thermostatUICfgAttrs.temp_offset; // x 0.01 C
				//measured_data.humi = ((u32)(10000 * _humi) >> 16) + g_zcl_thermostatUICfgAttrs.humi_offset; // x 0.01 %
				//if (measured_data.humi < 0) measured_data.humi = 0;
				//else if (measured_data.humi > 9999) measured_data.humi = 9999;
				//measured_data.count++;
				sht_send_sensor_word(SHT30_HIMEASURE); // start measure T/H
				return 1;
			}
		}
		irq_restore(r);
	} while (i--);
	sht_check_sensor();
	return 0;
}
int sht_read_sensor(void) {
	int re = 0;
	if (sht_sensor_i2c_addr) {
		re = sht_read_sensor_cb();
		battery_detect();
	} else
		sht_init_sensor();
	return re;
}

int read_sensor(void) {
	int re1 = 0;
	int re2=0;
	//printf("\nread_sensor in : aht_sensor_i2c_addr:%d, sht_sensori2c_addr:%d\n",aht_sensor_i2c_addr,sht_sensor_i2c_addr);
	//printf ("read_sensor in temp1:%d,temp2:%d\n",measured_data.temp,measured_data.temp2);
	re1=aht_read_sensor();
	if (sht_sensor_i2c_addr) {
			re2 = sht_read_sensor_cb();
			battery_detect();
		} else
			sht_init_sensor();
	//printf ("read_sensor out re1:%d,re2:%d, temp1:%d,temp2:%d\n",re1,re2,measured_data.temp,measured_data.temp2);
	return re1||re2 ;
}

void init_sensor(void) {
	aht_init_sensor();
	//sht_init_sensor();
}


#endif // defined(SENSOR_TYPE) && SENSOR_TYPE == SENSOR_SHTAHT2X3X

