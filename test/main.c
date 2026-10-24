#include <galns/galns.h>
#include <stdio.h>

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
    io_stream.close = _io_close;

    puts("LOG: Quit");
    GALNS_Quit();

    return 0;
}