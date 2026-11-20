#include <galns/nodesystem/nodestream.h>
#include <stdlib.h>
#include <string.h>
#include "nodestream.c.h"
#include "nodesystem.c.h"
#include "../galns.c.h"
#include "../debug.c.h"

int __GALNS_Internal_ConvertNodeConfig2Bin(
    void *dst,
    GALNS_NodeConfig *nodeconfig,
    GALNS_Nodesystem *nodesystem
)
{
    if (!dst) return -1;
    if (!nodeconfig) return -2;
    if (!nodesystem) return -3;

    off_t offset = 0;
    size16_t member_size = 0;

    member_size = sizeof(nodeconfig->prev_node_alloc_index);
    memcpy(dst + offset, &(nodeconfig->prev_node_alloc_index), member_size);
    offset += member_size;

    member_size = nodesystem->header.max_node_flags_size;
    memcpy(dst + offset, nodeconfig->flags, member_size);
    offset += member_size;

    member_size = sizeof(nodeconfig->data_size);
    memcpy(dst + offset, &(nodeconfig->data_size), member_size);
    offset += member_size;

    member_size = nodesystem->header.max_node_data_size;
    memcpy(dst + offset, nodeconfig->data, member_size);
    offset += member_size;

    member_size = nodesystem->header.max_node_hash_size;
    memcpy(dst + offset, nodeconfig->hash, member_size);
    offset += member_size;

    member_size = sizeof(nodeconfig->next_node_alloc_index);
    memcpy(dst + offset, &(nodeconfig->next_node_alloc_index), member_size);
    offset += member_size;

    return 0;
}

int __GALNS_Internal_ConvertBin2NodeConfig(
    GALNS_NodeConfig *nodeconfig,
    void *src,
    GALNS_Nodesystem *nodesystem
)
{
    if (!nodeconfig) return -1;
    if (!src) return -2;
    if (!nodesystem) return -3;

    off_t offset = 0;
    size16_t member_size = 0;

    member_size = sizeof(nodeconfig->prev_node_alloc_index);
    memcpy(&(nodeconfig->prev_node_alloc_index), src + offset, member_size);
    offset += member_size;

    member_size = nodesystem->header.max_node_flags_size;
    memcpy(nodeconfig->flags, src + offset, member_size);
    offset += member_size;

    member_size = sizeof(nodeconfig->data_size);
    memcpy(&(nodeconfig->data_size), src + offset, member_size);
    offset += member_size;

    member_size = nodesystem->header.max_node_data_size;
    memcpy(nodeconfig->data, src + offset, member_size);
    offset += member_size;

    member_size = nodesystem->header.max_node_hash_size;
    memcpy(nodeconfig->hash, src + offset, member_size);
    offset += member_size;

    member_size = sizeof(nodeconfig->next_node_alloc_index);
    memcpy(&(nodeconfig->next_node_alloc_index), src + offset, member_size);
    offset += member_size;

    return 0;
}

size_t _GALNS_Internal_GetNodeConfigSize(GALNS_Nodesystem *nodesystem)
{
    GALNS_NodeConfig *dummy = NULL;

    return 0
    + sizeof(dummy->prev_node_alloc_index)
    + nodesystem->header.max_node_flags_size // flags
    + sizeof(dummy->data_size)
    + nodesystem->header.max_node_data_size // data
    + nodesystem->header.max_node_hash_size // hash
    + sizeof(dummy->next_node_alloc_index)
    ;
}

GALNS_NodeIndexOrError GALNS_CreateNode(GALNS_Nodesystem *nodesystem)
{
    if (!nodesystem) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: nodesystem is NULL", -1);

    if (nodesystem->header.end_node_index == UINT32_MAX)
        GALNS_INTERNAL_DEBUG_ERROR_RETURN("It has reach max node index", -2);

    GALNS_NodeIndex cur_end_node_index = nodesystem->header.end_node_index++;

    if (nodesystem->io_stream.set_eof(
        GALNS_INTERNAL_MAX_NODESYSTEM_HEADER + (nodesystem->header.node_size * cur_end_node_index)
    ) < 0)
        GALNS_INTERNAL_DEBUG_ERROR_RETURN("Failed to set EOF", -3);

    return cur_end_node_index;
}

int64_t GALNS_CreateNodes(GALNS_Nodesystem *nodesystem, uint32_t maxcount, GALNS_NodeIndex *dst_result)
{
    if (!nodesystem) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: nodesystem is NULL", -1);
    if (maxcount == 0) return 0;

    uint32_t count = maxcount;
    uint32_t left = UINT32_MAX - nodesystem->header.end_node_index;

    if (left == 0) return 0;
    if (count > left) count = left;

    GALNS_NodeIndex prev_end_node_index = nodesystem->header.end_node_index;
    nodesystem->header.end_node_index += count;

    if (nodesystem->io_stream.set_eof(
        GALNS_INTERNAL_MAX_NODESYSTEM_HEADER + (nodesystem->header.node_size * nodesystem->header.end_node_index)
    ) < 0)
        GALNS_INTERNAL_DEBUG_ERROR_RETURN("Failed to set EOF", -2);

    if (dst_result)
    {
        for (
            uint64_t index = 0, node_index = prev_end_node_index;
            index < count;
            index++, node_index++
        )
        {
            dst_result[index] = node_index;
        }
    }

    return count;
}

GALNS_Nodestream *GALNS_OpenNode(GALNS_Nodesystem *nodesystem, GALNS_NodeIndex node_index)
{
    if (!nodesystem) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: nodesystem is NULL", NULL);
    if (node_index >= nodesystem->header.end_node_index) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: node_index exceeds Node counts", NULL);

    /* INIT */

    GALNS_Nodestream *nodestream = NULL;

    /* CODE */

    nodestream = malloc(sizeof(GALNS_Nodestream));
    if (!nodestream) GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to malloc Nodestream", error);

    nodestream->nodesystem = nodesystem;
    nodestream->index = node_index;

    /* RETURN IF SUCCESS */

    return nodestream;

    /* ERROR */

    error:;

    if (nodestream) free(nodestream);

    return NULL;
}

int GALNS_ReadNodeConfig(GALNS_Nodestream *nodestream, GALNS_NodeConfig *dst_nodeconfig)
{
    if (!nodestream) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: nodestream is NULL", -1);
    if (!nodestream->nodesystem) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: nodesystem in nodestream is NULL", -2);
    if (!dst_nodeconfig) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: dst_nodeconfig is NULL", -3);
    if (!(dst_nodeconfig->flags)) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: flags in dst_nodeconfig is NULL", -4);
    if (!(dst_nodeconfig->data)) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: data in dst_nodeconfig is NULL", -5);
    if (!(dst_nodeconfig->hash)) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: hash in dst_nodeconfig is NULL", -6);

    /* INIT */

    void *bin = NULL;
    size_t bin_size = _GALNS_Internal_GetNodeConfigSize(nodestream->nodesystem);
    off64_t offset = GALNS_INTERNAL_START_NODE_OFFSET
        + (nodestream->index * nodestream->nodesystem->header.node_size)
        + GALNS_INTERNAL_NODECONFIG_OFFSET;

    /* CODE */

    bin = malloc(bin_size);
    if (!bin) GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to malloc binary object", error);

    if (nodestream->nodesystem->io_stream.read(offset, bin, bin_size) < 0)
        GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to read node config", error);

    __GALNS_Internal_ConvertBin2NodeConfig(dst_nodeconfig, bin, nodestream->nodesystem);

    /* CLEANUP */

    free(bin);

    /* RETURN IF SUCCESS */

    return 0;

    /* ERROR */

    error:;

    if (bin) free(bin);

    return -100;
}

int GALNS_WriteNodeConfig(GALNS_Nodestream *nodestream, GALNS_NodeConfig *src_nodeconfig)
{
    if (!nodestream) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: nodestream is NULL", -1);
    if (!nodestream->nodesystem) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: nodesystem in nodestream is NULL", -2);
    if (!src_nodeconfig) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: dst_nodeconfig is NULL", -3);

    /* INIT */

    void *bin = NULL;
    size_t bin_size = nodestream->nodesystem->header.node_size;
    off64_t offset = GALNS_INTERNAL_START_NODE_OFFSET
        + (nodestream->index * bin_size)
        + GALNS_INTERNAL_NODECONFIG_OFFSET
        ;

    /* CODE */

    bin = malloc(bin_size);
    if (!bin) GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to malloc binary object", error);

    __GALNS_Internal_ConvertNodeConfig2Bin(bin + GALNS_INTERNAL_NODECONFIG_OFFSET, src_nodeconfig, nodestream->nodesystem);

    if (nodestream->nodesystem->io_stream.write(offset, bin, bin_size) < 0)
        GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to write node config to IO", error);

    /* CLEANUP */

    free(bin);

    /* RETURN IF SUCCESS */

    return 0;

    /* ERROR */

    error:;

    if (bin) free(bin);

    return -100;
}

void GALNS_CloseNode(GALNS_Nodestream *nodestream)
{
    if (!nodestream) return;

    free(nodestream);
}