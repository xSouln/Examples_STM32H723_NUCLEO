/* main.c
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
#include <wolfcrypt/test/test.h>
#include <wolfcrypt/benchmark/benchmark.h>

/* wolfCrypt_Init/wolfCrypt_Cleanup to turn CryptoCell hardware on/off */
#include <wolfssl/wolfcrypt/wc_port.h>

/* SEGGER_RTT_Init, you can potential replace it with other serial terminal */
#include "SEGGER_RTT.h"

int main(void)
{
    int ret;

    SEGGER_RTT_Init();

    if ((ret = wolfCrypt_Init()) != 0) {
        printf("wolfCrypt_Init failed %d\n", ret);
        return -1;
    }

#ifndef NO_CRYPT_TEST
    printf("\nwolfCrypt Test Started\n");
    wolfcrypt_test(NULL);
    printf("\nwolfCrypt Test Completed\n");
#endif

#ifndef NO_CRYPT_BENCHMARK
    printf("\nBenchmark Test Started\n");
    benchmark_test(NULL);
    printf("\nBenchmark Test Completed\n");
#endif

    if ((ret = wolfCrypt_Cleanup()) != 0) {
        printf("wolfCrypt_Cleanup failed %d\n", ret);
        return -1;
    }
    while(1) {
        __WFI();
    }

    return 0;
}

