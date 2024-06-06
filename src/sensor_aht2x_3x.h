
#ifndef _SENSORS_AHT2X3X_H_
#define _SENSORS_AHT2X3X_H_

#if (SENSOR_TYPE == SENSOR_AHT2X3X)
// Timing
#define SENSOR_POWER_TIMEOUT_ms  	20
#define SENSOR_RESET_TIMEOUT_ms		20
#define SENSOR_MEASURING_TIMEOUT_ms	80
#define SENSOR_MEASURING_TIMEOUT  (SENSOR_MEASURING_TIMEOUT_ms * CLOCK_16M_SYS_TIMER_CLK_1MS) // clk tick

#define CRC_POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

//  I2C addres
#define AHT2x_I2C_ADDR	0x38

#define AHT2x_CMD_INI	0x0E1  // Initialization Command
#define AHT2x_CMD_TMS	0x0AC  // Trigger Measurement Command
#define AHT2x_DATA1_TMS	0x33  // Trigger Measurement data
#define AHT2x_DATA2_TMS	0x00  // Trigger Measurement data
#define AHT2x_CMD_RST	0x0BA  // Soft Reset Command
#define AHT2x_DATA_LPWR	0x0800 // go into low power mode

typedef struct _measured_data_t {
	u16	battery_mv; // mV
	u16	average_battery_mv; // mV
	s16	temp; // in 0.01 C
	s16	humi; // in 0.01 %
	u16	battery_level; // in 0.5%
} measured_data_t;

extern u8 sensor_i2c_addr;
extern u32 sensor_id;
extern measured_data_t measured_data;

#define sensor_go_sleep()

void init_sensor(void);
int read_sensor(void);

#endif // (SENSOR_TYPE == SENSORS_AHT2X3X)
#endif // _SENSORS_AHT2X3X_H_

