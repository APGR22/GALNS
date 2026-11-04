#ifndef _GALNS_IO_H_
#define _GALNS_IO_H_

#include "macro.h"

#ifdef GALNS_PLATFORM_LINUX
#   ifndef _LARGEFILE64_SOURCE
#       define _LARGEFILE64_SOURCE
#   endif
#endif

#include <stdint.h>
#include <sys/types.h>

typedef enum GALNS_IO_OPENMODE
{
    GALNS_IO_OPENMODE_NEW,
    GALNS_IO_OPENMODE_LOAD,
} GALNS_IO_OPENMODE;

typedef struct GALNS_IOInterface
{
    int (* open)(const char *filename, GALNS_IO_OPENMODE mode);
    ssize_t (* read)(off64_t offset, void *dst, size_t size);
    ssize_t (* write)(off64_t offset, void *src, size_t size);
    ssize_t (* set_eof)(uint64_t length); // currently unused
    ssize_t (* size)();
    int (* close)();
} GALNS_IOInterface;

#endif