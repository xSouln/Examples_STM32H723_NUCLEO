/* ssl-dummy.c
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/ssl.h>
#include <wolfssl/internal.h>

Signer* GetCA(void* vp, byte* hash)
{
    return NULL  ;
}

Signer* GetCAByName(void* vp, byte* hash)
{
    return NULL ;
}

