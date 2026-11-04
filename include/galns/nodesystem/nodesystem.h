#ifndef _GALNS_NODESYSTEM_H_
#define _GALNS_NODESYSTEM_H_

#include "../macro.h"
#include "../io.h"
#include "../type.h"

typedef struct GALNS_Nodesystem GALNS_Nodesystem;

GALNS_API GALNS_Nodesystem *GALNS_CreateNodesystem(
    const GALNS_IOInterface *io_stream,
    const char *filename,
    size16_t node_name_size,
    size16_t node_data_size,
    size16_t node_flags_size
);

GALNS_API int GALNS_SyncNodeHeader(GALNS_Nodesystem *nodesystem);

GALNS_API void GALNS_ReleaseNodesystem(GALNS_Nodesystem *nodesystem);

#endif