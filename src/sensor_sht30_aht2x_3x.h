
#ifndef _SENSORS_SHTAHT2X3X_H_
#define _SENSORS_SHTAHT2X3X_H_



#if (SENSOR_TYPE == SENSOR_SHTAHT2X3X)
// Timing
#define AHT2x_SENSOR_POWER_TIMEOUT_ms  	20
#define AHT2x_SENSOR_RESET_TIMEOUT_ms		20
#define AHT2x_SENSOR_MEASURING_TIMEOUT_ms	80
#define AHT2x_SENSOR_MEASURING_TIMEOUT  (AHT2x_SENSOR_MEASURING_TIMEOUT_ms * CLOCK_16M_SYS_TIMER_CLK_1MS) // clk tick

#define CRC_POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

//  I2C addres
#define AHT2x_I2C_ADDR	0x38

#define AHT2x_CMD_INI	0x0E1  // Initialization Command
#define AHT2x_CMD_TMS	0x0AC  // Trigger Measurement Command
#define AHT2x_DATA1_TMS	0x33  // Trigger Measurement data
#define AHT2x_DATA2_TMS	0x00  // Trigger Measurement data
#define AHT2x_CMD_RST	0x0BA  // Soft Reset Command
#define AHT2x_DATA_LPWR	0x0800 // go into low power mode



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

typedef struct _measured_data_t {
	u16	battery_mv; // mV
	u16	average_battery_mv; // mV
	s16	temp; // in 0.01 C
	s16	temp2; // in 0.01 C
	u16	battery_level; // in 0.5%
} measured_data_t;

extern u8 sensor_sht_i2c_addr;
extern u8 sensor_aht_i2c_addr;
extern u32 sensor_sht_id;
extern u32 sensor_aht_id;
extern measured_data_t measured_data;

#define sensor_go_sleep()

void init_sensor(void);
int read_sensor(void);

#endif // (SENSOR_TYPE == SENSOR_SHTAHT2X3X)
#endif // _SENSORS_SHTAHT2X3X_H_

