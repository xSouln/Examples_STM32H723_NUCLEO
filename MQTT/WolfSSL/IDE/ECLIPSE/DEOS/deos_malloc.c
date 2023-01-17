/* deos_malloc.c
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/types.h>
#include <deos.h>

void free_deos(void *ptr) {
    free(ptr);
    return;
}

void *realloc_deos(void *ptr, size_t size) {
    void *newptr;

    if (size == 0)
        return ptr;
    newptr = malloc_deos(size);

    if (ptr != NULL && newptr != NULL) {
        XMEMCPY((char *) newptr, (const char *) ptr, size);
        free_deos(ptr);
    }

    return newptr;
}

void *malloc_deos(size_t size) {
  return malloc(size);
}
