/* retarget.c
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */


#include "hw.h"
#include "user_settings.h"
#include <stdio.h>

void __assert(const char *__expression, const char *__filename, int __line)
{
    printf("Assert: %s, File %s (%d)\n", __expression, __filename, __line);
}

unsigned long ksdk_time(unsigned long* timer)
{
    (void)timer;
    return hw_get_time_sec();
}

unsigned int LowResTimer(void)
{
    return hw_get_time_sec();
}

double current_time(int reset)
{
    double time;
    (void)reset;
    time = hw_get_time_sec();
    time += (double)hw_get_time_msec() / 1000;
    return time;
}

unsigned int custom_rand_generate(void)
{
    return hw_rand();
}

int custom_rand_generate_block(unsigned char* output, unsigned int sz)
{
    uint32_t i = 0;

    while (i < sz)
    {
        /* If not aligned or there is odd/remainder */
        if( (i + sizeof(CUSTOM_RAND_TYPE)) > sz ||
            ((uint32_t)&output[i] % sizeof(CUSTOM_RAND_TYPE)) != 0
        ) {
            /* Single byte at a time */
            output[i++] = (unsigned char)custom_rand_generate();
        }
        else {
            /* Use native 8, 16, 32 or 64 copy instruction */
            *((CUSTOM_RAND_TYPE*)&output[i]) = custom_rand_generate();
            i += sizeof(CUSTOM_RAND_TYPE);
        }
    }

    return 0;
}

// Debug print handler
int __putchar(int c, __printf_tag_ptr ctx)
{
    hw_uart_printchar(c);
}

extern unsigned char __stack_process_start__[];
unsigned char * __aeabi_read_tp(void)
{
    // thread-local storage addressing refers to the thread pointer
    // This is returning start address of stack process
    return (__stack_process_start__);
}

/* Stubs */
void __heap_lock(void)
{
}

void __heap_unlock(void)
{
}

void __printf_lock(void)
{
}

void __printf_unlock(void)
{
}

void __scanf_lock(void)
{
}

void __scanf_unlock(void)
{
}

void __debug_io_lock(void)
{
}

void __debug_io_unlock(void)
{
}
