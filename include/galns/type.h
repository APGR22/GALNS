#ifndef _GALNS_TYPE_H_
#define _GALNS_TYPE_H_

#include <stdint.h>

typedef unsigned short size16_t;
typedef unsigned char size8_t;

typedef unsigned int GALNS_NodeIndex;
typedef long long int GALNS_NodeIndexOrError;

typedef struct GALNS_Version
{
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} GALNS_Version;

#endif