#include <galns/galns.h>
#include <stdio.h>
#include <string.h>

FILE *__io_file = NULL;

int _io_open(const char *filename, GALNS_IO_OPENMODE mode)
{
    FILE *temp = NULL;

    if (mode == GALNS_IO_OPENMODE_NEW)
    {
        temp = fopen(filename, "wb");
        if (!temp) return -1;

        fclose(temp);
    }

    temp = fopen(filename, "rb+");
    if (!temp) return -2;

    __io_file = temp;

    return 0;
}

ssize_t _io_read(off64_t offset, void *dst, size_t size)
{
    if (fseek(__io_file, offset, SEEK_SET) < 0) return -1;

    return fread(dst, sizeof(unsigned char), size, __io_file);
}

ssize_t _io_write(off64_t offset, void *src, size_t size)
{
    if (fseek(__io_file, offset, SEEK_SET) < 0) return -1;

    return fwrite(src, sizeof(unsigned char), size, __io_file);
}

ssize_t _io_size()
{
    if (fseek(__io_file, 0, SEEK_END) < 0) return -1;

    return ftell(__io_file);
}

ssize_t _io_set_eof(uint64_t length)
{
    if (fseek(__io_file, length-1, SEEK_SET) < 0) return -1;

    if (fputc(0, __io_file) < 0) return -2;

    return ftell(__io_file);
}

int _io_close()
{
    return fclose(__io_file);
}

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
        256,
        512,
        32
    );
    if (!nodesystem)
    {
        printf("ERROR: %s\n", GALNS_GetErrorMsg());

        GALNS_Quit();
        return -1;
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

    memcpy(node0_config->name, "Root Node", sizeof("Root Node"));
    printf("Test: Set Node 0 name to \"%s\"\n", node0_config->name);

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

    puts("LOG: Release Nodesystem");
    GALNS_ReleaseNodesystem(nodesystem);

    puts("LOG: Quit");
    GALNS_Quit();

    return 0;
}