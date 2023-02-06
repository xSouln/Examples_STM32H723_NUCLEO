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

//static bool	SNTP_GetTime(void);
EventGroupHandle_t	xSNTP_EventGroup;

void SNTP_Task(void *pvParameters)
{
    EventBits_t xEventBits;

	while( true )
	{
		xEventBits = xEventGroupWaitBits(xSNTP_EventGroup, SNTP_EVENT_UPDATE_REQUESTED, pdFALSE, pdFALSE, SNTP_AUTO_INTERVAL);

		if(SNTP_EVENT_UPDATE_REQUESTED & xEventBits)
		{
			SNTP_GetTime();
		}
	};
}

void SNTP_Init(void)
{
	xSNTP_EventGroup = xEventGroupCreate();
}

bool SNTP_IsTimeValid(void)
{
	return (0 != (SNTP_EVENT_TIME_VALID & xEventGroupGetBits(xSNTP_EventGroup)));
}

bool SNTP_DidUpdateFail(void)
{
	return (0 != (SNTP_EVENT_UPDATE_FAILED & xEventGroupGetBits(xSNTP_EventGroup)));
}

bool SNTP_AwaitUpdate(bool MakeRequest, uint32_t TimeToWait)
{
	EventBits_t xEventBits = xEventGroupGetBits(xSNTP_EventGroup);

	if( (true == MakeRequest) && (0 == (SNTP_EVENT_UPDATE_UNDERWAY & xEventBits)) )
	{
		xEventGroupClearBits(xSNTP_EventGroup, SNTP_EVENT_UPDATE_FAILED | SNTP_EVENT_TIME_VALID);
		xEventGroupSetBits(xSNTP_EventGroup, SNTP_EVENT_UPDATE_REQUESTED);
	}

	xEventBits = xEventGroupWaitBits(xSNTP_EventGroup, SNTP_EVENT_UPDATE_FAILED | SNTP_EVENT_TIME_VALID, pdFALSE, pdFALSE, TimeToWait);
	if( (0 == TimeToWait) || (0 != (SNTP_EVENT_TIME_VALID & xEventBits)) )
	{
		return true;
	}
	return false;
}

static union
{
	uint8_t value[4];
	uint32_t word;

} input_ip_address;

static void private_dns_found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
	if (ipaddr)
	{
		input_ip_address.value[0] = ipaddr->addr >> 24;
		input_ip_address.value[1] = ipaddr->addr >> 16;
		input_ip_address.value[2] = ipaddr->addr >> 8;
		input_ip_address.value[3] = ipaddr->addr >> 0;
	}
}

static ip_addr_t addr = { 0 };

bool SNTP_GetTime(void)
{
	int sntpSocket;
	uint32_t rx_timeout = 1000; // TAM Check for the best value of this.
	bool success = false;
	NTP_PACKET* request = 0;
	struct timeval timeout = { 0 };

	xEventGroupClearBits(xSNTP_EventGroup, SNTP_EVENT_UPDATE_FAILED | SNTP_EVENT_TIME_VALID);
	xEventGroupSetBits(xSNTP_EventGroup, SNTP_EVENT_UPDATE_UNDERWAY);

	sntp_printf("\r\n\t@@@@@@@ Starting SNTP @@@@@@@\r\n");

	do
	{
		//host_result = gethostbyname(SNTP_SERVER);
		err_t res;
/*
		if (addr.addr == 0)
		{
			dns_gethostbyname(SNTP_SERVER, &addr, private_dns_found_callback, 0);www.google.com
		}
		*/
		//dns_gethostbyname("pool.ntp.org", &addr, private_dns_found_callback, 0);//SNTP_SERVER

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

		timeout.tv_sec = 1000;

		setsockopt(sntpSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
		setsockopt(sntpSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

		sntp_printf(SNTP_LINE "Resolving URL: %s | ", SNTP_SERVER);
		sntp_printf("Found at %08x.\r\n", address.sin_addr);
		sntp_printf(SNTP_LINE "Writing Request | ");

		//;mem_malloc(sizeof(NTP_PACKET))

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
			if(request->stratum == 0)
			{	// Invalid response, maybe a KoD.
				sntp_printf("SNTP received an invalid response: %.4s\r\n", request.ref_identifier);
				success = false;
				break;
			}

			request->tx_ts_secs = swap32(request->tx_ts_secs) - SNTP_EPOCH; // Flip endianess and subtract epoch.
			if( request->tx_ts_fraq & 0x80 ){ request->tx_ts_secs++; } // Fraction is 32-bit big-endian, so 0x80 represents 0.5.
			sntp_printf("Received: %d seconds since epoch.\r\n", request->tx_ts_secs);
			set_utc(request->tx_ts_secs);

			volatile struct tm time;
			time = *hermes_gmtime(&request->tx_ts_secs);
			sntp_printf(SNTP_LINE "Year: %d | Month: %d | Day: %d | Hour: %d | Minute: %d | Second: %d\r\n", time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
			success = true;
		}
		else
		{
			sntp_printf("Failed. Received %d/%d bytes.\r\n", result, sizeof(request));
		}
	}
	while( false ); // Just for non-returning breaks.

	shutdown(sntpSocket, SHUT_RDWR);
	closesocket(sntpSocket);

	// Signal that we're done. Note: doesn't mean the time is good.
	xEventGroupClearBits(xSNTP_EventGroup, SNTP_EVENT_UPDATE_REQUESTED | SNTP_EVENT_UPDATE_UNDERWAY);

	if(success)
	{
		// Signal that the time is good.
		xEventGroupSetBits(xSNTP_EventGroup, SNTP_EVENT_TIME_VALID);
	}
	else
	{
		// ...Or that it's not.
		xEventGroupSetBits(xSNTP_EventGroup, SNTP_EVENT_UPDATE_FAILED);
	}

	if(request)
	{
		mem_free(request);
	}

	sntp_printf("\t@@@@@@@ SNTP Complete @@@@@@@\r\n");
	sntp_flush();

	return success;
}
