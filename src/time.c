#include <galns/time.h>
#include <galns/macro.h>
#include <galns/type.h>
#include <sys/types.h>
#include <string.h>
#include "time.c.h"
#include "debug.c.h"

#ifdef GALNS_PLATFORM_WINDOWS
#   define timegm _mkgmtime
#endif

size_t _GALNS_Internal_GetTimestampSize()
{
    GALNS_Timestamp *dummy = NULL;

    return 0
    + sizeof(dummy->years)
    + sizeof(dummy->months)
    + sizeof(dummy->days)
    + sizeof(dummy->hours)
    + sizeof(dummy->minutes)
    + sizeof(dummy->seconds)
    + sizeof(dummy->nanoseconds)
    ;
}

int _GALNS_Internal_ConvertTimestamp2Bin(void *dst, GALNS_Timestamp *timestamp)
{
    if (!dst) return -1;
    if (!timestamp) return -2;

    off_t offset = 0;
    size8_t member_size = 0;

    member_size = sizeof(timestamp->years);
    memcpy(dst + offset, &(timestamp->years), member_size);
    offset += member_size;

    member_size = sizeof(timestamp->months);
    memcpy(dst + offset, &(timestamp->months), member_size);
    offset += member_size;

    member_size = sizeof(timestamp->days);
    memcpy(dst + offset, &(timestamp->days), member_size);
    offset += member_size;

    member_size = sizeof(timestamp->hours);
    memcpy(dst + offset, &(timestamp->hours), member_size);
    offset += member_size;

    member_size = sizeof(timestamp->minutes);
    memcpy(dst + offset, &(timestamp->minutes), member_size);
    offset += member_size;

    member_size = sizeof(timestamp->seconds);
    memcpy(dst + offset, &(timestamp->seconds), member_size);
    offset += member_size;

    member_size = sizeof(timestamp->nanoseconds);
    memcpy(dst + offset, &(timestamp->nanoseconds), member_size);
    offset += member_size;

    return 0;
}
int _GALNS_Internal_ConvertBin2Timestamp(GALNS_Timestamp *timestamp, void *src)
{
    if (!src) return -1;
    if (!timestamp) return -2;

    off_t offset = 0;
    size8_t member_size = 0;

    member_size = sizeof(timestamp->years);
    memcpy(&(timestamp->years), src + offset, member_size);
    offset += member_size;

    member_size = sizeof(timestamp->months);
    memcpy(&(timestamp->months), src + offset, member_size);
    offset += member_size;

    member_size = sizeof(timestamp->days);
    memcpy(&(timestamp->days), src + offset, member_size);
    offset += member_size;

    member_size = sizeof(timestamp->hours);
    memcpy(&(timestamp->hours), src + offset, member_size);
    offset += member_size;

    member_size = sizeof(timestamp->minutes);
    memcpy(&(timestamp->minutes), src + offset, member_size);
    offset += member_size;

    member_size = sizeof(timestamp->seconds);
    memcpy(&(timestamp->seconds), src + offset, member_size);
    offset += member_size;

    member_size = sizeof(timestamp->nanoseconds);
    memcpy(&(timestamp->nanoseconds), src + offset, member_size);
    offset += member_size;

    return 0;
}

int GALNS_GetTimestampNow(GALNS_Timestamp *dst_timestamp, int utc_offset_in_seconds)
{
    if (!dst_timestamp) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: dst_timestamp is NULL", -1);

    // https://en.cppreference.com/c/chrono/timespec_get

    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    ts.tv_sec += utc_offset_in_seconds;

    GALNS_ConvertTimespec2Timestamp(dst_timestamp, &ts);

    return 0;
}

int GALNS_ConvertTimespec2Timestamp(GALNS_Timestamp *timestamp, struct timespec *timespec)
{
    if (!timestamp) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: timestamp is NULL", -1);
    if (!timespec) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: timespec is NULL", -2);

    // https://stackoverflow.com/a/1442131/22880994

    struct tm time = *gmtime(&timespec->tv_sec);

    timestamp->years = time.tm_year + 1900;
    timestamp->months = time.tm_mon + 1;
    timestamp->days = time.tm_mday;
    timestamp->hours = time.tm_hour;
    timestamp->minutes = time.tm_min;
    timestamp->seconds = time.tm_sec;
    timestamp->nanoseconds = timespec->tv_nsec;

    return 0;
}

int GALNS_ConvertTimestamp2Timespec(struct timespec *timespec, GALNS_Timestamp *timestamp)
{
    if (!timespec) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: timespec is NULL", -1);
    if (!timestamp) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: timestamp is NULL", -2);

    // https://stackoverflow.com/a/57619535/22880994

    struct tm time = {0};
    time.tm_year = timestamp->years - 1900;
    time.tm_mon = timestamp->months - 1;
    time.tm_mday = timestamp->days;
    time.tm_hour = timestamp->hours;
    time.tm_min = timestamp->minutes;
    time.tm_sec = timestamp->seconds;
    time.tm_isdst = -1;

    time_t t = timegm(&time);

    timespec->tv_sec = t;
    timespec->tv_nsec = timestamp->nanoseconds;

    return 0;
}