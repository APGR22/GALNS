#ifndef _GALNS_NODESTREAM_H_
#define _GALNS_NODESTREAM_H_

#include "nodesystem.h"
#include "../macro.h"
#include "../type.h"
#include "../time.h"

#define GALNS_NULL_NEXT_ALLOC_NODE_INDEX 0

typedef struct GALNS_Nodestream GALNS_Nodestream;

typedef struct GALNS_NodeConfig
{
    GALNS_Timestamp created_at;
    GALNS_Timestamp modified_at;
    GALNS_Timestamp accessed_at;

    char *const name;
    void *const data;
    void *const flags;

    const size16_t name_mem_size;
    const size16_t data_mem_size;
    const size16_t flags_mem_size;

    size16_t data_size;
} GALNS_NodeConfig;

GALNS_API GALNS_NodeIndexOrError GALNS_CreateNode(GALNS_Nodesystem *nodesystem);
/**
 * @param dst_result The length depends to the @p `maxcount` . It may be NULL
 * @return Total nodes those successed to be created
 */
GALNS_API int64_t GALNS_CreateNodes(GALNS_Nodesystem *nodesystem, uint32_t maxcount, GALNS_NodeIndex *dst_result);
GALNS_API GALNS_Nodestream *GALNS_OpenNode(GALNS_Nodesystem *nodesystem, GALNS_NodeIndex node_index);

/**
 * @return GALNS_NodeConfig must be freed with `GALNS_ReleaseNodeConfig()`
 */
GALNS_API GALNS_NodeConfig *GALNS_GetNodeConfig(GALNS_Nodestream *nodestream);
GALNS_API int GALNS_SetNodeConfig(GALNS_Nodestream *nodestream, GALNS_NodeConfig *nodeconfig);
GALNS_API void GALNS_ReleaseNodeConfig(GALNS_NodeConfig *nodeconfig);

GALNS_API int GALNS_SyncNode(GALNS_Nodestream *nodestream);

GALNS_API void GALNS_CloseNode(GALNS_Nodestream *nodestream);

#endif