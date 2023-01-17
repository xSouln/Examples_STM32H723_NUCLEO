/* minimum-startup.c
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */

#include <stdint.h>
#pragma language=extended

extern void __iar_program_start(void);
static void Reset(void)
{
    __iar_program_start();
}

static void Nmi  (void){ while(1) ; }
static void Fault(void){ while(1) ; }

static unsigned long long Stack[256*4*16] @ ".noinit";

typedef union
{
    void (*Handler)(void);
    uint32_t Ptr;
} Vector;

__root const Vector  __vector_table[100] @ ".intvec" =
{
    { .Ptr = (uint32_t)Stack + sizeof(Stack) },
                                // stack top
    Reset,                   // reset
    Nmi,                        // NMI
    Fault,                      // hard fault
    Fault,                      // MPU fault
    Fault,                      // bus fault
    Fault,                      // usage fault
};

