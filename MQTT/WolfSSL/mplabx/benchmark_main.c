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

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfcrypt/benchmark/benchmark.h>

#if defined(WOLFSSL_MICROCHIP_PIC32MZ)
    #define MICROCHIP_PIC32
    #include <stdio.h>
    #include <stdlib.h>
    #include "PIC32MZ-serial.h"
    #include <xc.h>
    #define SYSTEMConfigPerformance(a) /* void out SYSTEMConfigPerformance(); */
    #define SYS_CLK 200000000
#else
    #define PIC32_STARTER_KIT
    #define _SUPPRESS_PLIB_WARNING
    #define _DISABLE_OPENADC10_CONFIGPORT_WARNING
    #include <plib.h>
    #include <sys/appio.h>
    #define init_serial() /* void out init_serial() ; */
    #define SYS_CLK 80000000
#endif

#if 1
/* enable this if ReadCoreTimer and WriteCoreTimer are missing */
unsigned int ReadCoreTimer(void)
{
    unsigned int timer;
    timer = __builtin_mfc0(9, 0);
    return timer;
}
void WriteCoreTimer(unsigned int t)
{
    /* do nothing here */
    (void)t;
}
#endif

/*
 * Main driver for wolfCrypt benchmarks.
 */
int main(int argc, char** argv)
{
    SYSTEMConfigPerformance(SYS_CLK);
    DBINIT();

    init_serial(SYS_CLK) ;  /* initialize PIC32MZ serial I/O */

    printf("wolfCrypt Benchmark:\n");

    benchmark_test(NULL);

    printf("End of wolfCrypt Benchmark:\n");
    return 0;
}

