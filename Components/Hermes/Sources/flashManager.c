/*****************************************************************************
*
* SUREFLAP CONFIDENTIALITY & COPYRIGHT NOTICE
*
* Copyright � 2013-2021 Sureflap Limited.
* All Rights Reserved.
*
* All information contained herein is, and remains the property of Sureflap 
* Limited.
* The intellectual and technical concepts contained herein are proprietary to
* Sureflap Limited. and may be covered by U.S. / EU and other Patents, patents 
* in process, and are protected by copyright law.
* Dissemination of this information or reproduction of this material is 
* strictly forbidden unless prior written permission is obtained from Sureflap 
* Limited.
*
* Filename: flashManager.c   
* Author:   Tony Thurgood
* Purpose:  19/9/2019  
*   
* Flash Manager methods to handle qSPI flash memory and NV Store
*            
**************************************************************************/
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* FreeRTOS+IP includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*Other includes*/
#include "Devices.h"
#include "stdbool.h"
#include "flashManager.h"
#include "credentials.h"
#include "utilities.h"
//==============================================================================
// declared in hermes.c
extern TaskHandle_t xHermesTestTaskHandle;
extern TaskHandle_t xFM_hermesFlashTaskHandle;

// This is a RAM copy of the product info from Flash.
extern PRODUCT_CONFIGURATION product_configuration;
//==============================================================================
// Shared mailboxes
QueueHandle_t xNvStoreMailboxSend;
QueueHandle_t xNvStoreMailboxResp;

#define HERMES_FLASH_BASE_ADDRESS FLASH_BANK1_BASE
#define HERMES_FLASH_END_ADDRESS FLASH_END
#define HERMES_FLASH_SIZE (HERMES_FLASH_END_ADDRESS - HERMES_FLASH_BASE_ADDRESS)
#define HERMES_FLASH_PAGE_SIZE 128000

//start address is 896000
#define HERMES_FLASH_OFFSET_ADDRESS 0x20000 * 7
//==============================================================================
typedef struct
{
	union
	{
		PRODUCT_CONFIGURATION product_configuration;
		uint8_t product_configuration_section[1024];
	};

	union
	{
		DEVICE_STATUS device_status[MAX_NUMBER_OF_DEVICES];
		uint8_t device_status_section[1024];
	};

	union
	{
		AES_CONFIG aes_config;
		uint8_t aes_config_section[128];
	};

	union
	{
		PERSISTENT_DATA persistent_data;
		uint8_t persistent_data_section[128];
	};

	union
	{
		FACTORY_TEST_DATA factory_test_data;
		uint8_t factory_test_data_section[128];
	};

} HermesFlashOperationBufferT;
//==============================================================================
static SemaphoreHandle_t FlashOperationMutex = 0;

static struct
{
	uint32_t OperationBufferChanged : 1;

} FlashOperationStates;

HermesFlashOperationBufferT HermesFlashOperationBuffer HERMES_FLASH_OPERATION_BUFFER_MEM_SECTION;
/*******************************************************************************
 * Flash Manager Initialisation
 ******************************************************************************/
static void HermesFlashOperationLock()
{
	xSemaphoreTake(FlashOperationMutex, portMAX_DELAY);
}
//------------------------------------------------------------------------------
static void HermesFlashOperationUnlock()
{
	xSemaphoreGive(FlashOperationMutex);
}
//------------------------------------------------------------------------------
FlashOperationResultT HermesFlashErase()
{
	uint32_t sector_error;

	FLASH_EraseInitTypeDef request =
	{
		.Banks = FLASH_BANK_1,
		.Sector = FLASH_SECTOR_7,
		.NbSectors = 1,
		.TypeErase = FLASH_TYPEERASE_SECTORS,
		.VoltageRange = FLASH_VOLTAGE_RANGE_3
	};

	HAL_FLASH_Unlock();
	HAL_StatusTypeDef result = HAL_FLASHEx_Erase(&request, &sector_error);
	HAL_FLASH_Lock();

	switch ((uint8_t)result)
	{
		case HAL_OK: return xResultAccept;
		case HAL_BUSY: return xResultBusy;
		case HAL_TIMEOUT: return xResultTimeOut;
	}

	return 0;
}
//------------------------------------------------------------------------------
FlashOperationResultT HermesFlashRead(uint32_t offset, void* out, uint32_t size)
{
	volatile uint8_t* mem = (volatile uint8_t*)(FLASH_BANK1_BASE + offset);
	uint8_t* ptr = out;

	for (uint32_t i = 0; i < size; i++)
	{
		ptr[i] = mem[i];
	}

	return 0;
}
//------------------------------------------------------------------------------
FlashOperationResultT HermesFlashWrite(uint32_t offset, void* in, uint32_t size)
{
	FlashOperationResultT result = 0;

	uint8_t* ptr = in;
	uint32_t i = 0;
	uint8_t byte_number;
	uint32_t address = FLASH_BANK1_BASE + offset;

	typedef union
	{
		uint32_t Words[FLASH_NB_32BITWORD_IN_FLASHWORD];
		uint8_t Bytes[sizeof(uint32_t) * FLASH_NB_32BITWORD_IN_FLASHWORD];

	} FlashWordT;

	FlashWordT FlashWord;

	memset(&FlashWord, 0xff, sizeof(FlashWord));

	HAL_FLASH_Unlock();

	while (i < size)
	{
		byte_number = 0;

		while (i < size && byte_number < sizeof(FlashWord))
		{
			FlashWord.Bytes[byte_number++] = ptr[i];
			i++;
		}

		if (HAL_FLASH_Program(0, address, (uint32_t)&FlashWord) != HAL_OK)
		{
			result = -1;
			break;
		}

		address += byte_number;
	}

	HAL_FLASH_Lock();

	return result;
}
//------------------------------------------------------------------------------
FlashOperationResultT HermesFlashReadEx(uint32_t offset, void* out, uint32_t size)
{
	FlashOperationResultT result;

	HermesFlashOperationLock();

	result = HermesFlashRead(offset, out, size);

	HermesFlashOperationUnlock();

	return result;
}
//------------------------------------------------------------------------------
static FlashOperationResultT HermesReadOperationBuffer()
{
	return HermesFlashRead(HERMES_FLASH_OFFSET_ADDRESS,
			&HermesFlashOperationBuffer,
			sizeof(HermesFlashOperationBuffer));
}
//------------------------------------------------------------------------------
static FlashOperationResultT HermesWriteOperationBuffer()
{
	return HermesFlashWrite(HERMES_FLASH_OFFSET_ADDRESS,
			&HermesFlashOperationBuffer,
			sizeof(HermesFlashOperationBuffer));
}
//------------------------------------------------------------------------------
int HermesFlashSaveData()
{
	HermesFlashOperationLock();

	HermesFlashErase();
	HermesWriteOperationBuffer();

	HermesFlashOperationUnlock();

	return 0;
}
//------------------------------------------------------------------------------
int HermesFlashReadData()
{
	HermesReadOperationBuffer();

	return 0;
}
//------------------------------------------------------------------------------
int HermesFlashSetProductConfig(PRODUCT_CONFIGURATION* in)
{
	HermesFlashOperationLock();

	memcpy(&HermesFlashOperationBuffer.product_configuration, in, sizeof(PRODUCT_CONFIGURATION));
	FlashOperationStates.OperationBufferChanged = true;

	HermesFlashOperationUnlock();

	return 0;
}
//------------------------------------------------------------------------------
int HermesFlashReadProductConfig(PRODUCT_CONFIGURATION* out)
{
	uint32_t offset = HERMES_FLASH_OFFSET_ADDRESS;
	offset += (uint32_t)&HermesFlashOperationBuffer.product_configuration - (uint32_t)&HermesFlashOperationBuffer;

	HermesFlashReadEx(offset, out, sizeof(PRODUCT_CONFIGURATION));
	//memcpy(config, &HermesFlashOperationBuffer.product_configuration, sizeof(PRODUCT_CONFIGURATION));

	return 0;
}
//------------------------------------------------------------------------------
int HermesFlashSetDeviceTable(DEVICE_STATUS* in)
{
	HermesFlashOperationLock();

	memcpy(&HermesFlashOperationBuffer.device_status, in, sizeof(HermesFlashOperationBuffer.device_status));
	FlashOperationStates.OperationBufferChanged = true;

	HermesFlashOperationUnlock();

	return 0;
}
//------------------------------------------------------------------------------
int HermesFlashReadDeviceTable(DEVICE_STATUS* out)
{
	uint32_t offset = HERMES_FLASH_OFFSET_ADDRESS;
	offset += (uint32_t)&HermesFlashOperationBuffer.device_status - (uint32_t)&HermesFlashOperationBuffer;

	HermesFlashReadEx(offset, out, sizeof(HermesFlashOperationBuffer.device_status));

	return 0;
}
/**************************************************************
 * Function Name   : FM_hermesFlashInit
 * Description     : Sets up Flash Manager
 * Inputs          : 
 * Outputs         : none
 * Returns         : void
 **************************************************************/
FlashOperationResultT HermesFlashInit(void)
{
	FlashOperationMutex = xSemaphoreCreateMutex();

	return 0;
}
//==============================================================================
