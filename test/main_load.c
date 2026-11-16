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

    puts("LOG: Get Node Config");
    GALNS_NodeConfig *node0_config = GALNS_GetNodeConfig(node0);

    puts("LOG: Get Node 0 data hash");
    unsigned char data_hash_1_buffer[GALNS_HASH_SIZE] = {0};
    if (GALNS_GetNodeDataHash(data_hash_1_buffer, node0) < 0)
    {
        printf("ERROR: %s\n", GALNS_GetErrorMsg());

        GALNS_Quit();
        return -1;
    }

    puts("Test: The hash is");
    for (uint16_t index = 0; index < GALNS_HASH_SIZE; index++)
    {
        printf("%x", data_hash_1_buffer[index]);
    }
    putc('\n', stdout);

    puts("LOG: Hash Node 0 data");
    unsigned char data_hash_2_buffer[GALNS_HASH_SIZE] = {0};
    if (GALNS_HashData(data_hash_2_buffer, node0_config->data, node0_config->data_mem_size) < 0)
    {
        printf("ERROR: %s\n", GALNS_GetErrorMsg());

        GALNS_Quit();
        return -1;
    }

    puts("Test: The hash is");
    for (uint16_t index = 0; index < GALNS_HASH_SIZE; index++)
    {
        printf("%x", data_hash_2_buffer[index]);
    }
    putc('\n', stdout);

    puts("LOG: Compare 2 hash to verify");
    printf("Test: result is %s\n",
        memcmp(data_hash_1_buffer, data_hash_2_buffer, GALNS_HASH_SIZE) == 0 ? "same" : "not same"
    );

    puts("LOG: Release Node Config");
    GALNS_ReleaseNodeConfig(node0_config);

    puts("LOG: Close Node 0");
    GALNS_CloseNode(node0);

    puts("LOG: Release Nodesystem");
    GALNS_ReleaseNodesystem(nodesystem);

    puts("LOG: Quit");
    GALNS_Quit();

    return 0;
}