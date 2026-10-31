#include <galns/nodesystem/nodesystem.h>
#include <galns/macro.h>
#include <galns/system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include "nodesystem.c.h"
#include "../debug.c.h"
#include "../galns.c.h"

size_t __GALNS_Internal_NodesystemHeaderSize()
{
    GALNS_Nodesystem *dummy = NULL;

    return 0ULL
        + sizeof(dummy->header.version)
        + sizeof(dummy->header.is_little_endian)
        + sizeof(dummy->header.max_node_usercustomflags_size)
        + sizeof(dummy->header.max_node_name_size)
        + sizeof(dummy->header.max_node_data_size)
        + sizeof(dummy->header.node_size)
        + sizeof(dummy->header.end_node_index)
        + sizeof(dummy->header.header_hash)
    ;
}

int __GALNS_Internal_NodesystemHeader2Bin(void *dst, GALNS_Nodesystem *nodesystem)
{
    if (!dst) return -1;
    if (!nodesystem) return -2;

    off_t offset = 0;
    size16_t member_size = 0;

    member_size = sizeof(nodesystem->header.version.major);
    memcpy(dst + offset, &(nodesystem->header.version.major), member_size);
    offset += member_size;

    member_size = sizeof(nodesystem->header.version.minor);
    memcpy(dst + offset, &(nodesystem->header.version.minor), member_size);
    offset += member_size;

    member_size = sizeof(nodesystem->header.version.patch);
    memcpy(dst + offset, &(nodesystem->header.version.patch), member_size);
    offset += member_size;

    member_size = sizeof(nodesystem->header.is_little_endian);
    memcpy(dst + offset, &(nodesystem->header.is_little_endian), member_size);
    offset += member_size;

    member_size = sizeof(nodesystem->header.max_node_usercustomflags_size);
    memcpy(dst + offset, &(nodesystem->header.max_node_usercustomflags_size), member_size);
    offset += member_size;

    member_size = sizeof(nodesystem->header.max_node_name_size);
    memcpy(dst + offset, &(nodesystem->header.max_node_name_size), member_size);
    offset += member_size;

    member_size = sizeof(nodesystem->header.max_node_data_size);
    memcpy(dst + offset, &(nodesystem->header.max_node_data_size), member_size);
    offset += member_size;

    member_size = sizeof(nodesystem->header.node_size);
    memcpy(dst + offset, &(nodesystem->header.node_size), member_size);
    offset += member_size;

    member_size = sizeof(nodesystem->header.end_node_index);
    memcpy(dst + offset, &(nodesystem->header.end_node_index), member_size);
    offset += member_size;

    member_size = sizeof(nodesystem->header.header_hash);
    memcpy(dst + offset, &(nodesystem->header.header_hash), member_size);
    offset += member_size;

    return 0;
}

ssize_t __GALNS_Internal_HashNodesystemHeader(unsigned char *dst, GALNS_Nodesystem *nodesystem)
{
    if (!dst) return -1;
    if (!nodesystem) return -2;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) return -3;

    /* INIT */

    int ret = 1;
    unsigned char buffer[EVP_MAX_MD_SIZE] = {0};
    unsigned int length = 0;

    /* CODE */

    ret = EVP_DigestInit_ex2(ctx, __galns_hash_md, NULL);
    if (ret != 1) goto error;

    ret = EVP_DigestUpdate(ctx, &(nodesystem->header.version),                        sizeof(nodesystem->header.version)                      );
    if (ret != 1) goto error;
    ret = EVP_DigestUpdate(ctx, &(nodesystem->header.is_little_endian),               sizeof(nodesystem->header.is_little_endian)             );
    if (ret != 1) goto error;
    ret = EVP_DigestUpdate(ctx, &(nodesystem->header.max_node_usercustomflags_size),  sizeof(nodesystem->header.max_node_usercustomflags_size));
    if (ret != 1) goto error;
    ret = EVP_DigestUpdate(ctx, &(nodesystem->header.max_node_name_size),             sizeof(nodesystem->header.max_node_name_size)           );
    if (ret != 1) goto error;
    ret = EVP_DigestUpdate(ctx, &(nodesystem->header.max_node_data_size),             sizeof(nodesystem->header.max_node_data_size)           );
    if (ret != 1) goto error;
    ret = EVP_DigestUpdate(ctx, &(nodesystem->header.node_size),                      sizeof(nodesystem->header.node_size)                    );
    if (ret != 1) goto error;
    ret = EVP_DigestUpdate(ctx, &(nodesystem->header.end_node_index),                 sizeof(nodesystem->header.end_node_index)               );
    if (ret != 1) goto error;

    ret = EVP_DigestFinal_ex(ctx, buffer, &length);
    if (ret != 1) goto error;

    memcpy(dst, buffer, GALNS_HASH_SIZE);

    /* CLEANUP */

    EVP_MD_CTX_free(ctx);

    /* RETURN IF SUCCESS */

    return length;

    /* ERROR */

    error:;

    if (ctx) EVP_MD_CTX_free(ctx);

    return -4; // -4 means failed to digest
}

GALNS_Nodesystem *GALNS_CreateNodesystem(
    const GALNS_IOInterface *io_stream,
    const char *filename,
    size16_t node_name_size,
    size16_t node_data_size,
    size16_t node_usercustomflags_size
)
{
    if (!io_stream) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: io_stream is NULL", NULL);
    if (!filename) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: filename is NULL", NULL);

    /* INIT */

    GALNS_Nodesystem *nodesystem = NULL;
    void *bin = NULL;
    size_t bin_size = __GALNS_Internal_NodesystemHeaderSize();

    /* CODE */

    if (io_stream->open(filename, GALNS_IO_OPENMODE_NEW) < 0)
        GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to open IO stream", error);

    nodesystem = malloc(sizeof(GALNS_Nodesystem));
    if (!nodesystem) GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to malloc Nodesystem", error);

    bin = malloc(bin_size);
    if (!bin) GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to malloc binary object", error);

    nodesystem->io_stream = *io_stream;
    nodesystem->header.version.major = GALNS_VERSION_MAJOR;
    nodesystem->header.version.minor = GALNS_VERSION_MINOR;
    nodesystem->header.version.patch = GALNS_VERSION_PATCH;
    nodesystem->header.is_little_endian = GALNS_IsLittleEndian();
    nodesystem->header.max_node_usercustomflags_size = node_usercustomflags_size;
    nodesystem->header.max_node_name_size = node_name_size;
    nodesystem->header.max_node_data_size = node_data_size;
    nodesystem->header.node_size = 0; // currently not-defined
    nodesystem->header.end_node_index = 0;

    if (__GALNS_Internal_HashNodesystemHeader(nodesystem->header.header_hash, nodesystem) <= 0)
        GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to hash Nodesystem header", error);

    __GALNS_Internal_NodesystemHeader2Bin(bin, nodesystem);

    if (io_stream->write(0, bin, bin_size) < 0)
        GALNS_INTERNAL_DEBUG_ERROR_GOTO("Failed to write Nodesystem header", error);

    /* CLEANUP */

    free(bin);

    /* RETURN IF SUCCESS */

    return nodesystem;

    /* ERROR */

    error:;

    if (nodesystem) free(nodesystem);
    if (bin) free(bin);

    return NULL;
}

void GALNS_ReleaseNodesystem(GALNS_Nodesystem *nodesystem)
{
    if (!nodesystem) return;

    nodesystem->io_stream.close();

    free(nodesystem);
}