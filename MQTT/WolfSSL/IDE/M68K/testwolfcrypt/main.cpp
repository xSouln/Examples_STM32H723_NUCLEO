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

#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <random.h>
#include <init.h>

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfcrypt/test/test.h>

extern "C" {
void UserMain(void * pd);
}

const char * AppName="testwolfcrypt";

typedef struct func_args {
    int argc;
    char** argv;
    int return_code;
} func_args;


void UserMain(void * pd) {
    InitializeStack();
    GetDHCPAddressIfNecessary();
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();


    init();
    iprintf("wolfcrypt test Application started\n\r");

    iprintf("waiting for sufficient entropy before starting...\n\r");
    iprintf("looks like NetBurner is using uart/tcp to seed GetRandomX so ..."
            " input enough uart characters.\n\r");
    {
        BYTE b;
        do {
            b = GetRandomByte();
            iprintf(".");
        } while (!RandomValid());
        iprintf("\n\r");
        (void)b;
    }

    /* run wolfCrypt tests */
    {
        func_args args;
        args.argc = 0;
        args.argv = NULL;

        wolfcrypt_test(&args);
    }
    while (1) {
        OSTimeDly(TICKS_PER_SECOND);
    }
}
