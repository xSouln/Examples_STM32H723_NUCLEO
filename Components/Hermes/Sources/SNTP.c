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
#include "dns.h"

#include "lwip.h"
#include "sockets.h"
#include "netdb.h"

//static bool	SNTP_GetTime(void);
EventGroupHandle_t	xSNTP_EventGroup;

void SNTP_Init(void)
{
	xSNTP_EventGroup = xEventGroupCreate();
}

void SNTP_Task(void *pvParameters)
{
    EventBits_t xEventBits;

	while( true )
	{
		xEventBits = xEventGroupWaitBits(xSNTP_EventGroup, SNTP_EVENT_UPDATE_REQUESTED, pdFALSE, pdFALSE, SNTP_AUTO_INTERVAL);
		if( 0 != (SNTP_EVENT_UPDATE_REQUESTED & xEventBits) )
		{
			SNTP_GetTime();
		}
	};
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

static void (private_dns_found_callback)(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
	if (ipaddr)
	{
		input_ip_address.value[0] = ipaddr->addr >> 24;
		input_ip_address.value[1] = ipaddr->addr >> 16;
		input_ip_address.value[2] = ipaddr->addr >> 8;
		input_ip_address.value[3] = ipaddr->addr >> 0;
	}
}

bool SNTP_GetTime(void)
{
	int				sntpSocket;
	uint32_t		rx_timeout = 1000; // TAM Check for the best value of this.
	bool			success = false;

	xEventGroupClearBits(xSNTP_EventGroup, SNTP_EVENT_UPDATE_FAILED | SNTP_EVENT_TIME_VALID);
	xEventGroupSetBits(xSNTP_EventGroup, SNTP_EVENT_UPDATE_UNDERWAY);

	sntp_printf("\r\n\t@@@@@@@ Starting SNTP @@@@@@@\r\n");

	do
	{
		struct sockaddr_in address;
		address.sin_port = 0;

		sntp_printf(SNTP_LINE "Setting up Socket | ");

		//host_result = gethostbyname(SNTP_SERVER);
		err_t res;
		ip_addr_t addr;
		addr.addr = IPADDR_ANY;//LWIP_MAKEU32(192, 168, 0, 1);

		//host_result = gethostbyname(SNTP_SERVER);
		res = dns_gethostbyname(SNTP_SERVER, &addr, private_dns_found_callback, 0);

		if(res == ERR_OK)
		{
			sntp_printf("Success!\r\n");
		}
		else
		{
			sntp_printf("Failure: %d!\r\n", result);
			break;
		}

		address.sin_addr.s_addr = addr.addr;//addr.addr;//inet_addr(input_ip_address.value);ipaddr_ntoa193.158.22.13

		address.sin_port = htons(SNTP_PORT);
		address.sin_family = AF_INET;

		sntpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(sntpSocket < 0)
		{
			break;
		}

		int32_t result = bind(sntpSocket, (struct sockaddr*)&address, sizeof(address));
		if (result != 0)
		{
			break;
		}

		setsockopt(sntpSocket, 0, SO_RCVTIMEO, &rx_timeout, sizeof(rx_timeout));
		setsockopt(sntpSocket, 0, SO_SNDTIMEO, &rx_timeout, sizeof(rx_timeout));

		sntp_printf(SNTP_LINE "Resolving URL: %s | ", SNTP_SERVER);
		sntp_printf("Found at %08x.\r\n", address.sin_addr);
		sntp_printf(SNTP_LINE "Writing Request | ");

		NTP_PACKET	request;
		memset(&request, 0, sizeof(request));
		request.flags.versionNumber = 3;
		request.flags.mode = 3;
		request.orig_ts_secs = swap32(SNTP_EPOCH);

		//connect(sntpSocket, &request, sizeof(request));

		result = sendto(sntpSocket, &request, sizeof(request), 0, (struct sockaddr*)&address, sizeof(address));
		if(result != sizeof(request))
		{
			sntp_printf("Failed!\r\n");
			break;
		}

		sntp_printf("Success!\r\n");
		sntp_printf(SNTP_LINE "Awaiting Response | ");

		uint32_t packet_len;
		result = recvfrom(sntpSocket, &request, sizeof(request), 0, (struct sockaddr*)&address, &packet_len);
		if(result == sizeof(request))
		{
			if(request.stratum == 0)
			{	// Invalid response, maybe a KoD.
				sntp_printf("SNTP received an invalid response: %.4s\r\n", request.ref_identifier);
				success = false;
				break;
			}

			request.tx_ts_secs = swap32(request.tx_ts_secs) - SNTP_EPOCH; // Flip endianess and subtract epoch.
			if( request.tx_ts_fraq & 0x80 ){ request.tx_ts_secs++; } // Fraction is 32-bit big-endian, so 0x80 represents 0.5.
			sntp_printf("Received: %d seconds since epoch.\r\n", request.tx_ts_secs);
			set_utc(request.tx_ts_secs);

			volatile struct tm time;
			time = *hermes_gmtime(&request.tx_ts_secs);
			sntp_printf(SNTP_LINE "Year: %d | Month: %d | Day: %d | Hour: %d | Minute: %d | Second: %d\r\n", time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
			success = true;
		}
		else
		{
			sntp_printf("Failed. Received %d/%d bytes.\r\n", result, sizeof(request));
		}
	} while( false ); // Just for non-returning breaks.

	xEventGroupClearBits(xSNTP_EventGroup, SNTP_EVENT_UPDATE_REQUESTED | SNTP_EVENT_UPDATE_UNDERWAY); // Signal that we're done. Note: doesn't mean the time is good.
	if(true == success)
	{
		xEventGroupSetBits(xSNTP_EventGroup, SNTP_EVENT_TIME_VALID);	// Signal that the time is good.
	} else
	{
		xEventGroupSetBits(xSNTP_EventGroup, SNTP_EVENT_UPDATE_FAILED);	// ...Or that it's not.
	}
	shutdown(sntpSocket, SHUT_RDWR);
	closesocket(sntpSocket);

	sntp_printf("\t@@@@@@@ SNTP Complete @@@@@@@\r\n");
	sntp_flush();
	return success;
}
