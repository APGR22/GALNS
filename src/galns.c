#include <galns/galns.h>
#include <stdlib.h>
#include "galns.c.h"

char *__galns_debug_msg = NULL;
EVP_MD *__galns_hash_md = NULL;

int GALNS_Init()
{
    __galns_debug_msg = malloc(GALNS_DEBUG_MAX_MESSAGE_SIZE);
    if (!__galns_debug_msg) goto error;

    __galns_hash_md = EVP_MD_fetch(NULL, "SHA256", NULL);
    if (!__galns_hash_md) goto error;

    return 0;

    error:;

    if (!__galns_debug_msg) free(__galns_debug_msg);
    if (!__galns_hash_md) EVP_MD_free(__galns_hash_md);
}

void GALNS_Quit()
{
    free(__galns_debug_msg);
    EVP_MD_free(__galns_hash_md);
}