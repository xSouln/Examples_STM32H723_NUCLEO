/* target.ld
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */

/* IoT-safe example
 * Linker script for STM32L4
 */


MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1M
    SRAM1_STACK (rw) : ORIGIN = 0x20000000, LENGTH = 16K
    SRAM1(rw) : ORIGIN = 0x20000000 + 16K, LENGTH = 256K - 16K
    SRAM2 (rw) : ORIGIN = 0x20040000, LENGTH = 64K
}

SECTIONS
{
    .text :
    {
        _start_text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _end_text = .;
    } > FLASH

    .edidx :
    {
        . = ALIGN(4);
        *(.ARM.exidx*)
    } > FLASH

    _stored_data = .;

    .data : AT (_stored_data)
    {
        _start_data = .;
        *(.data*)
        . = ALIGN(4);
        _end_data = .;
    } > SRAM1

    .bss :
    {
        _start_bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _end_bss = .;
        _end = .;
    } > SRAM1 

}

PROVIDE(_start_heap = ORIGIN(SRAM2));
PROVIDE(_end_stack  = ORIGIN(SRAM1_STACK) + LENGTH(SRAM1_STACK));
