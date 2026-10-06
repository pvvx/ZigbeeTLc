#ifndef _REPORTING_H_
#define _REPORTING_H_


#include "tl_common.h"
#include "zcl_include.h"

status_t app_chk_report(u16 uptime_sec);
status_t app_forcedReport(u8 endpoint, u16 claster_id, u16 attr_id);
void app_set_thb_report(void);

#endif // _REPORTING_H_
