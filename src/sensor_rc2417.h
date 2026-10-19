/**
 * @file sensor_rc2417.h
 * @brief Driver header for Skyrelay RC2417FR 24GHz mmWave radar sensor
 */

#ifndef _SENSOR_RC2417_H_
#define _SENSOR_RC2417_H_

void rc2417_init(void);
void rc2417_wake_and_start(void);
void rc2417_go_sleep(void);
u8 rc2417_read_out(void);
u8 rc2417_is_powered(void);
void rc2417_set_cfg(void);

#endif // _SENSOR_RC2417_H_
