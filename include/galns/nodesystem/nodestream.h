#ifndef _GALNS_NODESTREAM_H_
#define _GALNS_NODESTREAM_H_

#include "nodesystem.h"
#include "../macro.h"
#include "../type.h"

#define GALNS_NULL_NODE_INDEX 0

typedef struct GALNS_Nodestream GALNS_Nodestream;

/**
 * @param dst_result The length depends to the @p `maxcount` . It may be NULL
 * @return Total nodes those successed to be created
 */
GALNS_API int64_t GALNS_CreateNodes(GALNS_Nodesystem *nodesystem, uint32_t maxcount, GALNS_NodeIndex *dst_result);
GALNS_API GALNS_Nodestream *GALNS_OpenNode(GALNS_Nodesystem *nodesystem, GALNS_NodeIndex node_index);
GALNS_API void GALNS_CloseNode(GALNS_Nodestream *nodestream);

#endif