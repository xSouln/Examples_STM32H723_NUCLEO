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
#include "hermes.h"

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
//==============================================================================
//externs:

extern struct netif gnetif;
extern EventGroupHandle_t xConnectionStatus_EventGroup;
//==============================================================================
//variables:

//static TaskHandle_t xNetworkWatchdog_task_handle = NULL;
static TaskHandle_t xNetworkHandler_task_handle = NULL;

static bool network_is_connected;

volatile uint32_t last_ethernet_packet_timestamp = 0;
QueueHandle_t xRestartNetworkMailbox;
EventGroupHandle_t xConnectionStatus_EventGroup;
//==============================================================================
//prototypes:

//static void network_watchdog_task(void *pvParameters);
BaseType_t InitialiseNetwork(void);
static void xNetworkHandlerTask(void *pvParameters);
//==============================================================================
//functions:

// This function initialises the MAC and PHY using helper functions in fsl_enet.c and fsl.phy.c
// Note it can be called in two scenarios:
// 1. When the system starts,
// 2. When the network restarts, e.g. a network cable is plugged back in.
// In the latter case, it doesn't have to do so much initialisation.
int xNetworkInterfaceInitialise()
{
	xRestartNetworkMailbox = xQueueCreate(1, sizeof(bool));
	xConnectionStatus_EventGroup = xEventGroupCreate();

	if(xTaskCreate(xNetworkHandlerTask, "NetworkHandle", 0x200, NULL, osPriorityNormal, &xNetworkHandler_task_handle) != pdPASS)
	{
		zprintf(CRITICAL_IMPORTANCE, "IP RX ISR task creation failed!.\r\n");
		
		return -1;
	}
	
    return 0;
}
//------------------------------------------------------------------------------
static void xNetworkHandlerTask(void *pvParameters)
{
	while (true)
	{
		bool is_connecnted = netif_is_up(&gnetif);

		if (is_connecnted && !network_is_connected)
		{
			network_is_connected = true;
			xEventGroupSetBits(xConnectionStatus_EventGroup, CONN_STATUS_NETWORK_UP);
		}
		else if(!is_connecnted && network_is_connected)
		{
			network_is_connected = false;
			xEventGroupClearBits(xConnectionStatus_EventGroup, CONN_STATUS_NETWORK_UP);
		}

		osDelay(pdMS_TO_TICKS(50));
	}
}
//------------------------------------------------------------------------------
static uint32_t ConvertAddresArrayToUInt32(uint8_t* value)
{
	uint32_t result = 0;

	result |= (uint32_t)value[0];
	result |= (uint32_t)value[1] << 8;
	result |= (uint32_t)value[2] << 16;
	result |= (uint32_t)value[3] << 24;

	return result;
}
//------------------------------------------------------------------------------
static uint32_t ConvertAddresValuesToUInt32(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4)
{
	uint32_t result = 0;

	result |= (uint32_t)a1;
	result |= (uint32_t)a2;
	result |= (uint32_t)a3;
	result |= (uint32_t)a4;

	return result;
}
//------------------------------------------------------------------------------
void NetworkInterface_GetAddressConfiguration(uint32_t* ip,
		uint32_t* mask,
		uint32_t* gateway,
		uint32_t* dns)
{
	if (ip)
	{
		*ip = ConvertAddresValuesToUInt32(configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3);
	}

	if (mask)
	{
		*mask = ConvertAddresValuesToUInt32(configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3);
	}

	if (gateway)
	{
		*gateway = ConvertAddresValuesToUInt32(configGATEWAY_ADDR0, configGATEWAY_ADDR1, configGATEWAY_ADDR2, configGATEWAY_ADDR3);
	}

	if (dns)
	{
		*dns = ConvertAddresValuesToUInt32(configDNS_SERVER_ADDR0, configDNS_SERVER_ADDR1, configDNS_SERVER_ADDR2, configDNS_SERVER_ADDR3);
	}
}
//------------------------------------------------------------------------------
void NetworkInterfaceLinkUp(void* arg)
{
	zprintf(LOW_IMPORTANCE,"Network up.\r\n");

	if (xConnectionStatus_EventGroup)
	{
		network_is_connected = true;
		xEventGroupSetBits(xConnectionStatus_EventGroup, CONN_STATUS_NETWORK_UP);
	}
}
//------------------------------------------------------------------------------
void NetworkInterfaceLinkDown(void* arg)
{
	zprintf(LOW_IMPORTANCE,"Network down.\r\n");

	if (xConnectionStatus_EventGroup)
	{
		network_is_connected = false;
		xEventGroupClearBits(xConnectionStatus_EventGroup, CONN_STATUS_NETWORK_UP);
	}
}
//------------------------------------------------------------------------------
bool NetworkInterface_IsActive()
{
	if (xConnectionStatus_EventGroup)
	{
		return ((xEventGroupGetBits(xConnectionStatus_EventGroup) & CONN_STATUS_NETWORK_UP) > 0);
	}

	return 0;
}
//------------------------------------------------------------------------------
uint32_t ulApplicationGetNextSequenceNumber(uint32_t ulSourceAddress,
											uint16_t usSourcePort,
											uint32_t ulDestinationAddress,
											uint16_t usDestinationPort)
{
	uint32_t result;
    result = hermes_rand();
	return result;
}
//==============================================================================
