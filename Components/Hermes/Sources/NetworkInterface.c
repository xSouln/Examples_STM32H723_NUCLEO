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

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
/*
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"
*/

/* Hardware includes */


#include "Hermes-app.h"
#include "debug.h"
#include "hermes-time.h"

static void iprxisr_task(void *pvParameters);
static TaskHandle_t xIpRxIsr_task_handle = NULL;
static void network_watchdog_task(void *pvParameters);
static TaskHandle_t xNetworkWatchdog_task_handle = NULL;
volatile uint32_t last_ethernet_packet_timestamp = 0;
QueueHandle_t xRestartNetworkMailbox;

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

	if( true == xFirstTime)
	{
		xRestartNetworkMailbox	= xQueueCreate(1, sizeof(bool));	// only make this once		
		/*
		// now start the deferred interrupt handler task
		if( xTaskCreate(iprxisr_task, "IpRxISR", ipISR_TASK_STACK_SIZE_WORDS, NULL, ISR_TASK_PRIORITY, &xIpRxIsr_task_handle) != pdPASS )
		{
			zprintf(CRITICAL_IMPORTANCE, "IP RX ISR task creation failed!.\r\n");
			xReturn = pdFAIL;
		}
		// And the Network Watchdog task
		if( PRODUCT_CONFIGURED == product_configuration.product_state)
		{
			if( xTaskCreate(network_watchdog_task, "Network Watchdog", NETWORK_WATCHDOG_TASK_STACK_SIZE, NULL, NORMAL_TASK_PRIORITY, &xNetworkWatchdog_task_handle) != pdPASS )
			{
				zprintf(CRITICAL_IMPORTANCE, "Network Watchdog task creation failed!.\r\n");
				xReturn = pdFAIL;
			}		
		}
		*/
	}
	
    return xReturn;
}
