/* time-dummy.c.c
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

#include "time.h"

struct tm *wolfssl_MDK_gmtime(const time_t *c)
{
    static struct tm date ;
    return(&date) ;
}

time_t time(time_t * t) { return 0 ; }
