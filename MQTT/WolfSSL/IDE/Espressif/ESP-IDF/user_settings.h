/* user_settings.h
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */
#undef WOLFSSL_ESPIDF
#define WOLFSSL_ESPIDF
#define WOLFSSL_ESPWROOM32
/* Uncomment next line if using Espressif ESP32-WROOM-32SE */
/* comment the above line #define WOLFSSL_ESPWROOM32 */
/* #define WOLFSSL_ESPWROOM32SE */

#define BENCH_EMBEDDED
#define USE_CERT_BUFFERS_2048

/* TLS 1.3                                 */
#define WOLFSSL_TLS13
#define HAVE_TLS_EXTENSIONS
#define WC_RSA_PSS
#define HAVE_HKDF
#define HAVE_AEAD
#define HAVE_SUPPORTED_CURVES

/* when you want to use SINGLE THREAD */
/* #define SINGLE_THREADED */
#define NO_FILESYSTEM

#define HAVE_AESGCM
/* when you want to use SHA384 */
/* #define WOLFSSL_SHA384 */
#define WOLFSSL_SHA512
#define HAVE_ECC
#define HAVE_CURVE25519
#define CURVE25519_SMALL
#define HAVE_ED25519

/* when you want to use pkcs7 */
/* #define HAVE_PKCS7 */

#if defined(HAVE_PKCS7)
    #define HAVE_AES_KEYWRAP
    #define HAVE_X963_KDF
    #define WOLFSSL_AES_DIRECT
#endif

/* when you want to use aes counter mode */
/* #define WOLFSSL_AES_DIRECT */
/* #define WOLFSSL_AES_COUNTER */

/* esp32-wroom-32se specific definition */
#if defined(WOLFSSL_ESPWROOM32SE)
    #define WOLFSSL_ATECC508A
    #define HAVE_PK_CALLBACKS
    /* when you want to use a custom slot allocation for ATECC608A */
    /* unless your configuration is unusual, you can use default   */
    /* implementation.                                             */
    /* #define CUSTOM_SLOT_ALLOCATION                              */
#endif

/* rsa primitive specific definition */
#if defined(WOLFSSL_ESPWROOM32) || defined(WOLFSSL_ESPWROOM32SE)
    /* Define USE_FAST_MATH and SMALL_STACK                        */
    #define ESP32_USE_RSA_PRIMITIVE
    /* threshold for performance adjustment for hw primitive use   */
    /* X bits of G^X mod P greater than                            */ 
    #define EPS_RSA_EXPT_XBTIS           36
    /* X and Y of X * Y mod P greater than                         */
    #define ESP_RSA_MULM_BITS            2000
#endif

/* debug options */
/* #define DEBUG_WOLFSSL */
/* #define WOLFSSL_ESP32WROOM32_CRYPT_DEBUG */
/* #define WOLFSSL_ATECC508A_DEBUG          */

/* date/time                               */
/* if it cannot adjust time in the device, */
/* enable macro below                      */
/* #define NO_ASN_TIME */
/* #define XTIME time */

/* when you want not to use HW acceleration */
/* #define NO_ESP32WROOM32_CRYPT */
/* #define NO_WOLFSSL_ESP32WROOM32_CRYPT_HASH*/
/* #define NO_WOLFSSL_ESP32WROOM32_CRYPT_AES */
/* #define NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI */

/* adjust wait-timeout count if you see timeout in rsa hw acceleration */
#define ESP_RSA_TIMEOUT_CNT    0x249F00
