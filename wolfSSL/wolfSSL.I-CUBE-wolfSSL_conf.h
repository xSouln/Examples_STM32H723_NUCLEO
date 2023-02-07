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

/* ------------------------------------------------------------------------- */
/* Hardware platform */
/* ------------------------------------------------------------------------- */
#define NO_STM32_HASH
#define NO_STM32_CRYPTO
//==============================================================================
#include "wolfSSL_user_config.h"
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
