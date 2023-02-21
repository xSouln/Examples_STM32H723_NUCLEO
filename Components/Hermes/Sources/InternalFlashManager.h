//==============================================================================
#ifndef _INTERNAL_FLASH_MANAGER_H_
#define _INTERNAL_FLASH_MANAGER_H_
//==============================================================================
//includes:

#include "task.h"
#include "leds.h"
#include "Devices.h"
//==============================================================================
//types:

//------------------------------------------------------------------------------
// data definitions for Programmer test results
typedef struct
{
    uint32_t standbyTest : 1;
    uint32_t greenTest : 1;
    uint32_t redTest : 1;
    uint32_t rfTest : 1;

} TEST_PASS;
//------------------------------------------------------------------------------
typedef union
{
    TEST_PASS testPass;

    // so we can track the pass/fails in a word32
    uint32_t all;

} PASS_RESULTS;
//------------------------------------------------------------------------------
typedef struct
{
	uint32_t		testRevNum;
	uint32_t		standby_mA;
	uint32_t		green_mA;
	uint32_t		red_mA;

	// I don't expect to see a failure
    PASS_RESULTS    passResults;

} FACTORY_TEST_DATA;
//------------------------------------------------------------------------------
typedef struct
{
	uint64_t		aes_high;
	uint64_t		aes_low;

} AES_CONFIG;
//------------------------------------------------------------------------------
// used to store current Hermes settings for POR continuity
typedef struct
{
	LED_MODE		brightness;
	uint8_t		    printLevel;
	uint8_t			RF_channel;
	uint8_t			spare_1_2;
	uint8_t			spare_1_3;
	uint8_t			spare_1_4;
	uint32_t		spare_2;

} PERSISTENT_DATA;
//------------------------------------------------------------------------------
typedef enum
{
	PERSISTENT_BRIGHTNESS,
	PERSISTENT_PRINTLEVEL,
	PERSISTENT_RF_CHANNEL,

} PERSISTENT_STORE_INDEX;
//------------------------------------------------------------------------------
typedef int FlashOperationResultT;
//------------------------------------------------------------------------------
// Prototypes

int HermesFlashInit(void);

int HermesFlashSetProductConfig(PRODUCT_CONFIGURATION* in);
int HermesFlashReadProductConfig(PRODUCT_CONFIGURATION* out);

int HermesFlashSetDeviceTable(DEVICE_STATUS* in);
int HermesFlashReadDeviceTable(DEVICE_STATUS* out);

int HermesFlashReadData();
int HermesFlashSaveData();
//==============================================================================
#endif // _INTERNAL_FLASH_MANAGER_H_


