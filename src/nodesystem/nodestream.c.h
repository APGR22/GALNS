#ifndef _GALNS_NODESTREAM_C_H_
#define _GALNS_NODESTREAM_C_H_

#include <galns/nodesystem/nodestream.h>
#include <galns/type.h>
#include <galns/macro.h>
#include <stdint.h>

typedef struct GALNS_Node
{
    char *name;
    void *data;
    void *flags;

    uint32_t system_flags;
    unsigned char data_hash[GALNS_HASH_SIZE];
    GALNS_NodeIndex next_node_alloc_index;

    size16_t data_size;
} GALNS_Node;

struct GALNS_Nodestream
{
    GALNS_Node node;
    GALNS_Nodesystem *nodesystem;
};

size_t _GALNS_Internal_NodeSize(GALNS_Nodesystem *nodesystem);

#endif