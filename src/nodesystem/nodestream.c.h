#ifndef _GALNS_NODESTREAM_C_H_
#define _GALNS_NODESTREAM_C_H_

#include <galns/nodesystem/nodestream.h>
#include <galns/macro.h>
#include <galns/type.h>
#include <stdint.h>

#define GALNS_INTERNAL_START_NODE_OFFSET GALNS_INTERNAL_MAX_NODESYSTEM_HEADER
#define GALNS_INTERNAL_NODECONFIG_OFFSET 0

typedef struct GALNS_NodeInfo
{
    uint32_t system_flags;
} GALNS_NodeInfo;

struct GALNS_Nodestream
{
    GALNS_Nodesystem *nodesystem;
    GALNS_NodeInfo nodeinfo;
    GALNS_NodeIndex index;
};

size_t _GALNS_Internal_GetNodeConfigSize(GALNS_Nodesystem *nodesystem);

#endif