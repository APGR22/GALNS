#include <galns/galns.h>
#include <stdio.h>
#include <string.h>

#ifdef GALNS_PLATFORM_WINDOWS
#   include <fileapi.h>
#   include <shlwapi.h>

HANDLE __io_handle = INVALID_HANDLE_VALUE;
#elif defined(GALNS_PLATFORM_LINUX)
#   include <fcntl.h>
#   include <unistd.h>
#   include <sys/stat.h>

int __io_fd = -1;
#else
FILE *__io_file = NULL;
#endif

int _io_open(const char *filename, GALNS_IO_OPENMODE mode)
{
#ifdef GALNS_PLATFORM_WINDOWS
    if (mode == GALNS_IO_OPENMODE_LOAD && !PathFileExistsA(filename)) return -1;

    int creation_mode = OPEN_EXISTING;
    if (mode == GALNS_IO_OPENMODE_NEW)
    {
        creation_mode = CREATE_ALWAYS;
    }

    int access = GENERIC_READ | GENERIC_WRITE;
    int share_mode = FILE_SHARE_READ;
    int flags_attr = FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL;

    HANDLE handle_temp = CreateFileA(
        filename,
        access,
        share_mode,
        NULL,
        creation_mode,
        flags_attr,
        NULL
    );
    if (handle_temp == INVALID_HANDLE_VALUE) return -2;

    __io_handle = handle_temp;

    return 0;
#elif defined(GALNS_PLATFORM_LINUX) // need to check file exists if the mode is LOAD
    int flags = O_LARGEFILE | O_CREAT | O_RDWR;
    if (mode == GALNS_IO_OPENMODE_NEW)
    {
        flags |= O_TRUNC;
    }

    int fd_temp = open64(filename, flags, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd_temp < 0) return -1;

    __io_fd = fd_temp;

    return 0;
#else
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
#endif
}

ssize_t _io_read(off64_t offset, void *dst, size_t size)
{
#ifdef GALNS_PLATFORM_WINDOWS
    OVERLAPPED overlapped_info;
    ZeroMemory(&overlapped_info, sizeof(overlapped_info));

    overlapped_info.Offset = offset;
    overlapped_info.OffsetHigh = offset >> (sizeof(DWORD) * CHAR_BIT);

    if (!ReadFile(__io_handle, dst, size, NULL, &overlapped_info))
    {
        DWORD lasterror_code = GetLastError();
        if (lasterror_code != ERROR_IO_PENDING) return -1;

        overlapped_info.hEvent = CreateEventA(NULL, TRUE, TRUE, NULL);
        if (!overlapped_info.hEvent) return -2;

        DWORD waitretval = WaitForSingleObject(overlapped_info.hEvent, INFINITE);
        if (waitretval != WAIT_OBJECT_0)
        {
            CloseHandle(overlapped_info.hEvent);

            return -3;
        }

        CloseHandle(overlapped_info.hEvent);
    }

    DWORD read_size = 0;
    if (!GetOverlappedResult(__io_handle, &overlapped_info, &read_size, FALSE)) return -4;

    return overlapped_info.InternalHigh; // use ULONG_PTR instead
#elif defined(GALNS_PLATFORM_LINUX)
    return pread64(__io_fd, dst, size, offset);
#else
    if (fseek(__io_file, offset, SEEK_SET) < 0) return -1;

    return fread(dst, sizeof(unsigned char), size, __io_file);
#endif
}

ssize_t _io_write(off64_t offset, void *src, size_t size)
{
#ifdef GALNS_PLATFORM_WINDOWS
    OVERLAPPED overlapped_info;
    ZeroMemory(&overlapped_info, sizeof(overlapped_info));

    overlapped_info.Offset = offset;
    overlapped_info.OffsetHigh = offset >> (sizeof(DWORD) * CHAR_BIT);

    if (!WriteFile(__io_handle, src, size, NULL, &overlapped_info))
    {
        DWORD lasterror_code = GetLastError();
        if (lasterror_code != ERROR_IO_PENDING) return -1;

        overlapped_info.hEvent = CreateEventA(NULL, TRUE, TRUE, NULL);
        if (!overlapped_info.hEvent) return -2;

        DWORD waitretval = WaitForSingleObject(overlapped_info.hEvent, INFINITE);\
        if (waitretval != WAIT_OBJECT_0)
        {
            CloseHandle(overlapped_info.hEvent);
            return -3;
        }

        CloseHandle(overlapped_info.hEvent);
    }

    DWORD written_size = 0;
    if (!GetOverlappedResult(__io_handle, &overlapped_info, &written_size, FALSE)) return -4;

    return overlapped_info.InternalHigh; // use ULONG_PTR instead
#elif defined(GALNS_PLATFORM_LINUX)
    return pwrite64(__io_fd, src, size, offset);
#else
    if (fseek(__io_file, offset, SEEK_SET) < 0) return -1;

    return fwrite(src, sizeof(unsigned char), size, __io_file);
#endif
}

ssize_t _io_size()
{
#ifdef GALNS_PLATFORM_WINDOWS
    LARGE_INTEGER size;
    if (!GetFileSizeEx(__io_handle, &size)) return -1;

    return size.QuadPart;
#elif defined(GALNS_PLATFORM_LINUX)
    struct stat64 fd_stats;
    if (fstat64(__io_fd, &fd_stats) < 0) return -1;

    return fd_stats.st_size;
#else
    if (fseek(__io_file, 0, SEEK_END) < 0) return -1;

    return ftell(__io_file);
#endif
}

ssize_t _io_set_eof(uint64_t length)
{
#ifdef GALNS_PLATFORM_WINDOWS
    LARGE_INTEGER _length;
    _length.QuadPart = length;

    if (!SetFilePointerEx(__io_handle, _length, NULL, FILE_BEGIN)) return -1;
    if (!SetEndOfFile(__io_handle)) return -2;

    LARGE_INTEGER length_result;
    if (!GetFileSizeEx(__io_handle, &length_result)) return -3;

    return length_result.QuadPart;
#elif defined(GALNS_PLATFORM_LINUX) // need to return file size
    return ftruncate64(__io_fd, length);
#else
    if (fseek(__io_file, length-1, SEEK_SET) < 0) return -1;

    if (fputc(0, __io_file) < 0) return -2;

    return ftell(__io_file);
#endif
}

int _io_close()
{
#ifdef GALNS_PLATFORM_WINDOWS
    if (!CloseHandle(__io_handle)) return -1;

    return 0;
#elif defined(GALNS_PLATFORM_LINUX)
    return close(__io_fd);
#else
    return fclose(__io_file);
#endif
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
        printf("ERROR: %s (%d)\n", GALNS_GetErrorMsg(), GetLastError());

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