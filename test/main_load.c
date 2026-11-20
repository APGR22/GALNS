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

    puts("LOG: Load Nodesystem");
    GALNS_Nodesystem *nodesystem = GALNS_LoadNodesystem(&io_stream, ".dumptest/test.galns");
    if (!nodesystem)
    {
        printf("ERROR: %s (%d)\n", GALNS_GetErrorMsg(), GetLastError());

        GALNS_Quit();
        return -1;
    }

    puts("LOG: Get Nodesystem Metadata");
    size_t metadata_size = 0;
    unsigned char *metadata = GALNS_GetNodesystemMetadata(nodesystem, &metadata_size);
    if (!metadata)
    {
        printf("ERROR: %s\n", GALNS_GetErrorMsg());

        GALNS_Quit();
        return -1;
    }

    puts("LOG: Read Nodesystem Metadata");
    printf("---\n|");
    for (uint16_t index = 0; index < metadata_size; index++)
    {
        printf("%x|", metadata[index]);
    }
    puts("\n---");

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

    puts("LOG: Get Node 0 Config");
    GALNS_GetNodeConfig(node0, &node0_config);

    puts("LOG: Read Node 0 flags");
    uint32_t temp_flags;
    memcpy(&temp_flags, node0_config.flags, FLAGS_SIZE);
    printf("--- Node 0 flags is: %u\n", temp_flags);

    puts("LOG: Read Node 0 data");
    printf("--- \n|");
    for (uint64_t index = 0; index < node0_config.data_size; index++)
    {
        printf("%x|", ((unsigned char *) node0_config.data)[index]);
    }
    puts("\n---");

    puts("LOG: Read Node 0 Hash");
    printf("--- \n|");
    for (uint64_t index = 0; index < HASH_SIZE; index++)
    {
        printf("%x|", node0_config.hash[index]);
    }
    puts("\n---");

    puts("LOG: Get Next Node 0 Alloc Index");
    printf("Test: next_node_alloc_index is %u\n", node0_config.next_node_alloc_index);

    puts("LOG: Cleanup Node 0 Config");
    free(node0_config.flags);
    free(node0_config.data);
    free(node0_config.hash);

    puts("LOG: Close Node 0");
    GALNS_CloseNode(node0);

    puts("LOG: Release Nodesystem");
    GALNS_ReleaseNodesystem(nodesystem);

    puts("LOG: Quit");
    GALNS_Quit();

    return 0;
}