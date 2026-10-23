#include <galns/galns.h>
#include <stdlib.h>
#include "galns.c.h"

char *__galns_debug_msg = NULL;

int GALNS_Init()
{
    __galns_debug_msg = malloc(GALNS_DEBUG_MAX_MESSAGE_SIZE);
    if (!__galns_debug_msg) return -1;

    return 0;
}

void GALNS_Quit()
{
    free(__galns_debug_msg);
}