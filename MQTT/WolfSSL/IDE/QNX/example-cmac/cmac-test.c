/* cmac-test.c
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/cmac.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>

#include <sys/mman.h>
#include <hw/inout.h>
#include <sys/iofunc.h>
#include <sys/neutrino.h>

static const byte k256[] =
{
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};


static int createTag(const byte* key, int keySz, byte* msg, int msgSz,
        byte* msg2, int msg2Sz)
{
    Cmac cmac;
    byte tag[AES_BLOCK_SIZE];
    word32 i, tagSz;
    byte out[48];
    word32 outSz;

    XMEMSET(tag, 0, sizeof(tag));
    tagSz = AES_BLOCK_SIZE;

    outSz = 48;
    wc_caamCoverKey((byte*)key, keySz, out, &outSz, 0);

    if (wc_InitCmac(&cmac, NULL, keySz, WC_CMAC_AES, out) != 0) {
        printf("Cmac init error\n");
        return -1;
    }

    if (wc_CmacUpdate(&cmac, msg, msgSz) != 0) {
        printf("message update error\n");
        return -1;
    }

    if (msg2Sz > 0) {
        if (wc_CmacUpdate(&cmac, msg2, msg2Sz) != 0) {
            printf("message2 update error\n");
            return -1;
        }
    }

    if (wc_CmacFinal(&cmac, tag, &tagSz) != 0) {
        printf("create tag error\n");
        return -1;
    }

    printf("TAG :");
    for (i = 0; i < tagSz; i++)
        printf("%02X", tag[i]);
    printf("\n");

    return 0;
}

int main(int argc, char* argv[])
{
    uintptr_t virtual_base;
    byte msg[256];
    byte msg2[256];
    int i;

    printf("checking out permissions, can we call mmap_device_io?\n");
    virtual_base = mmap_device_io(0x00010000, 0x02140000);
    if (virtual_base == (uintptr_t)MAP_FAILED) {
        perror("mmap_device_io failed for base address ");
    }
    else {
        munmap_device_io(virtual_base, 0x00010000);
    }

    wolfCrypt_Init();

    XMEMSET(msg, 1, sizeof(msg));
    XMEMSET(msg2, 9, sizeof(msg2));

    for (i = 0; i < 256; i+=8) {
        if (createTag(k256, sizeof(k256), msg, i, NULL, 0) != 0)
            return -1;
    }

    wolfCrypt_Cleanup();
    return 0;
}
