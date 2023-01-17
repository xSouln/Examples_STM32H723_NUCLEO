/* tls_wolfssl.h
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */

#ifndef  __TLS_WOLFSSL_H__
#define  __TLS_WOLFSSL_H__

#ifdef __cplusplus
extern "C" {
#endif

int wolfsslRunTests(void);
void wolfssl_client_test(uintData_t);
void wolfssl_server_test(uintData_t);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* TLS_WOLFSSL_H */
