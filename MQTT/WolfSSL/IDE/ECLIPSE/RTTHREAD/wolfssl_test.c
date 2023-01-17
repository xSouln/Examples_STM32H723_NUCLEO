/* wolfsslRunTests.c
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */
#include <stdint.h>
#include <wolfcrypt/test/test.h>
#include <wolfcrypt/benchmark/benchmark.h>

int wolfssl_test(void) {
#if !defined(NO_CRYPT_TEST)
    wolfcrypt_test(NULL);
#endif
#if !defined(NO_CRYPT_BENCHMARK)
    benchmark_test(NULL);
#endif
    return 0;
}
