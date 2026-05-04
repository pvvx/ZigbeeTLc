/********************************************************************************************************
 * @file    ev_rtc.c
 *
 * @brief   This is the source file for ev_rtc
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "tl_common.h"
#include "os/ev_rtc.h"

#define	START_YEAR              2000  //UTC started at 00:00:00 January 1, 2000
#define	DAY_SECONDS             86400 //24 hours * 60 minutes * 60 seconds

#define	IS_LEAP_YEAR(year)      (!((year) % 400) || (((year) % 100) && !((year) % 4)))
#define	YEAR_LENGTH_GET(year)   ((u16)(IS_LEAP_YEAR(year) ? 366 : 365))

static u32 rtcRemainingMs;
u32 rtcSeconds;

static u8 monthLengthGet(bool isLeapYear, u8 month)
{
    u8 days = 31;

    if (month == 1) {
        days = 28 + (isLeapYear ? 1 : 0);
    } else {
        if (month > 6) {
            month--;
        }
        if (month & 1) {
            days = 30;
        }
    }

    return days;
}

void ev_rtc_second2utc(utcTime_t *time, UTCTime utcSeconds)
{
    u16 days = utcSeconds / DAY_SECONDS;
    u32 seconds = utcSeconds % DAY_SECONDS;

    time->hour = seconds / 3600;
    time->min = (seconds % 3600) / 60;
    time->sec = seconds % 60;

    time->year = START_YEAR;
    while (days >= YEAR_LENGTH_GET(time->year)) {
        days -= YEAR_LENGTH_GET(time->year);
        time->year++;
    }

    time->month = 0;
    while (days >= monthLengthGet(IS_LEAP_YEAR(time->year), time->month)) {
        days -= monthLengthGet(IS_LEAP_YEAR(time->year), time->month);
        time->month++;
    }

    time->day = (u8)days;

    time->month++;
    time->day++;
}

UTCTime ev_rtc_utc2Second(utcTime_t *time)
{
    u32 seconds = (((time->hour * 60) + time->min) * 60) + time->sec;

    u16 days = time->day - 1;

    s8 month = time->month - 1;
    while (--month >= 0) {
        days += monthLengthGet(IS_LEAP_YEAR(time->year), month);
    }

    u16 year = time->year;
    while (--year >= START_YEAR) {
        days += YEAR_LENGTH_GET(year);
    }

    seconds += (days * DAY_SECONDS);

    return seconds;
}

void ev_rtc_secondsSet(UTCTime seconds)
{
    rtcSeconds = seconds;
}

UTCTime ev_rtc_secondsGet(void)
{
    return rtcSeconds;
}

void ev_rtc_update(u32 updateTime)
{
    rtcRemainingMs += updateTime;
    if (rtcRemainingMs >= 1000) {
        rtcSeconds += rtcRemainingMs / 1000;
        rtcRemainingMs = rtcRemainingMs % 1000;
    }
}
