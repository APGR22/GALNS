#include "main.h"

int main()
{
    puts("LOG: Init");
    GALNS_Init();

    puts("LOG: Set Error Test");
    GALNS_SetErrorMsg("Error berhasil!");
    printf("Test: %s\n", GALNS_GetErrorMsg());

    puts("LOG: Create IO Interface");
    GALNS_IOInterface io_stream;
    io_stream.open = _io_open;
    io_stream.read = _io_read;
    io_stream.write = _io_write;
    io_stream.size = _io_size;
    io_stream.set_eof = _io_set_eof;
    io_stream.close = _io_close;

    puts("LOG: Create Nodesystem");
    GALNS_Nodesystem *nodesystem = GALNS_CreateNodesystem(
        &io_stream,
        ".dumptest/test.galns",
        DATA_ALLOC_SIZE,
        FLAGS_SIZE,
        HASH_SIZE,
        HEADER_METADATA_SIZE
    );
    if (!nodesystem)
    {
        printf("ERROR: %s (%d)\n", GALNS_GetErrorMsg(), GetLastError());

        GALNS_Quit();
        return -1;
    }

    puts("LOG: Get Nodesystem Metadata");
    unsigned char *metadata = GALNS_GetNodesystemMetadata(nodesystem);
    if (!metadata)
    {
        printf("ERROR: %s\n", GALNS_GetErrorMsg());

        GALNS_Quit();
        return -1;
    }

    puts("LOG: Write to Nodesystem Metadata");
    for (uint16_t index = 0; index < HEADER_METADATA_SIZE; index++)
    {
        metadata[index] = index % 256;
    }

    puts("LOG: Create Node");
    printf("Test: Get Node index is %lld\n", GALNS_CreateNode(nodesystem));

    puts("LOG: Create Nodes (2)");
    GALNS_NodeIndex temp_indexes[2];
    int64_t new_node_count = GALNS_CreateNodes(nodesystem, sizeof(temp_indexes) / sizeof(GALNS_NodeIndex), temp_indexes);
    printf("Test: Node count is %lld\n", new_node_count);
    puts("---");
    for (uint64_t index = 0; index < sizeof(temp_indexes) / sizeof(GALNS_NodeIndex); index++)
    {
        printf("New Index: %u\n", temp_indexes[index]);
    }
    puts("---");

    puts("LOG: Open Node 0");
    GALNS_Nodestream *node0 = GALNS_OpenNode(nodesystem, 0);
    if (!node0)
    {
        printf("ERROR: %s\n", GALNS_GetErrorMsg());

        GALNS_Quit();
        return -1;
    }

    puts("LOG: Setup Node 0 Config");
    GALNS_NodeConfig node0_config;
    memset(&node0_config, 0, sizeof(node0_config));

    node0_config.flags = malloc(FLAGS_SIZE);
    if (!(node0_config.flags))
    {
        puts("ERROR: Failed to malloc flags\n");

        GALNS_Quit();
        return -1;
    }
    node0_config.data = malloc(DATA_ALLOC_SIZE);
    if (!(node0_config.data))
    {
        puts("ERROR: Failed to malloc data\n");

        GALNS_Quit();
        return -1;
    }
    node0_config.hash = malloc(HASH_SIZE);
    if (!(node0_config.hash))
    {
        puts("ERROR: Failed to malloc hash\n");

        GALNS_Quit();
        return -1;
    }

    puts("LOG: Set Prev Node 0 Alloc Index");
    node0_config.prev_node_alloc_index = GALNS_NULL_PREV_ALLOC_NODE_INDEX;

    puts("LOG: Set flags");
    uint32_t temp_hash = 1;
    memcpy(node0_config.flags, &temp_hash, FLAGS_SIZE);

    printf("--- data_size node 0: %u\n", node0_config.data_size);

    puts("LOG: Write random bytes to Node 0");
    node0_config.data_size = DATA_ALLOC_SIZE;
    for (uint64_t index = 0; index < node0_config.data_size; index++)
    {
        ((unsigned char *)node0_config.data)[index] = index % 256;
    }

    puts("LOG: Write Node 0 Hash (random bytes, not real hash)");
    for (uint64_t index = 0; index < HASH_SIZE; index++)
    {
        node0_config.hash[index] = index * 2 % 256;
    }

    puts("LOG: Set Next Node 0 Alloc Index");
    node0_config.next_node_alloc_index = 1;

    puts("LOG: Write Node 0 to IO");
    if (GALNS_WriteNodeConfig(node0, &node0_config) < 0)
    {
        printf("ERROR: %s\n", GALNS_GetErrorMsg());

        GALNS_Quit();
        return -1;
    }

    puts("LOG: Close Node 0");
    GALNS_CloseNode(node0);

    puts("LOG: Cleanup Node 0 Config");
    free(node0_config.flags);
    free(node0_config.data);
    free(node0_config.hash);

    puts("LOG: Sync Node Header to IO");
    if (GALNS_SyncNodeHeader(nodesystem) < 0)
    {
        printf("ERROR: %s\n", GALNS_GetErrorMsg());

        GALNS_Quit();
        return -1;
    }

    puts("LOG: Release Nodesystem");
    GALNS_ReleaseNodesystem(nodesystem);

    puts("LOG: Quit");
    GALNS_Quit();

    return 0;
}