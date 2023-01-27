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
* Filename: NetworkInterface.c   
* Author:   Chris Cowdery 16/10/2018
* Purpose:  Network Interface between FreeRTOS-TCP and iMXRT1021 Driver.
*
* We provide initialisation, Transmit and Receive functions. We also
* provide a few misc helper functions. 
*             
**************************************************************************/
#ifndef FREERTOS_TCP_ENABLE
#include "hermes.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/* FreeRTOS+TCP includes. */
#include "lwip.h"
#include "ethernetif.h"

/* Hardware includes */

#include "Hermes-app.h"
#include "debug.h"
#include "hermes-time.h"

BaseType_t 					InitialiseNetwork(void); // prototype

static void iprxisr_task(void *pvParameters);
static TaskHandle_t xIpRxIsr_task_handle = NULL;
static void network_watchdog_task(void *pvParameters);
static TaskHandle_t xNetworkWatchdog_task_handle = NULL;
volatile uint32_t last_ethernet_packet_timestamp = 0;
QueueHandle_t xRestartNetworkMailbox;

extern EventGroupHandle_t xConnectionStatus_EventGroup;

// This function initialises the MAC and PHY using helper functions in fsl_enet.c and fsl.phy.c
// Note it can be called in two scenarios:
// 1. When the system starts,
// 2. When the network restarts, e.g. a network cable is plugged back in.
// In the latter case, it doesn't have to do so much initialisation.
BaseType_t xNetworkInterfaceInitialise( void )
{
	extern PRODUCT_CONFIGURATION product_configuration;
	static bool xFirstTime = true;
    BaseType_t xReturn = pdPASS;
	UBaseType_t xCurrentPriority;
	BaseType_t xRetVal;

	if(xFirstTime)
	{
		xRestartNetworkMailbox	= xQueueCreate(1, sizeof(bool));	// only make this once		
		
	}
	
	xFirstTime = false;	// future invocations of this call tree will do less initialisation
	
    return xReturn;
}

void NetworkInterfaceLinkUp(void* arg)
{
	zprintf(LOW_IMPORTANCE,"Network up.\r\n");
	xEventGroupSetBits(xConnectionStatus_EventGroup, CONN_STATUS_NETWORK_UP);
}

void NetworkInterfaceLinkDown(void* arg)
{
	zprintf(LOW_IMPORTANCE,"Network down.\r\n");
	xEventGroupClearBits(xConnectionStatus_EventGroup, CONN_STATUS_NETWORK_UP);
}

void NetworkInterface_IsActive(void* arg)
{
	return ((CONN_STATUS_NETWORK_UP & xEventGroupGetBits(xConnectionStatus_EventGroup)) > 0);
}
/*
void HAL_ETH_WakeUpCallback(ETH_HandleTypeDef *heth)
{
    switch (heth->Init)
    {
        case eNetworkDown:
            zprintf(LOW_IMPORTANCE,"Network down.\r\n");
			xEventGroupClearBits(xConnectionStatus_EventGroup, CONN_STATUS_NETWORK_UP);
            break;
        case eNetworkUp:
//            zprintf(LOW_IMPORTANCE,"Network up.\r\n");
			xEventGroupSetBits(xConnectionStatus_EventGroup, CONN_STATUS_NETWORK_UP);
            break;
        default:
            zprintf(MEDIUM_IMPORTANCE,"Unknown IP Network Event Hook\r\n");
            break;
    }
}
*/
uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
													uint16_t usSourcePort,
													uint32_t ulDestinationAddress,
													uint16_t usDestinationPort )
{
	uint32_t result;
    result = hermes_rand();
	return result;
}
#endif
