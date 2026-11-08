#ifndef _GALNS_TIME_C_H_
#define _GALNS_TIME_C_H_

#include <galns/time.h>
#include <stdint.h>

size_t _GALNS_Internal_GetTimestampSize();

int _GALNS_Internal_ConvertTimestamp2Bin(void *dst, GALNS_Timestamp *timestamp);
int _GALNS_Internal_ConvertBin2Timestamp(GALNS_Timestamp *timestamp, void *src);

#endif