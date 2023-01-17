/* pie_redirect_table.c -- module load/unload hooks for libwolfssl.ko
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */

#ifndef __PIE__
    #error pie_redirect_table.c must be compiled -fPIE.
#endif

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/ssl.h>

/* compiling -fPIE results in references to the GOT or equivalent thereof, which remain after linking
 * even if all other symbols are resolved by the link.  naturally there is no
 * GOT in the kernel, and the wolfssl Kbuild script explicitly checks that no
 * GOT relocations occur in the PIE objects, but we still need to include a
 * dummy value here, scoped to the module, to eliminate the otherwise unresolved
 * symbol.
 */
#if defined(CONFIG_X86)
    extern void * const _GLOBAL_OFFSET_TABLE_;
    void * const _GLOBAL_OFFSET_TABLE_ = 0;
#elif defined(CONFIG_MIPS)
  extern void * const _gp_disp;
  void * const _gp_disp = 0;
#endif

struct wolfssl_linuxkm_pie_redirect_table wolfssl_linuxkm_pie_redirect_table;

const struct wolfssl_linuxkm_pie_redirect_table
*wolfssl_linuxkm_get_pie_redirect_table(void) {
    return &wolfssl_linuxkm_pie_redirect_table;
}

/* placeholder implementations for missing functions. */
#if defined(CONFIG_MIPS)
    #undef memcpy
    void *memcpy(void *dest, const void *src, size_t n) {
        char *dest_i = (char *)dest;
        char *dest_end = dest_i + n;
        char *src_i = (char *)src;
        while (dest_i < dest_end)
            *dest_i++ = *src_i++;
        return dest;
    }

    #undef memset
    void *memset(void *dest, int c, size_t n) {
        char *dest_i = (char *)dest;
        char *dest_end = dest_i + n;
        while (dest_i < dest_end)
            *dest_i++ = c;
        return dest;
    }
#endif
