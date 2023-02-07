/**
  ******************************************************************************
  * File Name          : wolfSSL.I-CUBE-wolfSSL_conf.h
  * Description        : This file provides code for the configuration
  *                      of the wolfSSL.I-CUBE-wolfSSL_conf.h instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WOLFSSL_I_CUBE_WOLFSSL_CONF_H__
#define __WOLFSSL_I_CUBE_WOLFSSL_CONF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#define STM32H723xx
/**
	MiddleWare name : wolfSSL.I-CUBE-wolfSSL.5.5.3
	MiddleWare fileName : wolfSSL.I-CUBE-wolfSSL_conf.h
	MiddleWare version :
*/
/*---------- WOLF_CONF_DEBUG -----------*/
#define WOLF_CONF_DEBUG      0

/*---------- WOLF_CONF_WOLFCRYPT_ONLY -----------*/
#define WOLF_CONF_WOLFCRYPT_ONLY      0

/*---------- WOLF_CONF_TLS13 -----------*/
#define WOLF_CONF_TLS13      0

/*---------- WOLF_CONF_TLS12 -----------*/
#define WOLF_CONF_TLS12      1

/*---------- WOLF_CONF_DTLS -----------*/
#define WOLF_CONF_DTLS      0

/*---------- WOLF_CONF_MATH -----------*/
#define WOLF_CONF_MATH      2

/*---------- WOLF_CONF_RTOS -----------*/
#define WOLF_CONF_RTOS     2

/*---------- WOLF_CONF_RNG -----------*/
#define WOLF_CONF_RNG      1

/*---------- WOLF_CONF_RSA -----------*/
#define WOLF_CONF_RSA      1

/*---------- WOLF_CONF_ECC -----------*/
#define WOLF_CONF_ECC      1

/*---------- WOLF_CONF_DH -----------*/
#define WOLF_CONF_DH      1

/*---------- WOLF_CONF_AESGCM -----------*/
#define WOLF_CONF_AESGCM      1

/*---------- WOLF_CONF_AESCBC -----------*/
#define WOLF_CONF_AESCBC      0

/*---------- WOLF_CONF_CHAPOLY -----------*/
#define WOLF_CONF_CHAPOLY      1

/*---------- WOLF_CONF_EDCURVE25519 -----------*/
#define WOLF_CONF_EDCURVE25519      0

/*---------- WOLF_CONF_MD5 -----------*/
#define WOLF_CONF_MD5      0

/*---------- WOLF_CONF_SHA1 -----------*/
#define WOLF_CONF_SHA1      1

/*---------- WOLF_CONF_SHA2_224 -----------*/
#define WOLF_CONF_SHA2_224      0

/*---------- WOLF_CONF_SHA2_256 -----------*/
#define WOLF_CONF_SHA2_256      1

/*---------- WOLF_CONF_SHA2_384 -----------*/
#define WOLF_CONF_SHA2_384      0

/*---------- WOLF_CONF_SHA2_512 -----------*/
#define WOLF_CONF_SHA2_512      0

/*---------- WOLF_CONF_SHA3 -----------*/
#define WOLF_CONF_SHA3      0

/*---------- WOLF_CONF_PSK -----------*/
#define WOLF_CONF_PSK      0

/*---------- WOLF_CONF_PWDBASED -----------*/
#define WOLF_CONF_PWDBASED      1//1

/*---------- WOLF_CONF_KEEP_PEER_CERT -----------*/
#define WOLF_CONF_KEEP_PEER_CERT      1//0

/*---------- WOLF_CONF_BASE64_ENCODE -----------*/
#define WOLF_CONF_BASE64_ENCODE      1

/*---------- WOLF_CONF_OPENSSL_EXTRA -----------*/
#define WOLF_CONF_OPENSSL_EXTRA      0

/*---------- WOLF_CONF_TEST -----------*/
#define WOLF_CONF_TEST      0

/*---------- WOLF_CONF_PQM4 -----------*/
#define WOLF_CONF_PQM4      0

/* ------------------------------------------------------------------------- */
/* Hardware platform */
/* ------------------------------------------------------------------------- */
#define NO_STM32_HASH
#define NO_STM32_CRYPTO
//==============================================================================
#include "hermes-time.h"

#define WOLFSSL_STM32_CUBEMX
#define WOLFSSL_STM32H7
#define NO_OLD_RNGNAME /* conflicts with STM RNG macro */
#define HAVE_HASHDRBG

#undef WOLFSSL_NO_CLIENT_AUTH

#define FREERTOS

#define NO_FILESYSTEM
//#define SINGLE_THREADED
#define WOLFSSL_USER_IO
#define HAVE_ERRNO_H

#define WOLFSSL_SMALL_STACK
//#define	WOLFSSL_SHA3_SMALL
//#define WOLFSSL_MAX_SUITE_SZ	8
//#define WOLFSSL_MAX_SIGALGO		4
//#define OPENSSL_EXTRA
#define WOLFSSL_STATIC_RSA
#define WC_RSA_BLINDING
#define USE_SLOW_SHA2
#define USE_SLOW_SHA256
#define WC_RSA_NO_PADDING
#define WOLFSSL_TRUST_PEER_CERT
#define WOLFSSL_BASE64_ENCODE

#define WOLFSSL_HAVE_SP_RSA
#define WOLFSSL_SP
#define WOLFSSL_SP_SMALL
#define WOLFSSL_SP_MATH
#define WOLFSSL_SP_NO_3072
//#define TFM_MIPS

#define WOLFSSL_AES_NO_UNROLL

// Ommissions:
//#define NO_RSA
#define NO_WOLFSSL_SERVER
#define NO_ERROR_STRINGS
#define NO_DES3
#define NO_DSA
#define NO_MD4
#define NO_DH
#define NO_OLD_TLS
#define NO_SHA
#define NO_RC4
#define NO_RABBIT
#define NO_HC128
#define NO_SESSION_CACHE
#define NO_AES_128
#define NO_AES_192
//#define NO_AES_256
//#define NO_64BIT
#define NO_MD5
#define NO_PSK
#define WC_NO_CACHE_RESISTANT

// Inclusions:
#define HAVE_HKDF
//#define HAVE_AESGCM
//#define HAVE_AEAD
#define HAVE_TLS_EXTENSIONS
#define HAVE_SNI				1
#define HAVE_CHACHA
#define HAVE_PBKDF2

//#define HAVE_ECC
#define HAVE_CURVE25519
//#define NO_ECC_SECP
//#define HAVE_ALL_CURVES
//#define HAVE_SUPPORTED_CURVES
//#define WOLFSSL_NO_SIGALG

//#define OPENSSL_EXTRA
//#define OLD_HELLO_ALLOWED
//#define WOLFSSL_DTLS
//#define WOLFSSL_LWIP

//#define OPENSSL_EXTRA
extern struct tm *hermes_gmtime_2(time_t *tod);

// to allow us to redirect gmtime() to a thread-safe form. Changes made to wc_port.h
#define USER_TIME
#define HAVE_TM_TYPE
#define HAVE_TIME_T_TYPE
//#define TIME_OVERRIDES
#define XGMTIME(c, t) hermes_gmtime_2((c))
#define XTIME(t) get_UTC_forWolfSSL()

#define TRNG0 RNG
//==============================================================================
/* bypass certificate date checking, due to lack of properly configured RTC source */
/*
#ifndef HAL_RTC_MODULE_ENABLED
    #define NO_ASN_TIME
#endif
*/
#ifdef __cplusplus
}
#endif
#endif /* WOLFSSL_I_CUBE_WOLFSSL_CONF_H_H */

/**
  * @}
  */

/*****END OF FILE****/
