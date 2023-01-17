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

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#include "wolfcrypt/test/test.h"

#include <stdio.h>
#include <time.h>

#if defined(WOLFSSL_CMSIS_RTOS)
#include "cmsis_os.h"
#elif defined(WOLFSSL_CMSIS_RTOSv2)
#include "cmsis_os2.h"
#endif

/* Dummy definition for test RTC */
#define RTC_YEAR 2019
#define RTC_MONTH 1
#define RTC_DAY 1
#define RTC_MONTH 1
#define RTC_DAY 1

#if defined(STM32F7xx)
#include "stm32f7xx_hal.h"
#elif defined(STM32F4xx)
#include "stm32f4xx_hal.h"
#elif defined(STM32F2xx)
#include "stm32f2xx_hal.h"
#endif

#warning "write MPU specific Set ups\n"
static void SystemClock_Config (void) {

}

static void MPU_Config (void) {

}

static void CPU_CACHE_Enable (void) {

}

#if defined(WOLFSSL_CMSIS_RTOS) || defined(WOLFSSL_CMSIS_RTOSv2)

#if defined(WOLFSSL_CMSIS_RTOS)
extern uint32_t os_time;
#endif

uint32_t HAL_GetTick(void)
{
    #if defined(WOLFSSL_CMSIS_RTOS)
        return os_time;
    #elif defined(WOLFSSL_CMSIS_RTOSv2)
        return osKernelGetTickCount();
    #endif
}

double current_time(int reset)
{
    if (reset)
        return 0;
    #if defined(WOLFSSL_CMSIS_RTOS)
        return (double)os_time / 1000.0;
    #elif defined(WOLFSSL_CMSIS_RTOSv2)
        return (double)osKernelGetTickCount() / 1000.0;
    #endif
}

#else

#include <stdint.h>
#define DWT                 ((DWT_Type       *)     (0xE0001000UL)     )
typedef struct
{
  uint32_t CTRL;                    /*!< Offset: 0x000 (R/W)  Control Register                          */
  uint32_t CYCCNT;                  /*!< Offset: 0x004 (R/W)  Cycle Count Register                      */
} DWT_Type;

extern uint32_t SystemCoreClock ;

double current_time(int reset)
{
      if(reset) DWT->CYCCNT = 0 ;
      return ((double)DWT->CYCCNT/SystemCoreClock) ;
}
#endif

static time_t epochTime;
time_t time(time_t *t)
{
  return epochTime;
}

void setTime(time_t t)
{
  epochTime = t;
}

/*-----------------------------------------------------------------------------
 *        Initialize a Flash Memory Card
 *----------------------------------------------------------------------------*/
#if !defined(NO_FILESYSTEM)
#include "rl_fs.h"                      /* FileSystem definitions             */

static void init_filesystem (void) {
  int32_t retv;

  retv = finit ("M0:");
  if (retv == fsOK) {
    retv = fmount ("M0:");
    if (retv == fsOK) {
      printf ("Drive M0 ready!\n");
    }
    else {
      printf ("Drive M0 mount failed(%d)!\n", retv);
    }
  }
  else {
    printf ("Drive M0 initialization failed!\n");
  }
}
#endif

/*-----------------------------------------------------------------------------
 *       mian entry
 *----------------------------------------------------------------------------*/
void benchmark_test(void *arg) ;

int main()
{
    void * arg = NULL ;
    
    MPU_Config(); 
    CPU_CACHE_Enable();
    HAL_Init();                        /* Initialize the HAL Library     */
    SystemClock_Config();              /* Configure the System Clock     */

    #if !defined(NO_FILESYSTEM)
    init_filesystem ();
    #endif
    
    setTime((RTC_YEAR-1970)*365*24*60*60 + RTC_MONTH*30*24*60*60 + RTC_DAY*24*60*60);

    printf("=== Start: Crypt Benchmark ===\n") ;
    benchmark_test(arg) ;
    printf("=== End: Crypt Benchmark  ===\n") ;

}

