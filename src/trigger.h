/*
 * tigger.h
 *
 *  Created on: 02.01.2021
 *      Author: pvvx
 */

#ifndef _TIGGER_H_
#define _TIGGER_H_

#include "app_cfg.h"

#if USE_TRIGGER

typedef struct __attribute__((packed)) _trigger_flg_t {
	u8 	trg_output	:	1; // GPIO_TRG pin output value (pull Up/Down)
	u8 	trigger_on	:	1; // Output GPIO_TRG pin is controlled according to the set parameters threshold temperature or humidity
	u8 	temp_out_on :	1; // Temperature trigger event
	u8 	humi_out_on :	1; // Humidity trigger event
}trigger_flg_t;

typedef struct __attribute__((packed)) _trigger_t {
	s16 temp_threshold; // x0.01°, Set temp threshold
	s16 humi_threshold; // x0.01%, Set humi threshold
	s16 temp_hysteresis; // Set temp hysteresis, -327.67..327.67 °
	s16 humi_hysteresis; // Set humi hysteresis, -327.67..327.67 %
	union {
		trigger_flg_t flg;
		u8	flg_byte;
	};
} trigger_t;

#define FEEP_SAVE_SIZE_TRG (sizeof(trg)-1)

extern trigger_t trg;
//extern const trigger_t def_trg;

void trigger_init(void);
void send_onoff(void);
void set_trigger_out(void);
nv_sts_t trigger_save(void);

#endif // USE_TRIGGER

#endif /* _TIGGER_H_ */
