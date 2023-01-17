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

#include <wolfssl/wolfcrypt/visibility.h>
#include <wolfssl/wolfcrypt/logging.h>
#define __CORTEX_M3__

#include <wolfssl/wolfcrypt/types.h>
#include "wolfcrypt/src/misc.c"
#include "stm32f2xx_hal.h"
#include "cmsis_os.h"
#include "rl_net.h"
#include <stdio.h>

#include <wolfssl/ssl.h>


/*-----------------------------------------------------------------------------
 *        Initialize Clock Configuration
 *----------------------------------------------------------------------------*/
void SystemClock_Config(void) {
    #warning "write MPU specific System Clock Set up\n"
}

/*-----------------------------------------------------------------------------
 *        Initialize a Flash Memory Card
 *----------------------------------------------------------------------------*/
#if !defined(NO_FILESYSTEM)
#include "rl_fs.h"
static void init_filesystem (void) {
  int32_t retv;

  retv = finit ("M0:");
  if (retv == 0) {
    retv = fmount ("M0:");
    if (retv == 0) {
      printf ("Drive M0 ready!\n");
    }
    else {
      printf ("Drive M0 mount failed!\n");
    }
  }
  else {
    printf ("Drive M0 initialization failed!\n");
  }
}
#endif

typedef struct func_args {
    int    argc;
    char** argv;
} func_args;


extern void shell_main(func_args * args) ;

/*-----------------------------------------------------------------------------
 *       mian entry
 *----------------------------------------------------------------------------*/
int myoptind = 0;
char* myoptarg = NULL;

int main()
{
    void *arg = NULL ;

	  SystemClock_Config() ;
	  #if !defined(NO_FILESYSTEM)
        init_filesystem ();
	  #endif

    netInitialize() ;
    osDelay(300) ;

    #if defined(DEBUG_WOLFSSL)
         printf("Turning ON Debug message\n") ;
         wolfSSL_Debugging_ON() ;
    #endif

    shell_main(arg) ;

}
