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
        512,
        32,
        1024
    );
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

    puts("LOG: Write to Nodesystem Metadata");
    for (uint16_t index = 0; index < metadata_size; index++)
    {
        metadata[index] = index % 256;
    }

    puts("LOG: Create Node");
    printf("Test: Get Node index is %lld\n", GALNS_CreateNode(nodesystem));

    puts("LOG: Create Nodes (2)");
    int64_t new_node_count = GALNS_CreateNodes(nodesystem, 2, NULL);
    printf("Test: Node count is %lld\n", new_node_count);

    puts("LOG: Open Node 0");
    GALNS_Nodestream *node0 = GALNS_OpenNode(nodesystem, 0);
    if (!node0)
    {
        printf("ERROR: %s\n", GALNS_GetErrorMsg());

        GALNS_Quit();
        return -1;
    }

    puts("LOG: Get Node Config");
    GALNS_NodeConfig *node0_config = GALNS_GetNodeConfig(node0);

    puts("LOG: Set timestamp to now (UTC)");
    GALNS_GetTimestampNow(&(node0_config->created_at), 0);
    GALNS_GetTimestampNow(&(node0_config->modified_at), 0);
    GALNS_GetTimestampNow(&(node0_config->accessed_at), 0);

    printf("Test: Created at %u:%u:%u.%u %u/%u/%u\n",
        node0_config->created_at.hours,
        node0_config->created_at.minutes,
        node0_config->created_at.seconds,
        node0_config->created_at.nanoseconds,
        node0_config->created_at.months,
        node0_config->created_at.days,
        node0_config->created_at.years
    );

    printf("Test: Modified at %u:%u:%u.%u %u/%u/%u\n",
        node0_config->modified_at.hours,
        node0_config->modified_at.minutes,
        node0_config->modified_at.seconds,
        node0_config->modified_at.nanoseconds,
        node0_config->modified_at.months,
        node0_config->modified_at.days,
        node0_config->modified_at.years
    );

    printf("Test: Accessed at %u:%u:%u.%u %u/%u/%u\n",
        node0_config->accessed_at.hours,
        node0_config->accessed_at.minutes,
        node0_config->accessed_at.seconds,
        node0_config->accessed_at.nanoseconds,
        node0_config->accessed_at.months,
        node0_config->accessed_at.days,
        node0_config->accessed_at.years
    );

    puts("LOG: Set Node Config");
    GALNS_SetNodeConfig(node0, node0_config);

    puts("LOG: Release Node Config");
    GALNS_ReleaseNodeConfig(node0_config);

    puts("LOG: Sync Node to IO");
    if (GALNS_SyncNode(node0) < 0)
    {
        printf("ERROR: %s\n", GALNS_GetErrorMsg());

        GALNS_Quit();
        return -1;
    }

    puts("LOG: Close Node 0");
    GALNS_CloseNode(node0);

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