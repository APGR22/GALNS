#ifndef _GALNS_NODESTREAM_H_
#define _GALNS_NODESTREAM_H_

#include "nodesystem.h"
#include "../macro.h"
#include "../type.h"

#define GALNS_NULL_PREV_ALLOC_NODE_INDEX UINT32_MAX
#define GALNS_NULL_NEXT_ALLOC_NODE_INDEX 0

typedef struct GALNS_Nodestream GALNS_Nodestream;

typedef struct GALNS_NodeConfig
{
    unsigned char *hash;
    void *data;
    void *flags;

    GALNS_NodeIndex prev_node_alloc_index;
    GALNS_NodeIndex next_node_alloc_index;

    size16_t data_size;
} GALNS_NodeConfig;

GALNS_API GALNS_NodeIndexOrError GALNS_CreateNode(GALNS_Nodesystem *nodesystem);
/**
 * @param dst_result The length depends to the @p `maxcount` . It may be NULL
 * @return Total nodes those successed to be created
 */
GALNS_API int64_t GALNS_CreateNodes(GALNS_Nodesystem *nodesystem, uint32_t maxcount, GALNS_NodeIndex *dst_result);
GALNS_API GALNS_Nodestream *GALNS_OpenNode(GALNS_Nodesystem *nodesystem, GALNS_NodeIndex node_index);

GALNS_API int GALNS_ReadNodeConfig(GALNS_Nodestream *nodestream, GALNS_NodeConfig *dst_nodeconfig);
GALNS_API int GALNS_WriteNodeConfig(GALNS_Nodestream *nodestream, GALNS_NodeConfig *src_nodeconfig);

GALNS_API void GALNS_CloseNode(GALNS_Nodestream *nodestream);

#endif