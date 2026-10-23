#ifndef _GALNS_DEBUG_H_
#define _GALNS_DEBUG_H_

#include "macro.h"

#define GALNS_DEBUG_MAX_MESSAGE_SIZE 1024

GALNS_API int GALNS_SetErrorMsg(const char *msg);
GALNS_API const char *GALNS_GetErrorMsg();
GALNS_API void GALNS_ClearErrorMsg();

#endif