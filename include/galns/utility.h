#ifndef _GALNS_UTILITY_H_
#define _GALNS_UTILITY_H_

#include <stdint.h>
#include "macro.h"

/**
 * @param dst_hash The size is depends on GALNS_HASH_SIZE
 */
GALNS_API ssize_t GALNS_HashData(unsigned char *dst_hash, void *data, size_t data_size);

#endif