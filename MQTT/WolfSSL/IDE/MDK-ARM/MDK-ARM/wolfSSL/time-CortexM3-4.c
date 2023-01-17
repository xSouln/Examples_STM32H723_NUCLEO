/* time-STM32F2.c
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


#include <stdint.h>
#define DWT                 ((DWT_Type       *)     (0xE0001000UL)     )
typedef struct
{
  uint32_t CTRL;                    /*< Offset: 0x000 (R/W)  Control Register                          */
  uint32_t CYCCNT;                  /*< Offset: 0x004 (R/W)  Cycle Count Register                      */
} DWT_Type;

extern uint32_t SystemCoreClock ;

double current_time(int reset)
{
      if(reset) DWT->CYCCNT = 0 ;
      return ((double)DWT->CYCCNT/SystemCoreClock) ;
}
