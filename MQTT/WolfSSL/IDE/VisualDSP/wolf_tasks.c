/* wolf-tasks.c
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfcrypt/test/test.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/fips_test.h>
#ifdef FUSION_RTOS
#include <fcl_os.h>

#define RESULT_BUF_SIZE  1024

typedef struct {
   int   isRunning;
   u8    buf[RESULT_BUF_SIZE];
   int   len;
} wolf_result_t;

static wolf_result_t _result = {0};

static void myFipsCb(int ok, int err, const char* hash);

static void myFipsCb(int ok, int err, const char* hash)
{

    FCL_PRINTF("in my Fips callback, ok = %d, err = %d\n", ok, err);
    FCL_PRINTF("message = %s\n", wc_GetErrorString(err));
    FCL_PRINTF("hash = %s\n", hash);

    if (err == IN_CORE_FIPS_E) {
        FCL_PRINTF("In core integrity hash check failure, copy above hash\n");
        FCL_PRINTF("into verifyCore[] in fips_test.c and rebuild\n");
    }
}

static fclThreadHandle _task = NULL;
#define WOLF_TASK_STACK_SIZE (1024 * 100)
fclThreadPriority WOLF_TASK_PRIORITY = (fclThreadPriority) (FCL_THREAD_PRIORITY_TIME_CRITICAL+1);

int wolfcrypt_test_taskEnter(void *args)
{
    int ret;

    wolfCrypt_SetCb_fips(myFipsCb);

    ret = wolfcrypt_test(args);

    printf("Result of test was %d\n", ret);

    _result.isRunning = 0;
    fosTaskDelete(_task);
    return 0;
}

/* Was only needed for CAVP testing purposes, not required for release.
int wolfcrypt_harness_taskEnter(void *args)
{
    wolfCrypt_SetCb_fips(myFipsCb);
    wolfcrypt_harness(args);
    _result.isRunning = 0;
    fosTaskDelete(_task);
    return 0;
}
*/

int wolf_task_start(void* voidinfo, char* argline)
{
    char optionA[] = "wolfcrypt_test";
    fssShellInfo *info = (fssShellInfo*)voidinfo;
    struct wolfArgs args;

    if (_result.isRunning) {
        fssShellPuts(info, "previous task still running\r\n");
        return 1;
    }

    _result.isRunning = 1;

    if (FCL_STRNCMP(argline, optionA, FCL_STRLEN(optionA)) == 0) {
         _task = fclThreadCreate(wolfcrypt_test_taskEnter,
                                 &args,
                                 WOLF_TASK_STACK_SIZE,
                                 WOLF_TASK_PRIORITY);
    } else if (FCL_STRNCMP(argline, optionB, FCL_STRLEN(optionB)) == 0) {
        _task = fclThreadCreate(wolfcrypt_harness_taskEnter,
                                &args,
                                WOLF_TASK_STACK_SIZE,
                                WOLF_TASK_PRIORITY);
    } else {
        printf("Invalid input: %s\n", argline);
        printf("Please try with either wolfcrypt_test or wolfcrypt_harness\n");
        return -1;
    }

    FCL_ASSERT(_task != FCL_THREAD_HANDLE_INVALID);

    return 0;
}
#endif /* FUSION_RTOS */
