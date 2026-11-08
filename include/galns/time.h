#ifndef _GALNS_TIME_H_
#define _GALNS_TIME_H_

#include <stdint.h>
#include <time.h>
#include "macro.h"

typedef struct GALNS_Timestamp
{
    uint16_t years;
    uint8_t months;
    uint8_t days;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint32_t nanoseconds;
} GALNS_Timestamp;

GALNS_API int GALNS_GetTimestampNow(GALNS_Timestamp *dst_timestamp, int utc_offset_in_seconds);

/**
 * @attention Minimum Timestamp::years and Timestamp::months are 1900 and 1
 */
GALNS_API int GALNS_ConvertTimespec2Timestamp(GALNS_Timestamp *timestamp, struct timespec *timespec);

/**
 * @attention Minimum Timestamp::years and Timestamp::months are 1900 and 1
 */
GALNS_API int GALNS_ConvertTimestamp2Timespec(struct timespec *timespec, GALNS_Timestamp *timestamp);

#endif