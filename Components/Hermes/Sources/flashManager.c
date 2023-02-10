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

// declared in hermes.c
extern TaskHandle_t xHermesTestTaskHandle;
extern TaskHandle_t xFM_hermesFlashTaskHandle;

// This is a RAM copy of the product info from Flash.
extern PRODUCT_CONFIGURATION product_configuration;

// Shared mailboxes
QueueHandle_t xNvStoreMailboxSend;
QueueHandle_t xNvStoreMailboxResp;

#define HERMES_FLASH_BASE_ADDRESS
#define HERMES_FLASH_OPERATION_BUFFER_SIZE 32768
#define HERMES_FLASH_OPERATION_BUFFER_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))

uint8_t flash_operation_buffer[HERMES_FLASH_OPERATION_BUFFER_SIZE] HERMES_FLASH_OPERATION_BUFFER_MEM_SECTION;

// Local Functions:
static int hermesFlashWriteCredential(SEND_TO_FM_MSG* msg);
/*******************************************************************************
 * Flash Manager Initialisation
 ******************************************************************************/

static FlashOperationResultT HermesFlashWrite(uint32_t address, void* data, uint32_t size)
{
	uint8_t* ptr = data;


}
/**************************************************************
 * Function Name   : FM_hermesFlashInit
 * Description     : Sets up Flash Manager
 * Inputs          : 
 * Outputs         : none
 * Returns         : void
 **************************************************************/
FlashOperationResultT FM_hermesFlashInit(void)
{
    static FlashOperationResultT nvStoreStatus;
    FlashOperationResultT ret;
/*
	// incoming mailbox queue for NV requests
	xNvStoreMailboxSend = xQueueCreate(1, sizeof(SEND_TO_FM_MSG));

	// outgoing mailbox queue for NV responses
	xNvStoreMailboxResp = xQueueCreate(1, sizeof(RESP_FROM_FM_MSG));

        if (product_configuration.product_state == PRODUCT_CONFIGURED)
        {
        	// Normal start-up, so check NV store for corruption - NOT sure what we are doing with this?
            nvStoreStatus = FM_crcIntegrityCheck();
            if (!nvStoreStatus)
            {
				//zprintf(LOW_IMPORTANCE,"\r\nNV Store CRC status is good. \r\n");
				ret = kStatus_Success;
            }
            else
            {
				ret = kStatus_Fail;
				//zprintf(CRITICAL_IMPORTANCE,"\r\n\nNV Store CRC FAIL - kStatus_Fail \r\n\n");   // <<<< Now what?
            }
        }
        */
	return ret;
}

/**************************************************************
 * Function Name   : FM_hermesFlashTask
 * Description     : FreeRtos do forever task - State m/c driven by incoming msgs
 * Inputs          : pvParameters - std parameter
 * Outputs         : none
 * Returns         : void
 **************************************************************/
void FM_hermesFlashSaveProductConfig(PRODUCT_CONFIGURATION* config)
{
	HAL_FLASH_Program(FLASH_PSIZE_BYTE, uint32_t FlashAddress, uint32_t DataAddress);
}


void FM_hermesFlashTask(void *pvParameters)
{
	/*
    status_t		status = kStatus_Fail;
    SEND_TO_FM_MSG	nvmIncomingMsg;
	uint32_t		i;
    
    while( 1 )
    {
		// Check the queue for incoming messages.
		if(xQueueReceive(xNvStoreMailboxSend, &nvmIncomingMsg, pdMS_TO_TICKS(5)) == pdPASS)
		{
			// Service the incoming message, type and action
			switch(nvmIncomingMsg.type)
			{
				case FM_PRODUCT_CONFIG:
					if( nvmIncomingMsg.action == FM_PUT )
					{
						// Data string shoulld be checked for length, but strlen() can't be used, because 00 is a valid char
						if( nvmIncomingMsg.dataLength == sizeof(PRODUCT_CONFIGURATION) ) 
						{
                            status = FM_hermesFlashPageUpdate(&nvmIncomingMsg, FLASH_BASE+(FM_SECTOR_ID*FLASH_SECTOR_SIZE_BYTES));
							// Flush the cache, so subsequent read will be from flash
							DCACHE_InvalidateByRange((uint32_t)(FLASH_BASE+(FM_SECTOR_ID*FLASH_SECTOR_SIZE_BYTES)), nvmIncomingMsg.dataLength);
							FM_hermesFlashNotifyResponse(status, nvmIncomingMsg.xClientTaskHandle);
							FM_crcUpdateAreaCode(FM_CRC_MAC_SERIAL);
						}
						else
						{
							FM_hermesFlashNotifyResponse(kStatus_Fail, nvmIncomingMsg.xClientTaskHandle);
						}
					}
					else
					{
						// Get PRODUCT_CONFIG from NV Store
						FM_hermesFlashElementRead(nvmIncomingMsg.ptrToBuf, (FLASH_BASE+(FM_SECTOR_ID*FLASH_SECTOR_SIZE_BYTES)), nvmIncomingMsg.dataLength, nvmIncomingMsg.xClientTaskHandle);
					}
					break;
				
				case FM_DEVICE_STATS:
					if(nvmIncomingMsg.action == FM_PUT)
					{
						FM_hermesFlashUpdateDevStats(&nvmIncomingMsg, (FLASH_BASE + (FM_SECTOR_STATS * FLASH_SECTOR_SIZE_BYTES)) );
						// Flush the cache, so subsequent read will be from flash
						DCACHE_InvalidateByRange((uint32_t)(FLASH_BASE + (FM_SECTOR_STATS * FLASH_SECTOR_SIZE_BYTES)), nvmIncomingMsg.dataLength);
						FM_crcUpdateAreaCode(FM_CRC_DEV_STATS);
					}
					else
					{ 	// Return the Device Stats from NV area
						FM_hermesFlashReadDevStats(&nvmIncomingMsg, (FLASH_BASE + (FM_SECTOR_STATS * FLASH_SECTOR_SIZE_BYTES)) );
					}
					break;
				
				case FM_FACTORY_TEST:
					if( nvmIncomingMsg.action == FM_PUT )
					{
						// Data string should be checked for length, but strlen() can't be used, because 00 is a valid char
						if( nvmIncomingMsg.dataLength == sizeof(FACTORY_TEST_DATA) ) 
						{
                            status = FM_hermesFlashPageUpdate(&nvmIncomingMsg, FLASH_BASE+(FM_SECTOR_TEST_DATA*FLASH_SECTOR_SIZE_BYTES));
							// Flush the cache, so subsequent read will be from flash
							DCACHE_InvalidateByRange((uint32_t)(FLASH_BASE+(FM_SECTOR_TEST_DATA*FLASH_SECTOR_SIZE_BYTES)), nvmIncomingMsg.dataLength);
							FM_hermesFlashNotifyResponse(status, nvmIncomingMsg.xClientTaskHandle);
							FM_crcUpdateAreaCode(FM_CRC_FT_DATA);
						} else
						{
							FM_hermesFlashNotifyResponse(kStatus_Fail, nvmIncomingMsg.xClientTaskHandle);
						}
					} else
					{ 	// Get TEST_DATA from NV Store
						FM_hermesFlashElementRead(nvmIncomingMsg.ptrToBuf, (FLASH_BASE+(FM_SECTOR_TEST_DATA*FLASH_SECTOR_SIZE_BYTES)), nvmIncomingMsg.dataLength, nvmIncomingMsg.xClientTaskHandle);
					}
					break;
				
				case FM_AES_DATA:
					if( nvmIncomingMsg.action == FM_PUT )
					{
						if( nvmIncomingMsg.dataLength == sizeof(AES_CONFIG) ) 
						{
                            status = FM_hermesFlashPageUpdate(&nvmIncomingMsg, FLASH_BASE+(FM_SECTOR_AES*FLASH_SECTOR_SIZE_BYTES));
							// Flush the cache, so subsequent read will be from flash
							DCACHE_InvalidateByRange((uint32_t)(FLASH_BASE+(FM_SECTOR_AES*FLASH_SECTOR_SIZE_BYTES)), nvmIncomingMsg.dataLength);
							FM_hermesFlashNotifyResponse(status, nvmIncomingMsg.xClientTaskHandle);
							FM_crcUpdateAreaCode(FM_CRC_KEYS);
						} else
						{
							FM_hermesFlashNotifyResponse(kStatus_Fail, nvmIncomingMsg.xClientTaskHandle);
						}
					} else
					{ 	// Get PRODUCT_CONFIG from NV Store
						FM_hermesFlashElementRead(nvmIncomingMsg.ptrToBuf, (FLASH_BASE+(FM_SECTOR_AES*FLASH_SECTOR_SIZE_BYTES)), nvmIncomingMsg.dataLength, nvmIncomingMsg.xClientTaskHandle);
					}
					break;

				case FM_PERSISTENT_DATA:
					if( nvmIncomingMsg.action == FM_UPDATE )
					{
						if( nvmIncomingMsg.dataLength == sizeof(PERSISTENT_DATA) ) 
						{	
							status = FM_hermesFlashPageUpdate(&nvmIncomingMsg, FM_PERSISTENT_PAGE_ADDR);
							// Flush the cache, so subsequent read will be from flash
							DCACHE_InvalidateByRange((uint32_t)FM_PERSISTENT_PAGE_ADDR, nvmIncomingMsg.dataLength);
							FM_hermesFlashNotifyResponse(status, nvmIncomingMsg.xClientTaskHandle);
							FM_crcUpdateAreaCode(FM_CRC_PERSISTENT);
						} else
						{
							FM_hermesFlashNotifyResponse(kStatus_Fail, nvmIncomingMsg.xClientTaskHandle);
						}
					}
					break;
				
				case FM_ERASE_SECTOR:
					 // Exclude startup code and persistent data for now
                    if ((nvmIncomingMsg.sectorNum > FM_SECTOR_EO_STARTUP) && (nvmIncomingMsg.sectorNum <= FM_SECTOR_PERSISTENT))
					{
						status = FM_hermesFlashEraseSector(nvmIncomingMsg.sectorNum * SECTOR_SIZE);
						FM_hermesFlashNotifyResponse(status, nvmIncomingMsg.xClientTaskHandle);
					} else
					{
						FM_hermesFlashNotifyResponse(kStatus_Fail, nvmIncomingMsg.xClientTaskHandle);
					}
					break;
				
				case FM_TEST:                                           // Used for dev testing
					FM_crcUpdateAreaCode(FM_CRC_PERSISTENT);
					break;
					
				case FM_CREDENTIAL:
					status = hermesFlashWriteCredential(&nvmIncomingMsg);
					FM_hermesFlashNotifyResponse(status, nvmIncomingMsg.xClientTaskHandle);
					break;
					
				case FM_GENERIC_WRITE:
					status = kStatus_Success;
					while( (kStatus_Success == status) && (nvmIncomingMsg.dataLength > 0) )
					{
						status = FM_hermesFlashPageWrite(nvmIncomingMsg.offsetSectorAddr - FLASH_BASE, nvmIncomingMsg.ptrToBuf);
						__MEMORY_BARRIER();
				        DCACHE_InvalidateByRange(nvmIncomingMsg.offsetSectorAddr, FLASH_PAGE_SIZE_BYTES);							
						// Read it back via FlexSPI to avoid translation caused by BEE
						FM_hermesFlashPageRead(nvmIncomingMsg.offsetSectorAddr - FLASH_BASE, flash_page_read_buf);
						for( i=0; i<FLASH_PAGE_SIZE_BYTES; i++ )
						{
							if( flash_page_read_buf[i] != nvmIncomingMsg.ptrToBuf[i] )
							{
								status = kStatus_Fail;
							}
						}
						if( kStatus_Fail == status) zprintf(CRITICAL_IMPORTANCE,"Verify fail\r\n");
						nvmIncomingMsg.dataLength -= FLASH_PAGE_SIZE_BYTES;
						nvmIncomingMsg.offsetSectorAddr += FLASH_PAGE_SIZE_BYTES;
						nvmIncomingMsg.ptrToBuf += FLASH_PAGE_SIZE_BYTES;
					}
					FM_hermesFlashNotifyResponse(status, nvmIncomingMsg.xClientTaskHandle);
					break;
					
				case FM_GENERIC_ERASE:
					status = kStatus_Success;
					while( (kStatus_Success == status) && (nvmIncomingMsg.dataLength > 0) )
					{
						status = FM_hermesFlashEraseSector(nvmIncomingMsg.offsetSectorAddr - FLASH_BASE);
						nvmIncomingMsg.dataLength -= FLASH_SECTOR_SIZE_BYTES;
						nvmIncomingMsg.offsetSectorAddr += FLASH_SECTOR_SIZE_BYTES;
					}
					FM_hermesFlashNotifyResponse(status, nvmIncomingMsg.xClientTaskHandle);
					break;
					
				
			} //switch nvmIncomingMsg.type
		} // if( xQueueReceive...
    } // while loop
    */
}

static FlashOperationResultT hermesFlashWriteCredential(SEND_TO_FM_MSG* msg)
{
	FlashOperationResultT status;
	/*
	uint8_t* page_buffer = pvPortMalloc(FLASH_PAGE_SIZE_BYTES);
	if( NULL == page_buffer )
	{
		FM_hermesFlashNotifyResponse(kStatus_Fail, msg->xClientTaskHandle);
		return false;
	}
	
	status = FM_hermesFlashEraseSector(msg->offsetSectorAddr - FLASH_BASE);
	
	uint32_t buff_progress = 4;
	uint32_t data_progress = 0;
	uint32_t to_take;
	
	memset(page_buffer, 0, FLASH_PAGE_SIZE_BYTES);
	memcpy(page_buffer, &msg->dataLength, sizeof(msg->dataLength));
	
	while( (data_progress < msg->dataLength) && (status == kStatus_Success) )
	{
		to_take = msg->dataLength - data_progress;
		if( to_take > FLASH_PAGE_SIZE_BYTES - buff_progress )
		{
			to_take = FLASH_PAGE_SIZE_BYTES - buff_progress;
		}
		memcpy(&page_buffer[buff_progress], &msg->ptrToBuf[data_progress], to_take);	
		status = FM_hermesFlashPageWrite(msg->offsetSectorAddr - FLASH_BASE, page_buffer);
		
		msg->offsetSectorAddr += FLASH_PAGE_SIZE_BYTES;
		buff_progress = 0;
		data_progress += to_take;
		memset(page_buffer, 0, FLASH_PAGE_SIZE_BYTES);
	}
	
	vPortFree(page_buffer);
	*/
	return status;
}

/********************************************/
/**     Flash memory service functions     **/
/********************************************/


/**************************************************************
 * Function Name   : FM_hermesFlashCrcElementModify
 * Description     : Update a CRC element - read 2 pages - erase sector - modify CRC data - write 2 page
 * Inputs          : incoming_msg_buffer, target field address, data length
 * Outputs         : none
 * Returns         : status_t
 **************************************************************/
FlashOperationResultT FM_hermesFlashCrcElementModify(uint8_t* p_incomingData, uint32_t elementAddr, uint16_t dataLength)
{
	FlashOperationResultT status;
	/*
    static uint8_t* p_ramSinglePageData;
    static uint8_t* p_ramSecondPageData;
    uint32_t elementOffset = elementAddr % FLASH_PAGE_SIZE_BYTES;
    uint32_t pageBase = elementAddr & PAGE_BOUNDARY_MASK;                       // page boundary
    uint32_t sectorBase = elementAddr & SECTOR_BOUNDARY_MASK;                   // sector boundary
    
    // This function has been modified to specifically handle the CRC update (1 word) in the Persistent store,
    // where there are currently two pages of separate data structures...
    // page 1 - Stores the CRCs for all the NV sectors.
    // page 2 - Store the characterisation of Hermes e.g. led brightness, for POR continuity.
    //  Since a page modification requires a whole sector erase, the integrity of the second page must be maintained.
    
    // Copy pages into ram
    p_ramSinglePageData = (uint8_t*) pvPortMalloc( FLASH_PAGE_SIZE_BYTES * sizeof(uint8_t) );    // create buffer for single page update
    p_ramSecondPageData = (uint8_t*) pvPortMalloc( FLASH_PAGE_SIZE_BYTES * sizeof(uint8_t) );    // create buffer for second page store
    if (p_ramSinglePageData && p_ramSecondPageData)
    {
        uint8_t* p_nvReadAddr = (uint8_t*)pageBase;                    
        FM_hermesFlashCopyPage(p_ramSinglePageData, p_nvReadAddr);                 // get 1st page from flash
        p_nvReadAddr = (uint8_t*)FM_PERSISTENT_PAGE_ADDR;                    
        FM_hermesFlashCopyPage(p_ramSecondPageData, p_nvReadAddr);                 // get 2nd page from flash
        status = FM_hermesFlashEraseSector(sectorBase - FLASH_BASE);               // erase the target sector
        
        if (status == kStatus_Success)
        {
            for (uint16_t i=0; i<dataLength; i++)                               // Overwrite the local data element with new data
            {
                *(p_ramSinglePageData+i+elementOffset) = *(p_incomingData+i);
            }
            
            status = FM_hermesFlashPageWrite(pageBase-FLASH_BASE, p_ramSinglePageData);
            status = FM_hermesFlashPageWrite(FM_PERSISTENT_PAGE_ADDR-FLASH_BASE, p_ramSecondPageData);

        }
        vPortFree((void*)p_ramSinglePageData);
        vPortFree((void*)p_ramSecondPageData);
    }
    */
    return status;
}  
/**************************************************************
 * Function Name   : FM_hermesFlashPageUpdate
 * Description     : Update a page within a sector
 * Inputs          : incoming_msg_buffer, absolute page address
 * Outputs         : CRITICAL_IMPORTANCE zprintf indicates failure
 * Returns         : status_t
 **************************************************************/
FlashOperationResultT FM_hermesFlashPageUpdate(SEND_TO_FM_MSG* p_incomingMsg, uint32_t pageAddr)
{
	FlashOperationResultT status;
	/*
    uint16_t i;
    static uint8_t* p_ramPageData;
    static uint8_t* p_ramSectorData;
    uint32_t sectorBase = pageAddr & SECTOR_BOUNDARY_MASK;                      // sector boundary
    uint8_t pageNum = (pageAddr & 0xf00)>>8;
    bool success = true;

    // get the new incoming data page into our temp buffer
    p_ramPageData = (uint8_t*) pvPortMalloc( FLASH_PAGE_SIZE_BYTES * sizeof(uint8_t) );
    if (p_ramPageData)
    {
        for (i=0; i<FLASH_PAGE_SIZE_BYTES; i++)
        {
            *(p_ramPageData+i) = 0xff;
        }
        for (i=0; i<p_incomingMsg->dataLength; i++)
        {
            *(p_ramPageData+i) = *(p_incomingMsg->ptrToBuf+i);
        }

    
        // Now copy existing data + new page into buffer
        p_ramSectorData = (uint8_t*) pvPortMalloc( FLASH_PAGE_SIZE_BYTES * FLASH_PAGES_PER_SECTOR );  
        if (p_ramSectorData)
        {

            for (i=0; i<FLASH_PAGES_PER_SECTOR; i++)
            {
                if (i == pageNum)
                    FM_hermesFlashCopyPage(p_ramSectorData+(i*FLASH_PAGE_SIZE_BYTES), p_ramPageData);
                else
                    FM_hermesFlashCopyPage(p_ramSectorData+(i*FLASH_PAGE_SIZE_BYTES), (uint8_t*) sectorBase+(i*FLASH_PAGE_SIZE_BYTES));
            }
            
            status = FM_hermesFlashEraseSector(sectorBase - FLASH_BASE);        // erase the target sector
            if (status == kStatus_Success)
            {
                for (uint8_t i=0; i<FLASH_PAGES_PER_SECTOR; i++)
                {
                    // Write new pages to NV Store
                    status = FM_hermesFlashPageWrite(((sectorBase-FLASH_BASE)+(i*FLASH_PAGE_SIZE_BYTES)), p_ramSectorData+(i*FLASH_PAGE_SIZE_BYTES));
                    if (status != kStatus_Success)
                    {
                        success = false;
                        break;
                    }
                }
                if (success == false)
                {
                    zprintf(CRITICAL_IMPORTANCE,"FAILED write flash page %d, sector %0x\r\n", i, sectorBase);
                    return kStatus_Fail;
                }
    		}
            else
            {
                zprintf(CRITICAL_IMPORTANCE,"FAILED to erase sector %0x %d\r\n", sectorBase);
                return kStatus_Fail;
            }
        }
        else
        {
            zprintf(CRITICAL_IMPORTANCE,"FAILED TO pvPortMalloc 4K buffer for updating\r\n");
            vPortFree((void*)p_ramPageData);                                    // this one did succeed, so free up the memory
            return kStatus_Fail;
        }
    }
	else
    {
		zprintf(CRITICAL_IMPORTANCE,"FAILED TO pvPortMalloc page buffer for updating\r\n");
        return kStatus_Fail;
    }
    
    vPortFree((void*)p_ramSectorData);
    vPortFree((void*)p_ramPageData);
    */
    return status;
}  
/**************************************************************
 * Function Name   : FM_hermesFlashUpdateDevStats
 * Description     : Update the Device Stats in NV Store.
 * Inputs          : incoming_msg_buffer, field address
 * Outputs         : none
 * Returns         : void
 **************************************************************/
void FM_hermesFlashUpdateDevStats(SEND_TO_FM_MSG* p_incomingMsg, uint32_t elementAddr)
{
	/*
    static uint8_t* p_ramMultiPageData;

    status_t status = kStatus_Fail;
    uint8_t numOfPages;
    
    // Copy a pages into ram
    p_ramMultiPageData = (uint8_t*) pvPortMalloc( FLASH_PAGE_SIZE_BYTES * 16 ); // buffer to handle multi page updates 
    if (p_ramMultiPageData)
    {
        // determine how many pages this input data will require
        if (p_incomingMsg->dataLength % FLASH_PAGE_SIZE_BYTES)
            numOfPages = (p_incomingMsg->dataLength/FLASH_PAGE_SIZE_BYTES)+1;
        else
            numOfPages = p_incomingMsg->dataLength/FLASH_PAGE_SIZE_BYTES; 

        for (uint8_t i=0; i<numOfPages; i++)
        {
            // Copy pages into ram
            FM_hermesFlashCopyPage(p_ramMultiPageData+(i*FLASH_PAGE_SIZE_BYTES), p_incomingMsg->ptrToBuf+(i*FLASH_PAGE_SIZE_BYTES));
        }

		// erase the target sector
        status = FM_hermesFlashEraseSector(elementAddr - FLASH_BASE);
        if (status == kStatus_Success)
        {
            for (uint8_t i=0; i<numOfPages; i++)
            {
                // Write new pages to NV Store
                status = FM_hermesFlashPageWrite(((elementAddr-FLASH_BASE)+(i*FLASH_PAGE_SIZE_BYTES)), p_ramMultiPageData+(i*FLASH_PAGE_SIZE_BYTES));
            }
        }
        vPortFree((void*)p_ramMultiPageData);
    }
	else
		zprintf(HIGH_IMPORTANCE,"FAILED TO pvPortMalloc 4K BUFFER FOR WRITING DEV STATS\r\n");
    FM_hermesFlashNotifyResponse(status, p_incomingMsg->xClientTaskHandle);
    */
}  
/**************************************************************
 * Function Name   : FM_hermesFlashElementRead
 * Description     : Read data element in NV Store.
 * Inputs          : incoming_msg_buffer, field address, data length, TaskHandle_t
 * Outputs         : The read data is o/p via msg ptr, Notify response
 * Returns         : void
 **************************************************************/
void FM_hermesFlashElementRead(uint8_t* p_incomingMsg, uint32_t elementAddr, uint16_t dataLength, TaskHandle_t xClientTaskHandle)
{
	/*
    uint8_t* flash_read_ptr = (uint8_t*)( elementAddr );
    
    for (uint16_t i=0; i<dataLength; i++)
    {
        *(p_incomingMsg+i) = *(flash_read_ptr+i);
    }
    
    FM_hermesFlashNotifyResponse(kStatus_Success, xClientTaskHandle);
    */
}
/**************************************************************
 * Function Name   : FM_hermesFlashReadDevStats
 * Description     : Read the Device Stats in NV Store.
 * Inputs          : incoming_msg_buffer, field address
 * Outputs         : The read data is o/p via msg ptr
 * Returns         : void
 **************************************************************/
void FM_hermesFlashReadDevStats(SEND_TO_FM_MSG* p_incomingMsg, uint32_t elementAddr)
{
	/*
    uint8_t* flash_read_ptr = (uint8_t*)( elementAddr );
    for (uint16_t i=0; i<p_incomingMsg->dataLength; i++)
    {
        *(p_incomingMsg->ptrToBuf+i) = *(flash_read_ptr+i);
    }
    
    FM_hermesFlashNotifyResponse(kStatus_Success, p_incomingMsg->xClientTaskHandle);
    */
}  
/**************************************************************
 * Function Name   : FM_hermesFlashCopyPage
 * Description     : Copy a page from flash to ram buffer.
 * Inputs          : Ptr to ram buffer, Page address start
 * Outputs         : The 256k data buffer is filled
 * Returns         : void
 **************************************************************/
void FM_hermesFlashCopyPage(uint8_t* p_ramSinglePageData, uint8_t* p_nvReadAddr)
{
    uint16_t i;
    for (i = 0; i < 256; i++)
    {
        *(p_ramSinglePageData+i) = *(p_nvReadAddr+i);
    }
}
// This will probably be deprecated in favour of TaskNotify below
/**************************************************************
 * Function Name   : FM_hermesFlashResponseMsg
 * Description     : Construct a reply message.
 * Inputs          : ACK/NAK, length of data handled (as a check), echo the requested action
 * Outputs         : Format message struct
 * Returns         : void
 **************************************************************/
void FM_hermesFlashResponseMsg(FlashOperationResultT status, uint32_t dataLength, FM_STORE_ACTION action)
{
    static RESP_FROM_FM_MSG nvmResponseMsg;
    /*
    if(status == kStatus_Success)
    {
        nvmResponseMsg.action = FM_ACK;
        nvmResponseMsg.action |= action;
        nvmResponseMsg.dataLength = dataLength;
    } else
    {
        nvmResponseMsg.action = FM_NAK;
        nvmResponseMsg.action |= action;
    }
    xQueueSend(xNvStoreMailboxResp, &nvmResponseMsg, 0);
    */
}
/**************************************************************
 * Function Name   : FM_hermesFlashNotifyResponse
 * Description     : Reply with a Notification.
 * Inputs          : ACK/NAK, the recipient's Task Handle 
 * Outputs         : TaskNotification
 * Returns         : void
 **************************************************************/
void FM_hermesFlashNotifyResponse(FlashOperationResultT status, TaskHandle_t p_ClientTaskHandle)
{
	/*
    if( status == kStatus_Success )
    {
        xTaskNotify(p_ClientTaskHandle, FM_ACK, eSetValueWithOverwrite );
    } else
    {
        xTaskNotify(p_ClientTaskHandle, FM_NAK, eSetValueWithOverwrite );
    }
    */
}  
/**************************************************************
 * Function Name   : FM_crcIntegrityCheck
 * Description     : Check NV store pages against saved crc's.
 * Inputs          : void
 * Outputs         : none
 * Returns         : Success/Fail
 **************************************************************/
#define GET_NV_CRC(x)   ((uint16_t*)( FLASH_BASE+(FM_SECTOR_PERSISTENT*FLASH_SECTOR_SIZE_BYTES)+(x)))
FlashOperationResultT FM_crcIntegrityCheck(void)
{
	FlashOperationResultT crcStatus;
	/*
    uint16_t crcAreaVal;
    uint16_t* p_crcVal;
    uint32_t i;

    for (i=FM_CRC_DEV_STATS; i<(FM_CRC_PERSISTENT); i++)                        // From FM_SECTOR_STATS(1016) to (1022)
    {
        crcAreaVal = FM_crcCalcAreaCode((CRC_DATA_AREA)i);                         // calculate CRC each sector                           
        p_crcVal = GET_NV_CRC((i-FM_CRC_DEV_STATS)*2);                          // Get the stored CRC, passing the offset store posn
        if (*p_crcVal == 0xffff)
        {
//            zprintf(HIGH_IMPORTANCE,"\r\nSector %d = 0xffff\r\n",i);            // not yet been stored, so ignore
        }
        else
        {
            if ( crcAreaVal != *p_crcVal )                                      // compare calculated with stored value
            {
                crcStatus = kStatus_Fail;
                zprintf(CRITICAL_IMPORTANCE,"Sector %d CRC failed... %x\r\n",i, *p_crcVal);
            }
            else
            {
//                zprintf(HIGH_IMPORTANCE,"Sector %d CRC is ok... %x\r\n",i, *p_crcVal);
            }
        }
    }
    */
    return crcStatus;
}
/**************************************************************
 * Function Name   : FM_crcUpdateAreaCode
 * Description     : Store the CRC for given area.
 * Inputs          : Area type
 * Outputs         : Update CRC code
 * Returns         : Flash write status
 **************************************************************/
FlashOperationResultT FM_crcUpdateAreaCode(CRC_DATA_AREA dataArea)
{
	FlashOperationResultT status;
	/*
    uint16_t crcVal;
    uint16_t* p_crcVal = &crcVal;
    
    crcVal = FM_crcCalcAreaCode(dataArea);// get crc for sector
    
    switch(dataArea)
    {
        case FM_CRC_DEV_STATS:
            status = FM_hermesFlashCrcElementModify((uint8_t*)p_crcVal, (FLASH_BASE+(FM_CRC_PERSISTENT*FLASH_SECTOR_SIZE_BYTES))+FM_CRC_DEV_STATS_POSN, sizeof(uint16_t));
        
        break;
          
        case FM_CRC_MAC_SERIAL:
            status = FM_hermesFlashCrcElementModify((uint8_t*)p_crcVal, (FLASH_BASE+(FM_CRC_PERSISTENT*FLASH_SECTOR_SIZE_BYTES))+FM_CRC_MAC_SERIAL_POSN, sizeof(uint16_t));
        
        break;
          
        case FM_CRC_FT_DATA:
            status = FM_hermesFlashCrcElementModify((uint8_t*)p_crcVal, (FLASH_BASE+(FM_CRC_PERSISTENT*FLASH_SECTOR_SIZE_BYTES))+FM_CRC_FT_DATA_POSN, sizeof(uint16_t));
        
        break;
          
        case FM_CRC_KEYS:
            status = FM_hermesFlashCrcElementModify((uint8_t*)p_crcVal, (FLASH_BASE+(FM_CRC_PERSISTENT*FLASH_SECTOR_SIZE_BYTES))+FM_CRC_KEYS_POSN, sizeof(uint16_t));
        
        break;
          
        case FM_CRC_PERSISTENT:
            status = FM_hermesFlashCrcElementModify((uint8_t*)p_crcVal, (FLASH_BASE+(FM_CRC_PERSISTENT*FLASH_SECTOR_SIZE_BYTES))+FM_CRC_PERSISTENT_POSN, sizeof(uint16_t));
        
        break;


        default:
        
        break;
    }
    DCACHE_InvalidateByRange(FLASH_BASE+(FM_SECTOR_PERSISTENT*FLASH_SECTOR_SIZE_BYTES), FLASH_PAGE_SIZE_BYTES);
*/
    return status;
}

