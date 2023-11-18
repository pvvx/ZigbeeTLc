#include "tl_common.h"
#if defined(SENSOR_TYPE) && SENSOR_TYPE == SENSOR_SHTXX
#include "app_cfg.h"
#include "chip_8258/timer.h"

#include "i2c_drv.h"
#include "device.h"
#include "sensors.h"

// Sensor SHTC3 https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_SHTC3_Datasheet.pdf
//#define SHTC3_I2C_ADDR		0x70
#define SHTC3_WAKEUP		0x1735 // Wake-up command of the sensor
#define SHTC3_WAKEUP_us		240    // time us
#define SHTC3_SOFT_RESET	0x5d80 // Soft reset command
#define SHTC3_SOFT_RESET_us	240    // time us
#define SHTC3_GO_SLEEP		0x98b0 // Sleep command of the sensor
#define SHTC3_MEASURE		0x6678 // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
#define SHTC3_MEASURE_us	11000  // SHTV3 11 ms
#define SHTC3_MEASURE_CS	0xA27C // Measurement commands, Clock Stretching, Normal Mode, Read T First
#define SHTC3_LPMEASURE		0x9C60 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First
#define SHTC3_LPMEASURE_CS	0x245C // Measurement commands, Clock Stretching, Low Power Mode, Read T First
#define SHTC3_GET_ID		0xC8EF // read ID register

// Sensor SHT4x https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_Datasheet.pdf
//#define SHT4x_I2C_ADDR		0x44
#define SHT4x_SOFT_RESET	0x94 // Soft reset command
#define SHT4x_SOFT_RESET_us	900  // max 1 ms
#define SHT4x_MEASURE_HI	0xFD // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
#define SHT4x_MEASURE_HI_us 7000 // 6.9..8.2 ms
#define SHT4x_MEASURE_LO	0xE0 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First
#define SHT4x_MEASURE_LO_us 1700 // 1.7 ms
#define SHT4x_GET_ID		0x89 // read ID register

#define CRC_POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

#ifndef USE_SENSOR_ID
#define USE_SENSOR_ID	0
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

static void send_sensor_byte(u8 cmd) {

	unsigned char r = irq_disable();
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	reg_i2c_id = sensor_i2c_addr;
	reg_i2c_adr = cmd;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_STOP;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	irq_restore(r);

}

void sensor_go_sleep(void) {
	if (sensor_i2c_addr == (SHTC3_I2C_ADDR << 1))
		send_sensor_word(SHTC3_GO_SLEEP); // Sleep command of the sensor
}

static void soft_reset_sensor(void) {
	if (sensor_i2c_addr == (SHTC3_I2C_ADDR << 1)) {
		send_sensor_word(SHTC3_SOFT_RESET); // Soft reset command
		sleep_us(SHTC3_SOFT_RESET_us); // 240 us
	} else if (sensor_i2c_addr) {
		send_sensor_byte(SHT4x_SOFT_RESET); // Soft reset command
		sleep_us(SHT4x_SOFT_RESET_us); // max 1 ms
	}
}

u8 sensor_crc(u8 crc) {
	int i;
	for(i = 8; i > 0; i--) {
		if (crc & 0x80)
			crc = (crc << 1) ^ (CRC_POLYNOMIAL & 0xff);
		else
			crc = (crc << 1);
	}
	return crc;
}

#if USE_SENSOR_ID
u32 get_sensor_id(void) {
	static const u8 utr_c3_gid[] = { 0x02,0x03,SHTC3_I2C_ADDR << 1, SHTC3_GET_ID & 0xff, (SHTC3_GET_ID >> 8) & 0xff };
	i2c_utr_t utr;
	u8 buf_id[6];
	u32 id = 0;
	if(sensor_i2c_addr == (SHTC3_I2C_ADDR << 1)) {
		if(!I2CBusUtr(&buf_id, (i2c_utr_t *)&utr_c3_gid, sizeof(utr_c3_gid) - 3)
			&& buf_id[2] == sensor_crc(buf_id[1] ^ sensor_crc(buf_id[0] ^ 0xff))) { // = 0x5b
			id = (buf_id[0] << 8) | buf_id[1]; // = 0x8708
		}
	} else if(sensor_i2c_addr)	{
		if(!send_i2c_byte(sensor_i2c_addr, SHT4x_GET_ID)) {
			utr.mode = 0;
			utr.rdlen = 6;
			utr.wrdata[0] = sensor_i2c_addr | 1;
			sleep_us(SHT4x_SOFT_RESET_us);
			if(I2CBusUtr(&buf_id, (i2c_utr_t *)&utr, 0) == 0
			&& buf_id[2] == sensor_crc(buf_id[1] ^ sensor_crc(buf_id[0] ^ 0xff))
			&& buf_id[5] == sensor_crc(buf_id[4] ^ sensor_crc(buf_id[3] ^ 0xff))
			) {
				id = (buf_id[3] << 24) | (buf_id[4] << 16) | (buf_id[0] << 8) | buf_id[1];
			}
		}
	}
	return id;
}
#endif

static int check_sensor(void) {
	sensor_i2c_addr = scan_i2c_addr(SHTC3_I2C_ADDR << 1);
	if(!sensor_i2c_addr) {
		sensor_i2c_addr = scan_i2c_addr(SHT4x_I2C_ADDR << 1);
		if(!sensor_i2c_addr)
			sensor_i2c_addr = scan_i2c_addr(SHT4xB_I2C_ADDR << 1);
	}
	soft_reset_sensor();
	// no i2c sensor ? sensor_i2c_addr = 0
	return sensor_i2c_addr;
}

void init_sensor(void) {
	//scan_i2c_addr(0);
	sensor_i2c_addr = 0;
	send_sensor_byte(0x06); // Reset command using the general call address
	sleep_us(SHTC3_WAKEUP_us);	// 240 us
	//	Wake-up command of the SHTC3 sensor
	sensor_i2c_addr = SHTC3_I2C_ADDR << 1;
	send_sensor_word(SHTC3_WAKEUP);
	sleep_us(SHTC3_WAKEUP_us);	// 240 us
	check_sensor();
#if USE_SENSOR_ID
	sensor_id = get_sensor_id();
#endif
}

__attribute__((optimize("-Os")))
int read_sensor_cb(void) {
	u16 _temp;
	u16 _humi;
	u8 data, crc; // calculated checksum
	int i;
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
		init_i2c();
	if (sensor_i2c_addr == 0) {
		if(check_sensor())
			sensor_go_sleep();
		return 0;
	}
#if USE_SENSOR_ID
	if(sensor_id)
		reg_i2c_id = sensor_i2c_addr | FLD_I2C_WRITE_READ_BIT;
	else
		reg_i2c_id = FLD_I2C_WRITE_READ_BIT;
#else
	reg_i2c_id = sensor_i2c_addr | FLD_I2C_WRITE_READ_BIT;
#endif
	i = 128;
	do {
		unsigned char r = irq_disable();
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
				measured_data.temp = ((s32)(17500*_temp) >> 16) - 4500; // x 0.01 C
				if (sensor_i2c_addr == (SHTC3_I2C_ADDR << 1))
					measured_data.humi = ((u32)(10000*_humi) >> 16); // x 0.01 %
				 else
					measured_data.humi = ((u32)(12500*_humi) >> 16) - 600; // x 0.01 %
				if (measured_data.humi < 0) measured_data.humi = 0;
				else if (measured_data.humi > 9999) measured_data.humi = 9999;
				// measured_data.count++;
				// Sleep command of the sensor = sensor_go_sleep();
				if (sensor_i2c_addr == (SHTC3_I2C_ADDR << 1))
					send_sensor_word(SHTC3_GO_SLEEP); // Sleep command of the sensor
				return 1;
			}
		}
		irq_restore(r);
	} while (i--);
	soft_reset_sensor();
	sensor_go_sleep();
	return 0;
}

extern void voltage_detect_init(u32 detectPin);
extern void voltage_detect(bool powerOn);

void start_measure_sensor_deep_sleep(void) {
	if (sensor_i2c_addr == (SHTC3_I2C_ADDR << 1)) {
		send_sensor_word(SHTC3_WAKEUP); //	Wake-up command of the sensor
		sleep_us(SHTC3_WAKEUP_us - 5);	// 240 us
		send_sensor_word(SHTC3_MEASURE);
		battery_detect();
		pm_wait_us(SHTC3_MEASURE_us);
	} else if (sensor_i2c_addr) {
		send_sensor_byte(SHT4x_MEASURE_HI);
		battery_detect();
		pm_wait_us(SHT4x_MEASURE_HI_us);
	} else
		return;
	//timer_measure_cb = clock_time() | 1;
}

int read_sensor(void) {
	start_measure_sensor_deep_sleep();
  	return read_sensor_cb();
}

#endif //  defined(SENSOR_TYPE) && SENSOR_TYPE == SENSOR_SHTXX
