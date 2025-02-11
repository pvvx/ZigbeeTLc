#include "tl_common.h"

#if USE_SENSOR_TH
#include "chip_8258/timer.h"

#include "i2c_drv.h"
#include "device.h"
#include "sensors.h"
#include "battery.h"

#ifndef USE_I2C_DRV
#error "Define USE_I2C_DRV!"
#endif

sensor_th_t sensor_ht;

#define _TH_SPEED_CODE_SEC_ _attribute_ram_code_sec_ // for speed


#if USE_SENSOR_SHT30 || USE_SENSOR_SHT4X || USE_SENSOR_SHT30
int read_sensor_sht30_shtc3_sht4x(void *cfg);
#endif
//==================================== SHTC3
#if USE_SENSOR_SHTC3
//  I2C addres
//#define SHTC3_I2C_ADDR		0x70

// Sensor SHTC3 https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_SHTC3_Datasheet.pdf
#define SHTC3_WAKEUP		0x1735 // Wake-up command of the sensor
#define SHTC3_SOFT_RESET	0x5d80 // Soft reset command
#define SHTC3_GO_SLEEP		0x98b0 // Sleep command of the sensor
#define SHTC3_MEASURE		0x6678 // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
#define SHTC3_MEASURE_CS	0xA27C // Measurement commands, Clock Stretching, Normal Mode, Read T First
#define SHTC3_LPMEASURE		0x9C60 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First
#define SHTC3_LPMEASURE_CS	0x245C // Measurement commands, Clock Stretching, Low Power Mode, Read T First
#define SHTC3_GET_ID		0xC8EF // read ID register

#define SHTC3_WAKEUP_us			240    // time us
#define SHTC3_POWER_TIMEOUT_us	240		// time us, 180..240 us
#define SHTC3_SOFT_RESET_us		240		// time us, 180..240 us
#define SHTC3_HI_MEASURE_us		11000	// time us, 10.8..12.1 ms
#define SHTC3_LO_MEASURE_us		750		// time us, 0.7..0.8 ms ?

#define SHTC3_MAX_CLK_HZ		1000000	// I2C FM+ (1MHz)
#define SHTC3_MEASURING_TIMEOUT  SHTC3_HI_MEASURE_us // 11 ms

int start_measure_shtc3(void *cfg);

const sensor_def_cfg_t def_thcoef_shtc3 = {
		.coef.val1_k = 17500, // temp_k
		.coef.val1_z = -4500, // temp_z
		.coef.val2_k = 10000, // humi_k
		.coef.val2_z = 0, // humi_z
		.measure_timeout = SHTC3_MEASURING_TIMEOUT, // 11 ms
		.start_measure = start_measure_shtc3,
		.read_measure = read_sensor_sht30_shtc3_sht4x,
		.sensor_type = TH_SENSOR_SHTC3,
		.mode = MMODE_START_WAIT_READ
};
#endif
//==================================== SHT4x
#if  USE_SENSOR_SHT4X
int start_measure_sht4x(void *cfg);
//  I2C addres
//#define SHT4x_I2C_ADDR			0x44
//#define SHT4xB_I2C_ADDR			0x45

// Sensor SHT4x https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_Datasheet.pdf
#define SHT4x_SOFT_RESET	0x94 // Soft reset command
#define SHT4x_MEASURE_HI	0xFD // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
#define SHT4x_MEASURE_MD	0xF6 // Measurement commands, Clock Stretching Disabled, Medium precision, Read T First
#define SHT4x_MEASURE_LO	0xE0 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First
#define SHT4x_GET_ID		0x89 // read serial number

#define SHT4x_POWER_TIMEOUT_us	1000	// time us, 0.3..1 ms
#define SHT4x_SOFT_RESET_us		1000	// time us, 1 ms
#define SHT4x_HI_MEASURE_us		7000	// time us, 6.9..8.3 ms
#define SHT4x_MD_MEASURE_us		4000	// time us, 3.7..4.5 ms
#define SHT4x_LO_MEASURE_us		1600	// time us, 1.3..1.6 ms

#define SHT4x_MAX_CLK_HZ		1000000	// I2C FM+ (1MHz)
#define SHT4x_MEASURING_TIMEOUT  SHT4x_HI_MEASURE_us // 7 ms

const sensor_def_cfg_t def_thcoef_sht4x = {
		.coef.val1_k = 17500, // temp_k
		.coef.val1_z = -4500, // temp_z
		.coef.val2_k = 12500, // humi_k
		.coef.val2_z = -600, // humi_z
		.measure_timeout = SHT4x_MEASURING_TIMEOUT, // 7 ms
		.start_measure = start_measure_sht4x,
		.read_measure = read_sensor_sht30_shtc3_sht4x,
		.sensor_type = TH_SENSOR_SHT4x,
		.mode = MMODE_READ_START
};
#endif //  USE_SENSOR_SHT4X

//==================================== SHT30/GXHT3x/CHT832x

#if USE_SENSOR_SHT30
int start_measure_sht30(void *cfg);

//  I2C addres
//#define SHT30_I2C_ADDR		0x44
//#define SHT30_I2C_ADDR_MAX	0x45

#define SHT30_SOFT_RESET	0xA230 // Soft reset command
#define SHT30_RD_STATUS		0x2DF3 // Read status reg (2 bytes + crc)
#define SHT30_CLR_STATUS	0x4130 // Clear status reg (2 bytes + crc)
#define SHT30_HIMEASURE		0x0024 // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
#define SHT30_HIMEASURE_CS	0x062C // Measurement commands, Clock Stretching, Normal Mode, Read T First
#define SHT30_LPMEASURE		0x1624 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First
#define SHT30_LPMEASURE_CS	0x102C // Measurement commands, Clock Stretching, Low Power Mode, Read T First
#define CHT832x_CMD_MID		0x8137 // Read Manufacturer ID

#define SHT30_POWER_TIMEOUT_us	1500	// time us, 0.5..1.5 ms
#define SHT30_SOFT_RESET_us		1500	// time us, 0.5..1.5 ms
#define SHT30_HI_MEASURE_us		15000	// time us, 12.5..15.5 ms
#define SHT30_MD_MEASURE_us		5500	// time us, 4.5..6.5 ms
#define SHT30_LO_MEASURE_us		3500	// time us, 2.5..4.5 ms

#define SHT30_MAX_CLK_HZ		1000000	// I2C FM+ (1MHz)
#define SHT30_MEASURING_TIMEOUT  (SHT30_HI_MEASURE_us - 1000) // SHTV3 11 ms

const sensor_def_cfg_t def_thcoef_sht30 = {
		.coef.val1_k = 17500, // temp_k
		.coef.val2_k = 10000, // humi_k
		.coef.val1_z = -4500, // temp_z
		.coef.val2_z = 0,	  // humi_z
		.measure_timeout = 60000, // 60 ms
		.start_measure = start_measure_sht30,
		.read_measure = read_sensor_sht30_shtc3_sht4x,
		.sensor_type = TH_SENSOR_SHT30,
		.mode = MMODE_READ_START
};
#endif // #if USE_SENSOR_SHT30

//==================================== AHT20-30
#if USE_SENSOR_AHT20_30
int start_measure_aht2x(void *cfg);
int read_sensor_aht2x(void *cfg);

//  I2C addres
//#define AHT2x_I2C_ADDR	0x38

#define AHT2x_CMD_INI		0x0E1  // Initialization Command
#define AHT2x_CMD_TMS		0x0AC  // Trigger Measurement Command
#define AHT2x_DATA1_TMS		0x33  // Trigger Measurement data
#define AHT2x_DATA2_TMS		0x00  // Trigger Measurement data
/* Wait 80ms for the measurement to be completed, if the read status word Bit [7] is 0, it means
the measurement is completed, and then six bytes can be read continuously */
#define AHT2x_CMD_RST		0x0BA  // Soft Reset Command
#define AHT2x_RD_STATUS		0x071  // Read Status
/*Before reading the temperature and humidity value,
get a byte of status word by sending 0x71. If the status word and 0x18 are not equal to 0x18,
initialize the 0x1B, 0x1C, 0x1E registers, details Please refer to our official website routine for
the initialization process; if they are equal, proceed to the next step.*/
#define AHT2x_DATA_LPWR		0x0800 // go into low power mode

/* After power-on, the sensor needs less than 100ms stabilization time (SCL is
high at this time) to reach the idle state and it is ready to receive commands sent by the host
(MCU).*/
#define AHT2x_POWER_TIMEOUT_us	5000	// time us, 100 ms
#define AHT2x_SOFT_RESET_us		10000	// time us, 10 ms
#define AHT2x_MEASURE_us		80000	// time us, 80 ms

#define AHT2x_MAX_CLK_HZ		400000  // 400 kHz
#define AHT2x_MEASURING_TIMEOUT  AHT2x_MEASURE_us // 80 ms

const sensor_def_cfg_t def_thcoef_aht2x = {
		.coef.val1_k = 1250, // temp_k
		.coef.val2_k = 625, // temp_z
		.coef.val1_z = -5000, // humi_k
		.coef.val2_z = 0, // humi_z
		.measure_timeout = AHT2x_MEASURING_TIMEOUT, // 80 ms
		.start_measure = start_measure_aht2x,
		.read_measure = read_sensor_aht2x,
		.sensor_type = TH_SENSOR_AHT2x,
		.mode = MMODE_READ_START
};
#endif // USE_SENSOR_AHT20_30

//==================================== CHT8305
#if USE_SENSOR_CHT8305
int start_measure_cht8305(void *cfg);
int read_sensor_cht8305(void *cfg);

//  I2C addres
//#define CHT8305_I2C_ADDR		0x40
//#define CHT8305_I2C_ADDR_MAX	0x43

#define CHT8xxx_REG_MID		0xfe
#define CHT8xxx_REG_VID		0xff

//  Registers
#define CHT8305_REG_TMP		0x00
#define CHT8305_REG_HMD		0x01
#define CHT8305_REG_CFG		0x02
#define CHT8305_REG_ALR		0x03
#define CHT8305_REG_VLT		0x04
#define CHT8305_REG_MID		0xfe
#define CHT8305_REG_VID		0xff

//  Config register mask
#define CHT8305_CFG_SOFT_RESET          0x8000
#define CHT8305_CFG_CLOCK_STRETCH       0x4000
#define CHT8305_CFG_HEATER              0x2000
#define CHT8305_CFG_MODE                0x1000
#define CHT8305_CFG_VCCS                0x0800
#define CHT8305_CFG_TEMP_RES            0x0400
#define CHT8305_CFG_HUMI_RES            0x0300
#define CHT8305_CFG_ALERT_MODE          0x00C0
#define CHT8305_CFG_ALERT_PENDING       0x0020
#define CHT8305_CFG_ALERT_HUMI          0x0010
#define CHT8305_CFG_ALERT_TEMP          0x0008
#define CHT8305_CFG_VCC_ENABLE          0x0004
#define CHT8305_CFG_VCC_RESERVED        0x0003

#define CHT8305_MID	0x5959
#define CHT8305_VID	0x8305

#define CHT8305_POWER_TIMEOUT_us	5000	// time us, 5 ms
#define CHT8305_SOFT_RESET_us		5000	// time us, 5 ms
#define CHT8305_MEASURE_us			(6500+6500)	// time us, 6.5 T + 6.5 H ms

#define CHT8305_MAX_CLK_HZ			400000	// 400 kHz
#define CHT8305_MEASURING_TIMEOUT   CHT8305_MEASURE_us // 13 ms

const sensor_def_cfg_t def_thcoef_cht8305 = {
		.coef.val1_k = 16500, // temp_k
		.coef.val2_k = 10000, // humi_k
		.coef.val1_z = -4000, // temp_z
		.coef.val2_z = 0, // humi_z
		.measure_timeout = CHT8305_MEASURING_TIMEOUT,
		.start_measure = start_measure_cht8305, // 13 ms
		.read_measure = read_sensor_cht8305,
		.sensor_type = TH_SENSOR_CHT8305,
		.mode = MMODE_READ_START
};
#endif // USE_SENSOR_CHT8305

//==================================== CHT8215/CHT8310
#if USE_SENSOR_CHT8215
int start_measure_cht8215(void *cfg);
int read_sensor_cht8215(void *cfg);

#define CHT8215_I2C_ADDR0	0x40
#define CHT8215_I2C_ADDR1	0x44
#define CHT8215_I2C_ADDR2	0x48
#define CHT8215_I2C_ADDR3	0x4C

//	Registers
#define CHT8215_REG_TMP		0x00
#define CHT8215_REG_HMD		0x01
#define CHT8215_REG_STA		0x02
#define CHT8215_REG_CFG		0x03
#define CHT8215_REG_CRT		0x04
#define CHT8215_REG_TLL		0x05
#define CHT8215_REG_TLM		0x06
#define CHT8215_REG_HLL		0x07
#define CHT8215_REG_HLM		0x08
#define CHT8215_REG_OST		0x0f
#define CHT8215_REG_RST		0xfc
#define CHT8215_REG_MID		0xfe
#define CHT8215_REG_VID		0xff

//	Status register mask
#define CHT8215_STA_BUSY	0x8000
#define CHT8215_STA_THI		0x4000
#define CHT8215_STA_TLO		0x2000
#define CHT8215_STA_HHI		0x1000
#define CHT8215_STA_HLO		0x0800

//	Config register mask
#define CHT8215_CFG_MASK		0x8000
#define CHT8215_CFG_SD			0x4000
#define CHT8215_CFG_ALTH		0x2000
#define CHT8215_CFG_EM			0x1000
#define CHT8215_CFG_EHT			0x0100
#define CHT8215_CFG_TME			0x0080
#define CHT8215_CFG_POL			0x0020
#define CHT8215_CFG_ALT			0x0018
#define CHT8215_CFG_CONSEC_FQ	0x0006
#define CHT8215_CFG_ATM			0x0001

#define CHT8215_MID	0x5959
#define CHT8215_VID	0x1582


#define CHT8215_POWER_TIMEOUT_us	5000	// time us, 5 ms
#define CHT8215_SOFT_RESET_us		5000	// time us, 5 ms
#define CHT8215_MEASURE_us			(6500+6500)	// time us, 6.5 T + 6.5 H ms

#define CHT8215_MAX_CLK_HZ			400000	// 400 kHz
#define CHT8215_MEASURING_TIMEOUT   CHT8215_MEASURE_us


const sensor_def_cfg_t def_thcoef_cht8215 = {
		.coef.val1_k = 25606, // temp_k
		.coef.val1_z = 0, // temp_z
		.coef.val2_k = 20000, // humi_k
		.coef.val2_z = 0, // humi_z
		.measure_timeout = CHT8215_MEASURING_TIMEOUT, // 13 ms
		.start_measure = start_measure_cht8215,
		.read_measure = read_sensor_cht8215,
		.sensor_type = TH_SENSOR_CHT8215,
		.mode = MMODE_READ_ONLY
};
#endif // USE_SENSOR_CHT8215

//===================================

sensor_th_t sensor_ht;

#define CRC_POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

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

u8 sensor_crc_buf(u8 * msg, int len) {
	int i;
	u8 crc = 0xff;
	for(i = 0; i < len; i++) {
		crc = sensor_crc(crc ^ msg[i]);
	}
	return crc;
}

//=================================== AHT20_30
#if USE_SENSOR_AHT20_30

int start_measure_aht2x(void *cfg) {
	sensor_th_t *p = (sensor_th_t *)cfg;
	const u8 data[3] = {AHT2x_CMD_TMS, AHT2x_DATA1_TMS, AHT2x_DATA2_TMS};
	// 04000070ac3300
	return send_i2c_bytes(p->i2c_addr, (u8 *)data, sizeof(data));
}

__attribute__((optimize("-Os")))
int read_sensor_aht2x(void *cfg) {
	sensor_th_t *p = (sensor_th_t *)cfg;
	u32 _temp;
	u8 data[7];
	// 04000771
	if(read_i2c_bytes(p->i2c_addr, data, sizeof(data)) == 0
	    && (data[0] & 0x80) == 0
	    && !sensor_crc_buf(data, sizeof(data))) {
			_temp = (data[3] & 0x0F) << 16 | (data[4] << 8) | data[5];
			p->temp = ((u32)(_temp * p->coef.val1_k) >> 16) + p->coef.val1_z; // x 0.01 C // 16500 -4000
			_temp = (data[1] << 12) | (data[2] << 4) | (data[3] >> 4);
			p->humi = ((u32)(_temp * p->coef.val2_k) >> 16) + p->coef.val2_z; // x 0.01 % // 10000 -0
//			if(!start_measure_aht2x(p)) // start measure T/H
				return 0; // ok
	}
	p->i2c_addr = 0;
	return 1; // error
}

#endif

//=================================== CHT8305
#if USE_SENSOR_CHT8305

int start_measure_cht8305(void *cfg) {
	sensor_th_t *p = (sensor_th_t *)cfg;
	//scan_i2c_addr(p->i2c_addr); // wakeup?
	return send_i2c_byte(p->i2c_addr, CHT8305_REG_TMP); // start measure T/H
}

__attribute__((optimize("-Os")))
int read_sensor_cht8305(void *cfg) {
	sensor_th_t *p = (sensor_th_t *)cfg;
	u32 _temp, i = 3;
	u8 reg_data[4];
	while(i--) {
		if (!read_i2c_bytes(sensor_ht.i2c_addr, reg_data, sizeof(reg_data))) {
			_temp = (reg_data[0] << 8) | reg_data[1];
			p->temp = ((u32)(_temp * p->coef.val1_k) >> 16) + p->coef.val1_z; // x 0.01 C // 16500 -4000
			_temp = (reg_data[2] << 8) | reg_data[3];
			p->humi = ((u32)(_temp * p->coef.val2_k) >> 16) + p->coef.val2_z; // x 0.01 % // 10000 -0
//			if(!send_i2c_byte(sensor_ht.i2c_addr, CHT8305_REG_TMP)) // start measure T/H
				return 0;
		}
	}
	p->i2c_addr = 0;
	return 1; // error
}
#endif

//=================================== CHT8215
#if USE_SENSOR_CHT8215

int start_measure_cht8215(void *cfg) {
	return 0;
}

__attribute__((optimize("-Os")))
int read_sensor_cht8215(void *cfg) {
	sensor_th_t *p = (sensor_th_t *)cfg;
	u32 _temp, i = 3;
	u8 reg_data[4];
	while(i--) {
		if ((!read_i2c_addr_bytes(sensor_ht.i2c_addr, CHT8215_REG_TMP, reg_data, 2))
			&&(!read_i2c_addr_bytes(sensor_ht.i2c_addr, CHT8215_REG_HMD, &reg_data[2], 2))) {
			_temp = (reg_data[0] << 8) | reg_data[1];
			p->temp = ((u32)(_temp * p->coef.val1_k) >> 16) + p->coef.val1_z; // x 0.01 C // 16500 -4000
			_temp = (reg_data[2] << 8) | reg_data[3];
			p->humi = ((u32)(_temp * p->coef.val2_k) >> 16) + p->coef.val2_z; // x 0.01 % // 10000 -0
			return 0; // ok
		}
	}
	p->i2c_addr = 0;
	return 1; // error
}
#endif

//=================================== SHT4X/SHTC3/SHT30/CHT832x
#if USE_SENSOR_SHTC3
int start_measure_shtc3(void *cfg) {
	sensor_th_t *p = (sensor_th_t *)cfg;
	if(!send_i2c_word(p->i2c_addr, SHTC3_WAKEUP)) { //	Wake-up command of the sensor
		sleep_us(SHTC3_WAKEUP_us - 5);	// 240 us
#if (DEVICE_TYPE == DEVICE_CGDK2)
		if(p->id == 0xBDC3)
			return send_i2c_word(0, SHTC3_MEASURE_CS);
		else
			return send_i2c_word(p->i2c_addr, SHTC3_MEASURE);
#else
		return send_i2c_word(p->i2c_addr, SHTC3_MEASURE);
#endif
	}
	return 1; // error
}
#endif

#if USE_SENSOR_SHT4X
int start_measure_sht4x(void *cfg) {
	sensor_th_t *p = (sensor_th_t *)cfg;
	return send_i2c_byte(p->i2c_addr, SHT4x_MEASURE_HI);
}
#endif

#if USE_SENSOR_SHT30
int start_measure_sht30(void *cfg) {
	sensor_th_t *p = (sensor_th_t *)cfg;
	return send_i2c_word(p->i2c_addr, SHT30_HIMEASURE); // start measure T/H
}
#endif

#if (USE_SENSOR_SHT4X || USE_SENSOR_SHTC3 || USE_SENSOR_SHT30)
__attribute__((optimize("-Os")))
int read_sensor_sht30_shtc3_sht4x(void *cfg) {
	sensor_th_t *p = (sensor_th_t *)cfg;
	int ret = 1;
	int i;
	u16 _temp;
	u16 _humi;
#if USE_I2C_DRV == I2C_DRV_SOFT // Soft I2C
	u8 buf[6];
	i = 256; // ~55 us * 256 = 14080 us min
#else
	u8 crc; // calculated checksum
	u8 data;
	i = 512;
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
		init_i2c();
#if (I2C_CLOCK > 400000)
	u8 s = reg_i2c_speed;
	reg_i2c_speed = (u8)(CLOCK_SYS_CLOCK_HZ/(4*I2C_MAX_SPEED)); // 700 kHz
#endif
#if (BOARD == BOARD_CGDK2) && (USE_SENSOR_SHTC3)// SHTC3
	if(p->id == 0xBDC3)
		reg_i2c_id = FLD_I2C_WRITE_READ_BIT;
	else
		reg_i2c_id = p->i2c_addr | FLD_I2C_WRITE_READ_BIT;
#else
	reg_i2c_id = p->i2c_addr | FLD_I2C_WRITE_READ_BIT;
#endif
#endif
	do {
#if USE_I2C_DRV == I2C_DRV_SOFT // Soft I2C
#if (BOARD == BOARD_CGDK2) && (USE_SENSOR_SHTC3) // SHTC3
		if(p->id == 0xBDC3) {
			if(read_i2c_bytes(0, buf, sizeof(buf)))
				continue;
		} else {
			if(read_i2c_bytes(p->i2c_addr, buf, sizeof(buf)))
				continue;
		}
#else
		if(read_i2c_bytes(p->i2c_addr, buf, sizeof(buf)))
			continue;
#endif
		if (!sensor_crc_buf(buf, 3) && !sensor_crc_buf(&buf[3], 3)) {
			_temp = (buf[0] << 8) | buf[1];
			_humi = (buf[3] << 8) | buf[4];
			if(_temp != 0xffff) {

#else
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
			if (crc != data) {
				reg_i2c_ctrl = FLD_I2C_CMD_STOP;
				while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			    irq_restore(r);
				continue;
			}
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			_humi = data << 8;
			crc = sensor_crc(data ^ 0xff);
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID | FLD_I2C_CMD_ACK;
			_humi |= data;
			crc = sensor_crc(crc ^ data);
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_STOP;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		    irq_restore(r);
			if (crc == data && _temp != 0xffff) {
#endif
				p->temp = ((s32)(_temp * p->coef.val1_k) >> 16) + p->coef.val1_z; // x 0.01 C //17500 - 4500
				p->humi = ((u32)(_humi * p->coef.val2_k) >> 16) + p->coef.val2_z; // x 0.01 %	   // 10000 -0
#if USE_SENSOR_SHTC3
				if (p->sensor_type == TH_SENSOR_SHTC3) {
					send_i2c_word(p->i2c_addr, SHTC3_GO_SLEEP); // Sleep command of the sensor
				}
#endif
				ret = 0;
				break;
			}
		}
	} while (i--);
#if (USE_I2C_DRV == I2C_DRV_HARD) && (I2C_CLOCK > 400000) // Hardware I2C
	reg_i2c_speed = s;
#endif
	if(ret)
		p->i2c_addr = 0;
	return ret;
}
#endif

//=================================== All sensors
static int check_sensor(void) {
#if USE_SENSOR_CHT8305 || USE_SENSOR_CHT8215
	int test_i2c_addr = CHT8305_I2C_ADDR << 1;
#else
#if (USE_SENSOR_SHT4X || USE_SENSOR_SHT30)
	int test_i2c_addr = SHT4x_I2C_ADDR << 1; // or SHT30_I2C_ADDR
#endif
#endif
	u8 buf[8];
	sensor_def_cfg_t *ptabinit = NULL;
	sensor_ht.sensor_type = TH_SENSOR_NONE;
#if SENSOR_SLEEP_MEASURE
	sensor_ht.measure_timeout = 0;
#endif
	sensor_ht.id = 0;
#if USE_SENSOR_SHTC3
	// cfg.hw_cfg.shtc3 = 0;
	// Scan I2C addr 0x70
	if ((sensor_ht.i2c_addr = (u8) scan_i2c_addr(SHTC3_I2C_ADDR << 1)) != 0) {
		// SHTC3
		if(!send_i2c_word(SHTC3_I2C_ADDR << 1, SHTC3_WAKEUP)) { //	Wake-up command of the SHTC3 sensor
			sleep_us(SHTC3_WAKEUP_us);	// 240 us
			// cfg.hw_cfg.shtc3 = 1; // = 1 - sensor SHTC3
			if(!send_i2c_word(sensor_ht.i2c_addr, SHTC3_GET_ID) // Get ID
			&& !read_i2c_bytes(sensor_ht.i2c_addr, buf, 3)
			&& !sensor_crc_buf(buf, 3)) { // = 0x5b
				sensor_ht.id = (0x00C3<<16) | (buf[0] << 8) | buf[1]; // = 0x8708
			} else {
				// DEVICE_CGDK2 and bad sensor
				sensor_ht.id = 0xBDC3;
			}
			ptabinit = (sensor_def_cfg_t *)&def_thcoef_shtc3;
		}
		send_i2c_word(sensor_ht.i2c_addr, SHTC3_GO_SLEEP); // Sleep command of the sensor
	} else
#endif
#if USE_SENSOR_AHT20_30
	// Scan I2C addr 0x38
	 if ((sensor_ht.i2c_addr = (u8) scan_i2c_addr(AHT2x_I2C_ADDR << 1)) != 0) {
		// AHT2x..30
		// pm_wait_us(AHT2x_POWER_TIMEOUT_us);
		if(!send_i2c_byte(sensor_ht.i2c_addr, AHT2x_CMD_RST)) { // Soft reset command
			pm_wait_us(AHT2x_SOFT_RESET_us);
			// 0401017071 -> I2C addres 0x70, write 1 bytes, read: 18
//			if(!read_i2c_addr_bytes(sensor_ht.i2c_addr, AHT2x_RD_STATUS, buf, 1)) { // buf[0] = 0x18
			if(!read_i2c_bytes(sensor_ht.i2c_addr, buf, 1)) { // buf[0] = 0x18
				sensor_ht.id = (0x0020 << 16) | buf[0];
				ptabinit = (sensor_def_cfg_t *)&def_thcoef_aht2x;
			}
		}
	} else
#endif
	// Scan I2C addr 0x40..0x46
	 {
#if (USE_SENSOR_CHT8305 || USE_SENSOR_CHT8215 || USE_SENSOR_SHT4X || USE_SENSOR_SHT30)
		do {
			if((sensor_ht.i2c_addr = (u8) scan_i2c_addr(test_i2c_addr)) != 0) {
#if (USE_SENSOR_CHT8305 || USE_SENSOR_CHT8215)
				if(sensor_ht.i2c_addr <= (CHT8305_I2C_ADDR_MAX << 1)) {
					// I2C addr 0x40..0x43
					// CHT8305/CHT8315
					if (!read_i2c_addr_bytes(sensor_ht.i2c_addr, CHT8xxx_REG_MID, buf, 2) // Get MID
						&& !read_i2c_addr_bytes(sensor_ht.i2c_addr, CHT8xxx_REG_VID, &buf[2], 2)) {
						sensor_ht.id = (buf[2] << 24) | (buf[3] << 16) | (buf[0] << 8) | buf[1];
#if USE_SENSOR_CHT8305
						if(sensor_ht.id == ((CHT8305_VID << 16) | CHT8305_MID)) {
							// Soft reset command
							buf[0] = CHT8305_REG_CFG;
							buf[1] = (CHT8305_CFG_SOFT_RESET | CHT8305_CFG_MODE) >> 8;
							buf[2] = (CHT8305_CFG_SOFT_RESET | CHT8305_CFG_MODE) & 0xff;
							send_i2c_bytes(sensor_ht.i2c_addr, buf, 3);
							pm_wait_us(CHT8305_SOFT_RESET_us);
							// Configure
//							buf[0] = CHT8305_REG_CFG;
							buf[1] = CHT8305_CFG_MODE >> 8;
							buf[2] = CHT8305_CFG_MODE & 0xff;
							send_i2c_bytes(sensor_ht.i2c_addr, buf, 3);
							pm_wait_us(CHT8305_SOFT_RESET_us);
							//sensor_ht.sensor_type = TH_SENSOR_CHT8305;
							ptabinit = (sensor_def_cfg_t *)&def_thcoef_cht8305;
							break;
						} else
#endif
#if USE_SENSOR_CHT8215
						if(sensor_ht.id == ((CHT8215_VID << 16) | CHT8215_MID)) {
							//sensor_ht.sensor_type = TH_SENSOR_CHT8215;
							// if(measurement_step_time >= 5000 * CLOCK_16M_SYS_TIMER_CLK_1MS) { // > 5 sec
								buf[0] = CHT8215_REG_CRT;
								buf[1] = 0x03;
								buf[2] = 0;
								send_i2c_bytes(sensor_ht.i2c_addr, buf, 3); // Set conversion ratio 5 sec
							//}
							ptabinit = (sensor_def_cfg_t *)&def_thcoef_cht8215;
							break;
						} else
#endif
							sensor_ht.i2c_addr = 0;
					}
				} else
#endif // USE_SENSOR_CHT8305
				{
					// I2C addr 0x44..0x46
#if USE_SENSOR_SHT4X
					// SHT4x
					if(!send_i2c_byte(sensor_ht.i2c_addr, SHT4x_SOFT_RESET)) { // Soft reset command
						sleep_us(SHT4x_SOFT_RESET_us);
						if(!send_i2c_byte(sensor_ht.i2c_addr, SHT4x_GET_ID)) { // Get ID
							sleep_us(SHT4x_SOFT_RESET_us);
							if(read_i2c_bytes(sensor_ht.i2c_addr,  buf, 6) == 0
							&& buf[2] == sensor_crc(buf[1] ^ sensor_crc(buf[0] ^ 0xff))
							&& buf[5] == sensor_crc(buf[4] ^ sensor_crc(buf[3] ^ 0xff))
							) {
								sensor_ht.id = (buf[3] << 24) | (buf[4] << 16) | (buf[0] << 8) | buf[1];
								ptabinit = (sensor_def_cfg_t *)&def_thcoef_sht4x;
								break;
							}
						}
					}
#endif // USE_SENSOR_SHT4X
#if USE_SENSOR_SHT30
					{
						// SHT30/GXHT3x/CHT832x
						if(!send_i2c_word(sensor_ht.i2c_addr, SHT30_SOFT_RESET)) { // Soft reset command
							sleep_us(SHT30_SOFT_RESET_us);
							// clear status reg
							// send_i2c_word(sensor_cfg.i2c_addr, SHT30_CLR_STATUS);
							// read status reg
							if(!send_i2c_word(sensor_ht.i2c_addr, SHT30_RD_STATUS)
							&& !read_i2c_bytes(sensor_ht.i2c_addr, buf, 3)
							&& buf[2] == sensor_crc(buf[1] ^ sensor_crc(buf[0] ^ 0xff))) {
								//if(!send_i2c_word(sensor_cfg.i2c_addr, SHT30_HIMEASURE)) { // start measure T/H
								sensor_ht.id = (0x0030 << 16) | (buf[0] << 8) | buf[1];
									ptabinit = (sensor_def_cfg_t *)&def_thcoef_sht30;
								//}
								break;
							}
						}
					}
#endif // USE_SENSOR_SHT30
				}
			}
			test_i2c_addr += 2;
		} while(test_i2c_addr <= (SHT4x_I2C_ADDR_MAX << 1));
#endif // (USE_SENSOR_CHT8305 || USE_SENSOR_SHT4X || USE_SENSOR_SHT30)
	}
	if(ptabinit) {
		if(sensor_ht.coef.val1_k == 0) {
			memcpy(&sensor_ht.coef, &ptabinit->coef, sizeof(sensor_ht.coef));
		}
		sensor_ht.measure_timeout = ptabinit->measure_timeout;
		sensor_ht.start_measure = ptabinit->start_measure;
		sensor_ht.read_measure = ptabinit->read_measure;
		sensor_ht.sensor_type = ptabinit->sensor_type;
		sensor_ht.mode = ptabinit->mode;
	} else
		sensor_ht.i2c_addr = 0;
	// no i2c sensor ? sensor_ht.i2c_addr = 0
	return sensor_ht.i2c_addr;
}

#if USE_SENSOR_SHTC3
void sensor_go_sleep(void) {
	if ((sensor_ht.sensor_type == TH_SENSOR_SHTC3) && (sensor_ht.i2c_addr != 0))
		send_i2c_word(sensor_ht.i2c_addr, SHTC3_GO_SLEEP); // Sleep command of the sensor
}
#endif

void init_sensor(void) {
	int re = 0;
	send_i2c_byte(0, 0x06); // Reset command using the general call address
	sleep_us(190);	// 190 us
	battery_detect();
	if(check_sensor()) {
		if(sensor_ht.mode != MMODE_READ_ONLY && sensor_ht.start_measure)
			re = sensor_ht.start_measure((void *) &sensor_ht);
		if(!re && sensor_ht.measure_timeout)
			pm_wait_us(sensor_ht.measure_timeout);
	}
}

//_TH_SPEED_CODE_SEC_
int read_sensor(void) {
	int re = sensor_ht.i2c_addr == 0;
	if (!re && sensor_ht.sensor_type != TH_SENSOR_NONE) {
		if(sensor_ht.mode == MMODE_START_WAIT_READ) {
			if(sensor_ht.start_measure) {
				re = sensor_ht.start_measure((void *) &sensor_ht);
				if(!re && sensor_ht.measure_timeout)
					pm_wait_us(sensor_ht.measure_timeout);
			}
		}
		battery_detect();
		if(!re && sensor_ht.read_measure) {
			re = sensor_ht.read_measure((void *) &sensor_ht);
		}
		if(sensor_ht.mode == MMODE_READ_START && !re && sensor_ht.start_measure) {
			re = sensor_ht.start_measure((void *) &sensor_ht);
		}
		if(!re) {
			sensor_ht.temp += g_zcl_thermostatUICfgAttrs.temp_offset;
			sensor_ht.humi += g_zcl_thermostatUICfgAttrs.humi_offset;
			if (sensor_ht.humi < 0)
				sensor_ht.humi = 0;
			else if (sensor_ht.humi > 9999)
				sensor_ht.humi = 9999;
		}
	}  else
		battery_detect();
	if(re && check_sensor() && sensor_ht.mode == MMODE_READ_START && sensor_ht.start_measure) {
		re = sensor_ht.start_measure((void *) &sensor_ht);
	}
	return re;
}

#endif // SENSOR_TH
