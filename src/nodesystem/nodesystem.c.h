#ifndef _GALNS_NODESYSTEM_C_H_
#define _GALNS_NODESYSTEM_C_H_

#include <galns/nodesystem/nodesystem.h>
#include <galns/type.h>
#include <galns/io.h>
#include <galns/macro.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct GALNS_Internal_NodesystemHeader
{
    unsigned char header_hash[GALNS_HASH_SIZE];

    uint32_t end_node_index;

    GALNS_Version version;

    size16_t max_node_usercustomflags_size;
    size16_t max_node_name_size;
    size16_t max_node_data_size;
    size16_t node_size;

    bool is_little_endian;
} GALNS_Internal_NodesystemHeader;

struct GALNS_Nodesystem
{
    GALNS_IOInterface io_stream;
    GALNS_Internal_NodesystemHeader header;
};

#endif