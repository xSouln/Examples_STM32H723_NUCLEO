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

#include <wolfssl/wolfcrypt/visibility.h>
#include <wolfssl/wolfcrypt/logging.h>

#include <RTL.h>
#include <stdio.h>
#include "wolfssl_MDK_ARM.h"

/*-----------------------------------------------------------------------------
 *        Initialize a Flash Memory Card
 *----------------------------------------------------------------------------*/
#if !defined(NO_FILESYSTEM)
static void init_card (void)
{
    U32 retv;

    while ((retv = finit (NULL)) != 0) {     /* Wait until the Card is ready */
        if (retv == 1) {
            printf ("\nSD/MMC Init Failed");
            printf ("\nInsert Memory card and press key...\n");
        } else {
            printf ("\nSD/MMC Card is Unformatted");
        }
     }
}
#endif


/*-----------------------------------------------------------------------------
 *        TCP/IP tasks
 *----------------------------------------------------------------------------*/
#ifdef WOLFSSL_KEIL_TCP_NET
__task void tcp_tick (void)
{

    WOLFSSL_MSG("Time tick started.") ;
    #if defined (HAVE_KEIL_RTX)
    os_itv_set (10);
    #endif

    while (1) {
        #if defined (HAVE_KEIL_RTX)
        os_itv_wait ();
        #endif
        /* Timer tick every 100 ms */
        timer_tick ();
    }
}

__task void tcp_poll (void)
{
    WOLFSSL_MSG("TCP polling started.") ;
    while (1) {
        main_TcpNet ();
        #if defined (HAVE_KEIL_RTX)
        os_tsk_pass ();
        #endif
    }
}
#endif

#if defined(HAVE_KEIL_RTX) && defined(WOLFSSL_MDK_SHELL)
#define SHELL_STACKSIZE 1000
static unsigned char Shell_stack[SHELL_STACKSIZE] ;
#endif


#if  defined(WOLFSSL_MDK_SHELL)
extern void shell_main(void) ;
#endif

extern void time_main(int) ;
extern void benchmark_test(void) ;
extern void SER_Init(void) ;

/*-----------------------------------------------------------------------------
 *       mian entry
 *----------------------------------------------------------------------------*/

/*** This is the parent task entry ***/
void main_task (void)
{
    #ifdef WOLFSSL_KEIL_TCP_NET
    init_TcpNet ();

    os_tsk_create (tcp_tick, 2);
    os_tsk_create (tcp_poll, 1);
    #endif

    #ifdef WOLFSSL_MDK_SHELL
        #ifdef  HAVE_KEIL_RTX
           os_tsk_create_user(shell_main, 1, Shell_stack, SHELL_STACKSIZE) ;
       #else
           shell_main() ;
       #endif
    #else

    /************************************/
    /*** USER APPLICATION HERE        ***/
    /************************************/
    printf("USER LOGIC STARTED\n") ;

    #endif

    #ifdef   HAVE_KEIL_RTX
    WOLFSSL_MSG("Terminating tcp_main") ;
    os_tsk_delete_self ();
    #endif

}


    int myoptind = 0;
    char* myoptarg = NULL;

#if defined(DEBUG_WOLFSSL)
    extern void wolfSSL_Debugging_ON(void) ;
#endif


/*** main entry ***/
extern void 	SystemInit(void);

int main() {

    SystemInit();
    #if !defined(NO_FILESYSTEM)
    init_card () ;     /* initializing SD card */
    #endif

    #if defined(DEBUG_WOLFSSL)
         printf("Turning ON Debug message\n") ;
         wolfSSL_Debugging_ON() ;
    #endif

    #ifdef   HAVE_KEIL_RTX
        os_sys_init (main_task) ;
    #else
        main_task() ;
    #endif

    return 0 ; /* There should be no return here */

}
