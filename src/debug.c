#include <galns/debug.h>
#include <string.h>
#include <stdio.h>
#include "galns.c.h"
#include "debug.c.h"

int GALNS_Internal_SetAndMergeErrorMsgs(const char *msg_1, const char *msg_2)
{
    if (!__galns_debug_msg) return -1;
    if (!msg_1) return -2;
    if (!msg_2) return -3;

    size_t str1_size = strnlen(msg_1, GALNS_DEBUG_MAX_MESSAGE_SIZE-1);
    size_t str2_size = strnlen(msg_2, GALNS_DEBUG_MAX_MESSAGE_SIZE-1);

    // Add 1 space between msg 1 and msg 2
    size_t total_size = str1_size + 1 + str2_size <= GALNS_DEBUG_MAX_MESSAGE_SIZE-1 ?
        str1_size + 1 + str2_size : GALNS_DEBUG_MAX_MESSAGE_SIZE-1;

    size_t size = 0;

    memcpy(__galns_debug_msg, msg_1, str1_size);
    size += str1_size;

    if (size == GALNS_DEBUG_MAX_MESSAGE_SIZE-1) goto finish;

    __galns_debug_msg[size] = ' ';
    size += 1;

    if (size == GALNS_DEBUG_MAX_MESSAGE_SIZE-1) goto finish;

    memcpy(__galns_debug_msg + size, msg_2, total_size - size);
    size += total_size - size;

    finish:

    __galns_debug_msg[size] = 0;

    return 0;
}

int GALNS_SetErrorMsg(const char *msg)
{
    if (!__galns_debug_msg) return -1;
    if (!msg) return -2;

    size_t str_size = strnlen(msg, GALNS_DEBUG_MAX_MESSAGE_SIZE-1);

    memcpy(__galns_debug_msg, msg, str_size);

    __galns_debug_msg[str_size] = 0;

    return 0;
}

const char *GALNS_GetErrorMsg()
{
    if (!__galns_debug_msg) return "Haven't done GALNS_Init()";

    return __galns_debug_msg;
}

void GALNS_ClearErrorMsg()
{
    if (!__galns_debug_msg) return;

    memset(__galns_debug_msg, 0, GALNS_DEBUG_MAX_MESSAGE_SIZE);
}