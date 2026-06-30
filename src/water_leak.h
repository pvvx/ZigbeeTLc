#ifndef _WATER_LEAK_H_
#define _WATER_LEAK_H_

#include "types.h"

void water_leak_init(void);
void water_leak_task(void);
void water_leak_force_report(void);
void water_leak_hold_awake(u16 seconds);
void water_leak_joining(void);
void water_leak_joined(void);
void water_leak_ota_start(void);
void water_leak_ota_stop(void);
void water_leak_prepare_sleep(void);
bool water_leak_keep_awake(void);

#endif // _WATER_LEAK_H_
