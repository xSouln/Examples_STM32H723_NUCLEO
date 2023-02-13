//==============================================================================
#ifndef FLASH_MANAGER_H
#define FLASH_MANAGER_H
//==============================================================================
#include "task.h"
#include "leds.h"
#include "Devices.h"
//------------------------------------------------------------------------------
// Hermes Flash Memory Map


//------------------------------------------------------------------------------
// Mailbox data types for handling NV Store
typedef enum
{
    FM_DEVICE_STATS,
    FM_PRODUCT_CONFIG,
    FM_FACTORY_TEST,
    FM_AES_DATA,
    FM_PERSISTENT_DATA,
    FM_UPDATE_FW,
    FM_ERASE_SECTOR,
    FM_TEST,
	FM_GENERIC_WRITE,
	FM_GENERIC_ERASE,
	FM_CREDENTIAL,

} FM_STORE_DATA_TYPE;
//------------------------------------------------------------------------------
typedef enum
{
    FM_NAK = 1 << 0,
    FM_ACK = 1 << 1,
    FM_GET = 1 << 2,
    FM_PUT = 1 << 3,
    FM_ERASE = 1 << 4,
    FM_UPDATE = 1 << 5,
    FM_FW_BLOCK = 1 << 6,
    FM_FW_ERASE_BANK = 1 << 7,
    FM_FW_CHECK = 1 << 8,
    FM_FW_FLIP = 1 << 9,

} FM_STORE_ACTION;
//------------------------------------------------------------------------------
typedef struct 
{
    FM_STORE_ACTION		action;
	FM_STORE_DATA_TYPE	type;
	uint8_t*			ptrToBuf;
	int32_t				dataLength;   
	int16_t				sectorNum;

	// offset from base... 0,000 to 1f7,000
	int32_t				offsetSectorAddr;

	// identify who sent this message
    TaskHandle_t		xClientTaskHandle;

} SEND_TO_FM_MSG;
//------------------------------------------------------------------------------
typedef struct 
{
    FM_STORE_ACTION action;
	FM_STORE_DATA_TYPE type;
	uint8_t *ptrToBuf;
	int16_t dataLength;

} RESP_FROM_FM_MSG;
//------------------------------------------------------------------------------
typedef enum
{
    FM_CRC_DEV_STATS        = 1016,
    FM_CRC_DEV_STATS_POSN   = 0,
    FM_CRC_MQTT_CERT        = 1017,
    FM_CRC_MQTT_CERT_POSN   = 2,
    FM_CRC_MQTT_KEY         = 1018,
    FM_CRC_MQTT_KEY_POSN    = 4,
    FM_CRC_FT_DATA          = 1019,
    FM_CRC_FT_DATA_POSN     = 6,
    FM_CRC_MAC_SERIAL       = 1020,
    FM_CRC_MAC_SERIAL_POSN  = 8,
    FM_CRC_KEYS             = 1021,
    FM_CRC_KEYS_POSN        = 10,
// 1022 - spare    
    FM_CRC_PERSISTENT       = 1023,
    FM_CRC_PERSISTENT_POSN  = 14,

} CRC_DATA_AREA;
//------------------------------------------------------------------------------
typedef enum
{
    FM_UFW_NULL,
    FM_UFW_FIRST_BLOCK,
    FM_UFW_IN_PROGRESS,
    FM_UFW_DONE,

} UFW_STATE;
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
#endif  // FLASH_MANAGER_H


