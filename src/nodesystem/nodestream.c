#include <galns/nodesystem/nodestream.h>
#include <stdlib.h>
#include <string.h>
#include "nodestream.c.h"
#include "nodesystem.c.h"
#include "../debug.c.h"
#include <stdio.h>

int __GALNS_Internal_Node2Bin(void *dst, GALNS_Nodestream *nodestream)
{
    if (!dst) return -1;
    if (!nodestream) return -2;

    off_t offset = 0;
    size16_t member_size = 0;

    member_size = sizeof(nodestream->node.system_flags);
    memcpy(dst + offset, &(nodestream->node.system_flags), member_size);
    offset += member_size;

    member_size = nodestream->nodesystem->header.max_node_flags_size;
    memcpy(dst + offset, nodestream->node.flags, member_size);
    offset += member_size;

    member_size = nodestream->nodesystem->header.max_node_name_size;
    memcpy(dst + offset, nodestream->node.name, member_size);
    offset += member_size;

    member_size = sizeof(nodestream->node.data_size);
    memcpy(dst + offset, &(nodestream->node.data_size), member_size);
    offset += member_size;

    member_size = nodestream->nodesystem->header.max_node_data_size;
    memcpy(dst + offset, nodestream->node.data, member_size);
    offset += member_size;

    member_size = sizeof(nodestream->node.data_hash);
    memcpy(dst + offset, nodestream->node.data_hash, member_size);
    offset += member_size;

    member_size = sizeof(nodestream->node.next_node_alloc_index);
    memcpy(dst + offset, &(nodestream->node.next_node_alloc_index), member_size);
    offset += member_size;

    return 0;
}
int __GALNS_Internal_Bin2Node(GALNS_Nodestream *nodestream, void *src)
{
    if (!src) return -1;
    if (!nodestream) return -2;

    off_t offset = 0;
    size16_t member_size = 0;

    member_size = sizeof(nodestream->node.system_flags);
    memcpy(&(nodestream->node.system_flags), src + offset, member_size);
    offset += member_size;

    member_size = nodestream->nodesystem->header.max_node_flags_size;
    memcpy(nodestream->node.flags, src + offset, member_size);
    offset += member_size;

    member_size = nodestream->nodesystem->header.max_node_name_size;
    memcpy(nodestream->node.name, src + offset, member_size);
    offset += member_size;

    member_size = sizeof(nodestream->node.data_size);
    memcpy(&(nodestream->node.data_size), src + offset, member_size);
    offset += member_size;

    member_size = nodestream->nodesystem->header.max_node_data_size;
    memcpy(nodestream->node.data, src + offset, member_size);
    offset += member_size;

    member_size = sizeof(nodestream->node.data_hash);
    memcpy(nodestream->node.data_hash, src + offset, member_size);
    offset += member_size;

    member_size = sizeof(nodestream->node.next_node_alloc_index);
    memcpy(&(nodestream->node.next_node_alloc_index), src + offset, member_size);
    offset += member_size;

    return 0;
}

size_t _GALNS_Internal_NodeSize(GALNS_Nodesystem *nodesystem)
{
    if (!nodesystem) return 0;

    GALNS_Nodestream *dummy = NULL;

    return 0
    + sizeof(dummy->node.system_flags)
    + (nodesystem->header.max_node_flags_size)
    + (nodesystem->header.max_node_name_size)
    + sizeof(dummy->node.data_size)
    + (nodesystem->header.max_node_data_size)
    + sizeof(dummy->node.data_hash)
    + sizeof(dummy->node.next_node_alloc_index)
    ;
}

int64_t GALNS_CreateNodes(GALNS_Nodesystem *nodesystem, uint32_t maxcount, GALNS_NodeIndex *dst_result)
{
    if (!nodesystem) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: nodesystem is NULL", -1);
    if (maxcount == 0) return 0;

    uint32_t count = maxcount;
    uint32_t left = UINT32_MAX - nodesystem->header.end_node_index;

    if (left == 0) return 0;
    if (count > left) count = left;

    ssize_t io_before_length = nodesystem->io_stream.size();
    if (io_before_length < 0) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Failed to get the IO size", -3);

    size_t io_length = io_before_length + (nodesystem->header.node_size * count);
    io_length = nodesystem->io_stream.set_eof(io_length);
    if (io_length < 0) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Failed to set EOF", -4);

    uint64_t result_count = (io_length - io_before_length) / nodesystem->header.node_size;

    if (dst_result)
    {
        for (
            uint64_t index = 0, node_index = nodesystem->header.end_node_index;
            index < result_count;
            index++, node_index++
        )
        {
            dst_result[index] = node_index;
        }
    }

    nodesystem->header.end_node_index += result_count;

    return result_count;
}

GALNS_Nodestream *GALNS_OpenNode(GALNS_Nodesystem *nodesystem, GALNS_NodeIndex node_index)
{
    if (!nodesystem) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: nodesystem is NULL", NULL);
    if (node_index >= nodesystem->header.end_node_index) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: node_index exceeds Node counts", NULL);

    /* INIT */

    GALNS_Nodestream *nodestream = NULL;
    unsigned char *node_flags = NULL;
    char *node_name = NULL;
    void *node_data = NULL;
    void *bin = NULL;
    size_t bin_size = _GALNS_Internal_NodeSize(nodesystem);

    /* CODE */

    nodestream = malloc(sizeof(GALNS_Nodestream));
    if (!nodestream) GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to malloc Nodestream", error);

    node_flags = malloc(nodesystem->header.max_node_flags_size);
    if (!node_flags) GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to malloc Nodestream user custom flags", error);

    node_name = malloc(nodesystem->header.max_node_name_size);
    if (!node_name) GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to malloc Nodestream name", error);

    node_data = malloc(nodesystem->header.max_node_data_size);
    if (!node_data) GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to malloc Nodestream data", error);

    bin = malloc(bin_size);
    if (!bin) GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to malloc binary object", error);

    nodestream->nodesystem = nodesystem;
    nodestream->node.flags = node_flags;
    nodestream->node.name = node_name;
    nodestream->node.data = node_data;

    if (nodesystem->io_stream.read(GALNS_INTERNAL_MAX_NODESYSTEM_HEADER + (nodesystem->header.node_size * node_index), bin, bin_size) < 0)
        GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to read Node from IO", error);

    __GALNS_Internal_Bin2Node(nodestream, bin);

    /* CLEANUP */

    free(bin);

    /* RETURN IF SUCCESS */

    return nodestream;

    /* ERROR */

    error:;

    if (nodestream) free(nodestream);
    if (node_flags) free(node_flags);
    if (node_name) free(node_name);
    if (node_data) free(node_data);
    if (bin) free(bin);

    return NULL;
}

void GALNS_CloseNode(GALNS_Nodestream *nodestream)
{
    if (!nodestream) return;

    free(nodestream->node.flags);
    free(nodestream->node.name);
    free(nodestream->node.data);
    free(nodestream);
}