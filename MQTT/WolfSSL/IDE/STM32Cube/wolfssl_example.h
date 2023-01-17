/* wolfssl_example.h
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */

#ifndef WOLFSSL_EXAMPLE_H_
#define WOLFSSL_EXAMPLE_H_

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#ifndef WOLFSSL_USER_SETTINGS
	#include <wolfssl/options.h>
#endif
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfcrypt/test/test.h>
#include <wolfcrypt/benchmark/benchmark.h>

#ifndef SINGLE_THREADED
#include <cmsis_os.h>
#endif

#ifndef WOLF_EXAMPLES_STACK
#define WOLF_EXAMPLES_STACK  (30*1024)
#endif

#ifdef CMSIS_OS2_H_
void wolfCryptDemo(void* argument);
#else
void wolfCryptDemo(void const * argument);
#endif

#endif /* WOLFSSL_EXAMPLE_H_ */
