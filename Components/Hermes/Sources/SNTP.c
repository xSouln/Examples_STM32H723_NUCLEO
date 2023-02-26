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
* Filename: SNTP.c
* Author:   Tom Monkhouse 02/04/2020
* Purpose:  Handles retrieving SNTP from an internet Time Server.
*
**************************************************************************/
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "list.h"
#include "event_groups.h"

#include "hermes-time.h"
#include "timer_interface.h"
#include "tls\network_platform.h"
#include "compiler.h"

#include "wolfssl/ssl.h"

#include "Hermes/Sources/SNTP.h"

#include "lwip.h"
#include "sockets.h"
#include "netdb.h"
#include "api.h"
#include "dns.h"
//==============================================================================
SNTP_StatusT SNTP_Status;

EventGroupHandle_t sntp_event_group;
//==============================================================================
void SNTP_Task(void *pvParameters)
{
	while(true)
	{
		EventBits_t xEventBits = xEventGroupWaitBits(sntp_event_group, SNTP_EVENT_UPDATE_REQUESTED, pdFALSE, pdFALSE, portMAX_DELAY);

		if (!(xEventBits & SNTP_EVENT_UPDATE_REQUESTED))
		{
			continue;
		}

		if (SNTP_Status.State == SNTP_STATE_REQUEST_UPDATE)
		{
			SNTP_Status.State = SNTP_STATE_IN_PROGRESS;

			SNTP_Status.Result = SNTP_GetTime() ? SNTP_RESULT_SUCCESS : SNTP_RESULT_ERROR;

			SNTP_Status.State = SNTP_STATE_STOPPED;
		}

		xEventGroupSetBits(sntp_event_group, SNTP_EVENT_COMPLITE);
		xEventGroupClearBits(sntp_event_group, SNTP_EVENT_UPDATE_REQUESTED);
	};
}
//------------------------------------------------------------------------------
void SNTP_Init(void)
{
	sntp_event_group = xEventGroupCreate();
}
//------------------------------------------------------------------------------
bool SNTP_IsTimeValid(void)
{
	return SNTP_Status.Result == SNTP_RESULT_SUCCESS;
}
//------------------------------------------------------------------------------
bool SNTP_DidUpdateFail(void)
{
	return SNTP_Status.Result == SNTP_RESULT_ERROR;
}
//------------------------------------------------------------------------------
void SNTP_RequestUpdate()
{
	if (SNTP_Status.State == SNTP_STATE_STOPPED)
	{
		SNTP_Status.State = SNTP_STATE_REQUEST_UPDATE;

		xEventGroupClearBits(sntp_event_group, SNTP_EVENT_COMPLITE);
		xEventGroupSetBits(sntp_event_group, SNTP_EVENT_UPDATE_REQUESTED);
	}
}
//------------------------------------------------------------------------------
bool SNTP_AwaitUpdate(bool MakeRequest, uint32_t TimeToWait)
{
	SNTP_RequestUpdate();

	xEventGroupWaitBits(sntp_event_group, SNTP_EVENT_COMPLITE, pdFALSE, pdFALSE, TimeToWait);

	if (SNTP_Status.Result == SNTP_RESULT_SUCCESS)
	{
		return true;
	}

	return false;
}
//------------------------------------------------------------------------------
bool SNTP_GetTime(void)
{
	int sntpSocket;
	bool success = false;
	NTP_PACKET* request = 0;
	struct timeval timeout = { 0 };
	ip_addr_t addr = { 0 };

	sntp_printf("\r\n\t@@@@@@@ Starting SNTP @@@@@@@\r\n");

	do
	{
		/* query host IP address */
		err_t err = netconn_gethostbyname(SNTP_SERVER, &addr);

		if(err == ERR_OK && addr.addr != 0)
		{
			sntp_printf("Success!\r\n");
		}
		else
		{
			sntp_printf("Failure: %d!\r\n", result);
			break;
		}

		struct sockaddr_in host_address;
		host_address.sin_family = AF_INET;
		host_address.sin_port = 0;
		host_address.sin_addr.s_addr = IPADDR_ANY;

		struct sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = addr.addr;
		address.sin_port = htons(SNTP_PORT);

		sntp_printf(SNTP_LINE "Setting up Socket | ");

		sntpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(sntpSocket < 0)
		{
			break;
		}

		int32_t result;

		result = bind(sntpSocket, (struct sockaddr*)&host_address, sizeof(host_address));
		if (result != 0)
		{
			break;
		}

		timeout.tv_sec = 5000;

		setsockopt(sntpSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
		setsockopt(sntpSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

		sntp_printf(SNTP_LINE "Resolving URL: %s | ", SNTP_SERVER);
		sntp_printf("Found at %08x.\r\n", address.sin_addr);
		sntp_printf(SNTP_LINE "Writing Request | ");

		NTP_PACKET* request = mem_malloc(sizeof(NTP_PACKET));
		memset(request, 0, sizeof(NTP_PACKET));
		request->flags.versionNumber = 3;
		request->flags.mode = 3;
		request->orig_ts_secs = swap32(SNTP_EPOCH);

		result = sendto(sntpSocket, request, sizeof(NTP_PACKET), 0, (struct sockaddr*)&address, sizeof(address));
		if(result != sizeof(NTP_PACKET))
		{
			sntp_printf("Failed!\r\n");
			break;
		}

		sntp_printf("Success!\r\n");
		sntp_printf(SNTP_LINE "Awaiting Response | ");

		uint32_t packet_len = sizeof(address);

		result = recvfrom(sntpSocket, request, sizeof(NTP_PACKET), 0, (struct sockaddr*)&address, &packet_len);
		if(result == sizeof(NTP_PACKET))
		{
			if(!request->stratum)
			{
				// Invalid response, maybe a KoD.
				sntp_printf("SNTP received an invalid response: %.4s\r\n", request.ref_identifier);
				success = false;
				break;
			}

			// Flip endianess and subtract epoch.
			request->tx_ts_secs = swap32(request->tx_ts_secs) - SNTP_EPOCH;

			// Fraction is 32-bit big-endian, so 0x80 represents 0.5.
			if(request->tx_ts_fraq & 0x80)
			{
				request->tx_ts_secs++;
			}

			sntp_printf("Received: %d seconds since epoch.\r\n", request->tx_ts_secs);
			set_utc(request->tx_ts_secs);

			struct tm time;
			time = *hermes_gmtime(&request->tx_ts_secs);

			sntp_printf(SNTP_LINE "Year: %d | Month: %d | Day: %d | Hour: %d | Minute: %d | Second: %d\r\n",
					time.tm_year,
					time.tm_mon,
					time.tm_mday,
					time.tm_hour,
					time.tm_min,
					time.tm_sec);

			success = true;

			time = time;
		}
		else
		{
			sntp_printf("Failed. Received %d/%d bytes.\r\n", result, sizeof(request));
		}

	} while(false);

	shutdown(sntpSocket, SHUT_RDWR);
	closesocket(sntpSocket);

	if(request)
	{
		mem_free(request);
	}

	sntp_printf("\t@@@@@@@ SNTP Complete @@@@@@@\r\n");
	sntp_flush();

	return success;
}
//==============================================================================
