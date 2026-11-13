#include <galns/utility.h>
#include <string.h>
#include <openssl/evp.h>
#include "galns.c.h"
#include "debug.c.h"

ssize_t GALNS_HashData(unsigned char *dst_hash, void *data, size_t data_size)
{
    if (!dst_hash) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: dst_hash is NULL", -1);
    if (!data) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: data is NULL", -2);
    if (data_size == 0) GALNS_INTERNAL_DEBUG_ERROR_RETURN("Invalid argument: data_size is 0", -3);

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) return -3;

    /* INIT */

    unsigned char buffer[EVP_MAX_MD_SIZE] = {0};
    unsigned int length = 0;

    /* CODE */

    if (EVP_DigestInit_ex2(ctx, __galns_hash_md, NULL) != 1) goto error;

    if (EVP_DigestUpdate(ctx, data, data_size) != 1) goto error;

    if (EVP_DigestFinal_ex(ctx, buffer, &length) != 1) goto error;

    memcpy(dst_hash, buffer, GALNS_HASH_SIZE);

    /* CLEANUP */

    EVP_MD_CTX_free(ctx);

    /* RETURN IF SUCCESS */

    return length;

    /* ERROR */

    error:;

    if (ctx) EVP_MD_CTX_free(ctx);

    return -4; // -4 means failed to digest
}