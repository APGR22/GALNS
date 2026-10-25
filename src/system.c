#include <galns/system.h>
#include <stdint.h>

bool GALNS_IsLittleEndian()
{
    // https://stackoverflow.com/a/4181991/22880994

    uint64_t x = 1;
    uint8_t *p = (uint8_t *) &x;

    return *p == x;
}