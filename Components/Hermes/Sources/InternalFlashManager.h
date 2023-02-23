//==============================================================================
#ifndef _INTERNAL_FLASH_MANAGER_H_
#define _INTERNAL_FLASH_MANAGER_H_
//==============================================================================
//includes:

#include "Hermes-compiller.h"

#include "hermes.h"
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
//buffer for writing data to flash memory
typedef struct
{
	union
	{
		uint8_t bootloader_flags_section[1024];
	};

	union
	{
		uint8_t reserved_section1[1024];
	};

	union
	{
		PRODUCT_CONFIGURATION product_configuration;

		//the maximum memory size of the section, also when changing the size of the structure,
		//save the position of the rest of the data not included in this section
		uint8_t product_configuration_section[1024];
	};

	union
	{
		PERSISTENT_DATA persisent_data;
		uint8_t persisent_data_section[256];
	};

	union
	{
		AES_CONFIG aes_config;
		uint8_t aes_config_section[256];
	};

	union
	{
		FACTORY_TEST_DATA factory_test_data;
		uint8_t factory_test_data_section[256];
	};

	union
	{
		uint8_t reserved_section2[1280];
	};

	union
	{
		DEVICE_STATUS device_status[MAX_NUMBER_OF_DEVICES];
		uint8_t device_status_section[1024];
	};

} HermesFlashOperationBufferT;
//------------------------------------------------------------------------------

typedef struct
{
	struct
	{
		uint32_t OperationBufferChanged : 1;

	} State;

	HermesFlashOperationBufferT* Buffer;

} HermesFlashManagerT;
//==============================================================================
//externs:

extern HermesFlashManagerT HermesFlashManager;
//==============================================================================
// Prototypes

FlashOperationResultT HermesFlashInit(void);

FlashOperationResultT HermesFlashSetProductConfig(PRODUCT_CONFIGURATION* in);
FlashOperationResultT HermesFlashReadProductConfig(PRODUCT_CONFIGURATION* out);

FlashOperationResultT HermesFlashSetPersisentData(PERSISTENT_DATA* in);
FlashOperationResultT HermesFlashReadPersisentData(PERSISTENT_DATA* out);

FlashOperationResultT HermesFlashSetDeviceTable(DEVICE_STATUS* in);
FlashOperationResultT HermesFlashReadDeviceTable(DEVICE_STATUS* out);

FlashOperationResultT HermesFlashSetFactoryTestData(FACTORY_TEST_DATA* in);
FlashOperationResultT HermesFlashReadFactoryTestData(FACTORY_TEST_DATA* out);

FlashOperationResultT HermesFlashReadData();
FlashOperationResultT HermesFlashSaveData();
//==============================================================================
#endif // _INTERNAL_FLASH_MANAGER_H_


