/* benchmark_main.c
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
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfcrypt/benchmark/benchmark.h>
#include <stdio.h>

#ifndef NO_CRYPT_BENCHMARK
typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

static func_args args = { 0 } ;
#endif

int main(void)
{
    int ret;
#ifndef NO_CRYPT_BENCHMARK
	wolfCrypt_Init();

	printf("\nBenchmark Test\n");
	benchmark_test(&args);
    ret = args.return_code;
	printf("Benchmark Test: Return code %d\n", ret);

	wolfCrypt_Cleanup();
#else
    ret = NOT_COMPILED_IN;
#endif
	return ret;
}
