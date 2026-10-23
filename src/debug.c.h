#ifndef _GALNS_DEBUG_C_H_
#define _GALNS_DEBUG_C_H_

#include <galns/debug.h>

int GALNS_Internal_SetAndMergeErrorMsgs(const char *msg_1, const char *msg_2);

#define GALNS_INTERNAL_DEBUG(msg) GALNS_SetErrorMsg(msg)
#define GALNS_INTERNAL_DEBUG_ERROR_RETURN(msg, retval) do { GALNS_INTERNAL_DEBUG(msg); return retval; } while (0)
#define GALNS_INTERNAL_DEBUG_ERROR_GOTO(msg, gotval) do { GALNS_INTERNAL_DEBUG(msg); goto gotval; } while (0)

#define GALNS_INTERNAL_DEBUG_v2(msg_1, msg_2) GALNS_Internal_SetAndMergeErrorMsgs(msg_1, msg_2)
#define GALNS_INTERNAL_DEBUG_v2_ERROR_RETURN(msg_1, msg_2, retval) do { GALNS_INTERNAL_DEBUG_v2(msg_1, msg_2); return retval; } while (0)
#define GALNS_INTERNAL_DEBUG_v2_ERROR_GOTO(msg_1, msg_2, gotval) do { GALNS_INTERNAL_DEBUG_v2(msg_1, msg_2); goto gotval; } while (0)

#endif