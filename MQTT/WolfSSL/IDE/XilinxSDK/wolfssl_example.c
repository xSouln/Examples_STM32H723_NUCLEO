/* wolfssl_example.c
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */

#include "xil_printf.h"
#include "xrtcpsu.h"

#include "wolfssl/wolfcrypt/settings.h"
#include "wolfssl/wolfcrypt/wc_port.h"
#include "wolfssl/wolfcrypt/error-crypt.h"
#include "wolfssl/wolfcrypt/logging.h"
#include "wolfcrypt/test/test.h"
#include "wolfcrypt/benchmark/benchmark.h"

/*****************************************************************************
 * Configuration
 ****************************************************************************/

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
typedef struct func_args {
	int argc;
	char** argv;
	int return_code;
} func_args;

const char menu1[] = "\n"
		"\tt. WolfCrypt Test\n"
		"\tb. WolfCrypt Benchmark\n";

/*****************************************************************************
 * Private functions
 ****************************************************************************/
/* Test RNG Seed Function */
/* TODO: Must provide real seed to RNG */
unsigned char my_rng_seed_gen(void)
{
	static unsigned int kTestSeed = 1;
	return kTestSeed++;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/
int main()
{
	uint8_t cmd;
	func_args args;

#ifdef DEBUG_WOLFSSL
    wolfSSL_Debugging_ON();
#endif

    /* initialize wolfSSL */
    wolfCrypt_Init();

	while (1) {
        memset(&args, 0, sizeof(args));
        args.return_code = NOT_COMPILED_IN; /* default */

		xil_printf("\n\t\t\t\tMENU\n");
		xil_printf(menu1);
		xil_printf("Please select one of the above options:\n");

        do {
        	cmd = inbyte();
        } while (cmd == '\n' || cmd == '\r');

		switch (cmd) {
		case 't':
			xil_printf("Running wolfCrypt Tests...\n");
        #ifndef NO_CRYPT_TEST
			args.return_code = 0;
			wolfcrypt_test(&args);
        #endif
			xil_printf("Crypt Test: Return code %d\n", args.return_code);
			break;

		case 'b':
			xil_printf("Running wolfCrypt Benchmarks...\n");
        #ifndef NO_CRYPT_BENCHMARK
			args.return_code = 0;
			benchmark_test(&args);
        #endif
			xil_printf("Benchmark Test: Return code %d\n", args.return_code);
			break;

		default:
			xil_printf("\nSelection out of range\n");
			break;
		}
	}

    wolfCrypt_Cleanup();

    return 0;
}
