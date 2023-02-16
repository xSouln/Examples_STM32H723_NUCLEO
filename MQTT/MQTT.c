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
* Filename: MQTT.c
* Author:   Tom Monkhouse
* Purpose:
**************************************************************************/
#include "hermes.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"

#include "MQTT.h"
#include "AWS.h"
#include "Backoff.h"
#include "hermes-time.h"
#include "MQTT_internal.h"
#include "wolfssl/wolfcrypt/pkcs12.h"
#include "wolfssl/wolfcrypt/coding.h"

#include "HTTP_Helper.h"
#include "SNTP.h"
#include "Server_Buffer.h"
#include "BuildNumber.h"
#include "Hermes-app.h"
#include "Hermes/Sources/SNTP.h"

#include "wolfssl/wolfcrypt/sha256.h"

#include "lwip.h"
#include "rng.h"
//==============================================================================
#ifndef MQTT_SERVER_SIMULATED

extern QueueHandle_t		xOutgoingMQTTMessageMailbox;
extern QueueHandle_t		xBufferMessageMailbox;
extern QueueHandle_t 		xRestartNetworkMailbox;
extern EventGroupHandle_t	xConnectionStatus_EventGroup;

STORED_CREDENTIAL MQTT_Stored_Certificate MQTT_STORED_CERTIFICATE_MEM_SECTION;
STORED_CREDENTIAL MQTT_Stored_Private_Key MQTT_STORED_PRIVATE_KEY_MEM_SECTION;

static AWS_IoT_Client 			aws_client MQTT_AWS_CLIENT_MEM_SECTION;
SUREFLAP_CREDENTIALS			aws_credentials MQTT_SUREFLAP_CREDENTIALS_MEM_SECTION;
static MQTT_CONNECTION_STATE	mqtt_connection_state = MQTT_STATE_INITIAL;
static char aws_certificat[CERTIFICATE_MAX_SIZE] MQTT_CERTIFICATE_MEM_SECTION;
static bool no_creds = false;
//==============================================================================
static void MQTT_Hash_It_Up(SUREFLAP_CREDENTIALS* creds);
//==============================================================================
static bool MQTT_Init(AWS_IoT_Client* client)
{
	memset(client, 0, sizeof(AWS_IoT_Client));
	return true;
}
//------------------------------------------------------------------------------
void MQTT_Alarm(MQTT_ALARM alarm_code, MQTT_MESSAGE* alarming_message)
{
	// THIS OUGHT TO FILL A MAILBOX FOR THE MQTT STUFF
	switch( alarm_code )
	{
		case MQTT_MESSAGE_FAILED_A_FEW_TIMES:
		    mqtt_printf("MQTT_MESSAGE_FAILED_A_FEW_TIMES to topic %s\r\n", alarming_message->subtopic);
    		break;

		case MQTT_MESSAGE_FAILED_TOO_MANY_TIMES:
		    mqtt_printf("MQTT_MESSAGE_FAILED_TOO_MANY_TIMES to topic %s\r\n", alarming_message->subtopic);
			break;

		default:
			mqtt_printf("No idea what this error code means: %d\r\n", alarm_code);
			break;
	}
}
//------------------------------------------------------------------------------
// If the Server responds to a credential request with
// the x-update:1 field, then there will be no
// credentials.
void MQTT_notify_no_creds(void)
{
	no_creds = true;
}
//------------------------------------------------------------------------------
MQTT_CONNECTION_STATE get_mqtt_connection_state(void)
{
	return mqtt_connection_state;
}
//------------------------------------------------------------------------------
void MQTT_Task(void *pvParameters)
{
	bool clean_connect = false;
	EventBits_t ConnStatus;
	uint32_t connect_delay_timestamp = 0;
	uint32_t connection_attempts_timestamp = get_microseconds_tick();
	bool restart_network = true;
	static bool first_time = true;

	while(true)
	{
		// If we have received an x-update:1 during our credential request,
		// then we stop the state machine and supress the MQTT watchdog
		if(no_creds)
		{
			mqtt_connection_state = MQTT_STATE_STOP;
		}

		DebugCounter.mqtt_task_circle++;

		switch(mqtt_connection_state)
		{
			case MQTT_STATE_INITIAL:
				//Alternate Red
				process_system_event(STATUS_GETTING_CREDENTIALS);
				mqtt_printf("\tInit\r\n");
				xEventGroupSetBits(xConnectionStatus_EventGroup, CONN_STATUS_STARTING);
				if(MQTT_Init(&aws_client))
				{
					mqtt_connection_state = MQTT_STATE_GET_TIME;
				}
				break;

			case MQTT_STATE_GET_TIME:
				ConnStatus = xEventGroupWaitBits(xConnectionStatus_EventGroup, CONN_STATUS_NETWORK_UP, false, false, pdMS_TO_TICKS( 1000 ));
				if(CONN_STATUS_NETWORK_UP & ConnStatus)
				{
					// Network's up, so let's get cooking.
					if(SNTP_AwaitUpdate(false, pdMS_TO_TICKS(1000)))
					{
						mqtt_connection_state = MQTT_STATE_GET_CREDENTIALS;
						//process_system_event(STATUS_GETTING_CREDENTIALS);//Alternate Red
					}
					else
					{
						// Failed to get the time, which we need for TLS, so come back later.
						uint32_t rand = hermes_rand();

						// randomise it a bit to hopefully avoid NTP kiss of death
						vTaskDelay(pdMS_TO_TICKS(3000 + (rand & 0xfff)));
					}
				}
				break;

			case MQTT_STATE_GET_CREDENTIALS:
				if(MQTT_Get_Credentials(&aws_credentials))
				{
					process_system_event(STATUS_CONNECTING_TO_CLOUD);//Alternate green
					connect_delay_timestamp = get_microseconds_tick();
					if(first_time)
					{
						// alternate green for fake reasons only on first connect
						first_time = false;
						mqtt_connection_state = MQTT_STATE_DELAY;
					}
					else
					{
						connection_attempts_timestamp = get_microseconds_tick();
						mqtt_connection_state = MQTT_STATE_CONNECT;
					}
					mqtt_printf("\tConnecting\r\n");
				}
				break;

			case MQTT_STATE_DELAY:
				// This state is just a delay to allow the alternate green LED pattern
				// to show for a minimum period (otherwise it may be practically
				// instantaneous which is at odds with the manuals and App
				if((get_microseconds_tick() - connect_delay_timestamp) > (MIN_GREEN_FLASH_DURATION * usTICK_SECONDS))
				{
					connection_attempts_timestamp = get_microseconds_tick();
					mqtt_connection_state = MQTT_STATE_CONNECT;
				}
				break;

			case MQTT_STATE_CONNECT:
				if(MQTT_Connect(&aws_client, &aws_credentials, clean_connect))
				{
					mqtt_connection_state = MQTT_STATE_SUBSCRIBE;
				}

				if((get_microseconds_tick() - connection_attempts_timestamp) > CONNECT_ATTEMPTS_TIMEOUT)
				{
					xQueueSend(xRestartNetworkMailbox, &restart_network, 0);
					zprintf(LOW_IMPORTANCE,"MQTT Connect timeout - triggering network restart...\r\n");
					// restart everything, including getting new credentials
					mqtt_connection_state = MQTT_STATE_INITIAL;
				}
				break;

			case MQTT_STATE_SUBSCRIBE:
				mqtt_printf("\tSubscribe\r\n");
				if(MQTT_Subscribe(&aws_client, &aws_credentials))
				{
					// Chris has seen it get stuck in this state...
					mqtt_connection_state = MQTT_STATE_CONNECTED;
					process_system_event(STATUS_CONNECTED_TO_CLOUD);
					mqtt_printf("\tConnected\r\n");
					xEventGroupSetBits(xConnectionStatus_EventGroup, CONN_STATUS_MQTT_UP);
				}
				break;

			case MQTT_STATE_CONNECTED:
				if(!MQTT_Poll(&aws_client, &aws_credentials))
				{
					mqtt_connection_state = MQTT_STATE_DISCONNECT;
				}
				break;

			case MQTT_STATE_DISCONNECT:
				mqtt_printf("\tDisconnecting\r\n");
				if(MQTT_Disconnect(&aws_client))
				{
					// process_system_event(STATUS_GETTING_CREDENTIALS); // Alternate Red
					xEventGroupClearBits(xConnectionStatus_EventGroup, CONN_STATUS_MQTT_UP);
					// connection_attempts_timestamp = get_microseconds_tick();
					mqtt_connection_state = MQTT_STATE_INITIAL;
				}
				break;

			case MQTT_STATE_STOP:
				// do nothing - a f/w update is on it's way
				break;

			case MQTT_STATE_BACKSTOP:
			default:
				mqtt_connection_state = MQTT_STATE_GET_CREDENTIALS;
				break;
		}

		 osDelay(pdMS_TO_TICKS(10));
	}
}
//------------------------------------------------------------------------------
static void AWS_Hash(uint8_t* data, uint32_t size, char* result)
{
	wc_Sha256 sha;

	if(size > CREDS_ABSOLUTE_MAX_LENGTH)
	{
		return;
	}

	if (wc_InitSha256(&sha) != 0)
	{
		return;
	}

	if (wc_Sha256Update(&sha, data, size) != 0)
	{
		return;
	}

	if (wc_Sha256Final(&sha, (byte*)result) != 0)
	{
		return;
	}

	word32 out_size = 48;
	Base64_Encode((const byte*)result, WC_SHA256_DIGEST_SIZE, (byte*)result, &out_size);

	//mqtt_printf("Hash Results: %d %d %d\r\n", returns[0], returns[1], returns[2]);
}
//------------------------------------------------------------------------------
uint32_t MQTT_Unpack_Credentials(SUREFLAP_CREDENTIALS* credentials)
{
	credentials->decode_result = 0;

	if(strlen(credentials->certificate) > 0)
	{
		WC_PKCS12* pkcs = wc_PKCS12_new();

		credentials->unpacked_cert_length = strlen(credentials->certificate);

		credentials->decode_result = Base64_Decode((byte*)credentials->certificate,
													credentials->unpacked_cert_length,
													(byte*)credentials->certificate,
													(word32*)&credentials->unpacked_cert_length);

		if (credentials->decode_result != 0)
		{
			wc_PKCS12_free(pkcs);
			//vPortFree(credentials->certificate);
			credentials->certificate = NULL;

			return credentials->decode_result;
		}

		credentials->decode_result = wc_d2i_PKCS12((byte*)credentials->certificate,
													(word32)credentials->unpacked_cert_length,
													pkcs);

		if(!credentials->decode_result)
		{
			credentials->decode_result = wc_PKCS12_parse(pkcs,
														(char const*)GetDerivedKey_text(),
														(byte**)&credentials->decoded_key,
														(word32*)&credentials->decoded_key_size,
														(byte**)&credentials->decoded_cert,
														(word32*)&credentials->decoded_cert_size,
														NULL);
		}

		wc_PKCS12_free(pkcs);
		//vPortFree(credentials->certificate);
		credentials->certificate = NULL;

		if(credentials->decode_result == 0)
		{
			MQTT_Stored_Certificate.size = credentials->decoded_cert_size;
			memcpy(MQTT_Stored_Certificate.data,
					(uint8_t*)credentials->decoded_cert,
					credentials->decoded_cert_size);

			MQTT_Stored_Private_Key.size = credentials->decoded_key_size;
			memcpy(MQTT_Stored_Private_Key.data,
					(uint8_t*)credentials->decoded_key,
					credentials->decoded_key_size);

			vPortFree(credentials->decoded_cert);
			vPortFree(credentials->decoded_key);

			credentials->decoded_cert = 0;
			credentials->decoded_key = 0;
		}
	}

	if(credentials->decode_result == 0)
	{
		MQTT_Hash_It_Up(credentials);
	}

	return credentials->decode_result;
}
//------------------------------------------------------------------------------
static void MQTT_Hash_It_Up(SUREFLAP_CREDENTIALS* creds)
{
	creds->decoded_cert_size = MQTT_Stored_Certificate.size;
	creds->decoded_key_size = MQTT_Stored_Private_Key.size;
	creds->decoded_cert = (void*)MQTT_Stored_Certificate.data;
	creds->decoded_key = (void*)MQTT_Stored_Private_Key.data;

	AWS_Hash((uint8_t*)creds->decoded_cert, creds->decoded_cert_size, creds->cert_hash);
	AWS_Hash((uint8_t*)creds->decoded_key, creds->decoded_key_size, creds->key_hash);

	creds->combined_hash = 0;
	for(uint32_t i = 0; i < AWS_HASH_MAX_LENGTH; i += 4)
	{
		creds->combined_hash ^= *(uint32_t*)&creds->cert_hash[i];
		creds->combined_hash ^= *(uint32_t*)&creds->key_hash[i];
	}
}
//------------------------------------------------------------------------------
static bool MQTT_Load_Credential_Field(char* buffer, uint32_t* index, char* field, uint32_t max_size)
{
	uint32_t field_index	= 0;
	bool ret	= true;

	// Copy all text before the delimiter ':' into the field, up to a maximum.
	while(buffer[*index] && (buffer[*index] != ':') && (field_index < (max_size-1)))
	{
		field[field_index++] = buffer[(*index)++];
	}

	// If we ended before finding a delimiter, something went wrong.
	if(buffer[*index] != ':')
	{
		ret = false;
	}

	// Cap the field with a null terminator, so we can use str functions.
	field[field_index] = '\0';
	(*index)++;
	return ret;
}
//------------------------------------------------------------------------------
static bool MQTT_Interpret_Credentials(SUREFLAP_CREDENTIALS* creds, char* response)
{
	bool valid_creds = true;
	char network_type[2];
	uint32_t progress	= 0;
	//strstr(response, "\r\n\r\n");
	char*		data_start 	= response;

	if((strlen(response) < MQTT_MIN_CRED_LENGTH)
	|| (data_start == NULL))
	{
		return false;
	}

	//(uint32_t)(data_start - response) + strlen("\r\n\r\n");
	progress = 0;

	if(valid_creds)
	{
		valid_creds = MQTT_Load_Credential_Field(response, &progress, creds->version, sizeof(creds->version));
	}

	if(valid_creds)
	{
		valid_creds = MQTT_Load_Credential_Field(response, &progress, creds->id, sizeof(creds->id));
	}

	if(valid_creds)
	{
		valid_creds = MQTT_Load_Credential_Field(response, &progress, creds->client_id, sizeof(creds->client_id));
	}

	if(valid_creds)
	{
		valid_creds = MQTT_Load_Credential_Field(response, &progress, creds->username, sizeof(creds->username));
	}

	if(valid_creds)
	{
		valid_creds = MQTT_Load_Credential_Field(response, &progress, creds->password, sizeof(creds->password));
	}

	if(valid_creds)
	{
		valid_creds = MQTT_Load_Credential_Field(response, &progress, network_type, sizeof(network_type));
		if(network_type[0] == '0')
		{
			creds->network_type = SUREFLAP_NETWORK_TYPE_XIVELY;
		}

		else
		{
			creds->network_type = SUREFLAP_NETWORK_TYPE_AWS;
		}
	}

	if(valid_creds)
	{
		valid_creds = MQTT_Load_Credential_Field(response, &progress, creds->base_topic, sizeof(creds->base_topic));
	}

	if(valid_creds)
	{
		valid_creds = MQTT_Load_Credential_Field(response, &progress, creds->host, sizeof(creds->host));
	}

	if(valid_creds)
	{
		creds->certificate = aws_certificat;//pvPortMalloc(CERTIFICATE_MAX_SIZE);
		if(!creds->certificate)
		{
			valid_creds = false;
		}
	}

	if(valid_creds)
	{
		MQTT_Load_Credential_Field(response, &progress, creds->certificate, CERTIFICATE_MAX_SIZE-1);
	}

	if((valid_creds) && (strlen(creds->client_id) < AWS_CLIENT_ID_MIN_LENGTH))
	{
		valid_creds = false;
	}

	return valid_creds;
}
//------------------------------------------------------------------------------
typedef enum
{
	CRED_REQ_USE_FLASH_KEY,
	CRED_REQ_USE_RAM_KEY,
} CRED_REQ_KEY_TYPE;

static char response_buffer[CREDENTIAL_BUFFER_SIZE];

static bool MQTT_Send_Credential_Request(SUREFLAP_CREDENTIALS* creds)
{
	static char fullContent[] = "serial_number=####-#######"
								"&mac_address=################"
								"&product_id=1"
								"&firmware_version=######.######"
								"&bv=################################"
								"&tv=##############\0"; //&cred_hash=########\0";

	static char* postContent = "serial_number=%s"
								"&mac_address=0000%02X%02X%02X%02X%02X%02X&product_id=1"
								"&firmware_version=%06d.%06d"
								"&bv=%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
								"&tv=0%s\0";
								//"&tv=%014lu\0"; //&cred_hash=%08X\0";

	bool		http_req_result	= false;
	bool		parse_result 	= false;
	bool		retval			= false;
	extern 		PRODUCT_CONFIGURATION product_configuration;
	uint8_t 	*SharedSecret;
	DERIVED_KEY_SOURCE key;
	uint64_t 	time_since_epoch_ms;
	// indicates if x-enc was present in HTTP response header. We don't care here.
	int32_t		encrypted_data;
	static CRED_REQ_KEY_TYPE cred_req_key_type = CRED_REQ_USE_FLASH_KEY;

	// check if the Derived Key stored in Flash is valid
	if(checkFlashDerivedKey() && (CRED_REQ_USE_FLASH_KEY == cred_req_key_type))
	{
		zprintf(LOW_IMPORTANCE,"Using Derived Key from Flash\r\n");
		// use the Derived Key from before the power cycle / restart
		key = DERIVED_KEY_FLASH;
	}
	else
	{
		zprintf(LOW_IMPORTANCE,"Using Derived Key from RAM\r\n");
		// there is no old key, so use the current key
		key = DERIVED_KEY_CURRENT;
	}

	MQTT_Hash_It_Up(creds);

	SharedSecret = GetSharedSecret();

	get_UTC_ms(&time_since_epoch_ms);

	char str_time[15] = { 0 };

	xConverterUInt64ToStr(str_time, time_since_epoch_ms);

	uint32_t length = snprintf(fullContent, sizeof(fullContent),
			postContent,
			product_configuration.serial_number,

			product_configuration.ethernet_mac[0], product_configuration.ethernet_mac[1],
			product_configuration.ethernet_mac[2], product_configuration.ethernet_mac[3],
			product_configuration.ethernet_mac[4], product_configuration.ethernet_mac[5],

			SVN_REVISION, BUILD_MARK,

			SharedSecret[0],SharedSecret[1],SharedSecret[2],SharedSecret[3],
			SharedSecret[4],SharedSecret[5],SharedSecret[6],SharedSecret[7],
			SharedSecret[8],SharedSecret[9],SharedSecret[10],SharedSecret[11],
			SharedSecret[12],SharedSecret[13],SharedSecret[14],SharedSecret[15],

			str_time);

	if(length > sizeof(fullContent))
	{
		return false;
	}

	//char* response_buffer = pvPortMalloc(CREDENTIAL_BUFFER_SIZE);
	//char response_buffer[CREDENTIAL_BUFFER_SIZE];// = malloc(CREDENTIAL_BUFFER_SIZE);//;mem_malloc(CREDENTIAL_BUFFER_SIZE);
	//if(response_buffer)
	{
		memset(response_buffer, 0, CREDENTIAL_BUFFER_SIZE);	// cannot guarantee that creds are null terminated.
		// Note we always use the current key for checking the signature of the response because
		// we always send the bv= argument with a credential request, hence updating the key
		// on the Server.
		http_req_result = HTTP_POST_Request(HUB_API_SERVER,
											"/api/credentials",
											fullContent,
											response_buffer,
											CREDENTIAL_BUFFER_SIZE,
											true,
											key,
											DERIVED_KEY_CURRENT,
											&encrypted_data,
											NULL);

		// Note that an 'empty' response from the server is v02:0::::1::: and this causes MQTT_Interpret_Credentials() to return false
		if(http_req_result)
		{
			parse_result = MQTT_Interpret_Credentials(creds, response_buffer);
		}
		//vPortFree(response_buffer);
		//mem_free(response_buffer);
	}

	if(http_req_result && parse_result)
	{
		// To get this far, we have used the Derived Key to check the signature on the credentials
		// from the Server, and also used it to decrypt them. So it must be OK.
		zprintf(LOW_IMPORTANCE,"Storing Derived Key in Flash\r\n");

		if (key != DERIVED_KEY_FLASH)
		{
			StoreDerivedKey();
		}

		cred_req_key_type = CRED_REQ_USE_FLASH_KEY;

		// AWS_SUCCESS
		retval = true;
	}
	else
	{	// we got a response, but the parser failed to parse it. This probably means the server did not like the
		// key we used, so responded with v02:0::::1:::
		// The server may have also not sent a signature if it didn't like our one
		zprintf(LOW_IMPORTANCE,"Server probably did not like my signature\r\n");
		// try the RAM key next time
		cred_req_key_type = CRED_REQ_USE_RAM_KEY;
		retval = false;
	}

	return retval;
}
//------------------------------------------------------------------------------
static bool MQTT_Get_Credentials(SUREFLAP_CREDENTIALS* creds)
{
	static const BACKOFF_SPECS	cred_backoff_specs =
	{
		MQTT_CRED_RETRY_BASE,
		MQTT_CRED_RETRY_MULT,
		MQTT_CRED_RETRY_JITTER,
		MQTT_CRED_RETRY_MAX
	};

	static BACKOFF	cred_backoff =
	{
		0, MQTT_CRED_RETRY_BASE,
		0, &cred_backoff_specs
	};

	BACKOFF_RESULT	backoff_result = Backoff_GetStatus(&cred_backoff);
	switch(backoff_result)
	{
		case BACKOFF_READY:

		case BACKOFF_FINAL_ATTEMPT:
			mqtt_printf("\tAttempting Credential Request... ");
			break;

		case BACKOFF_WAITING:
			osDelay(pdMS_TO_TICKS(1000));
			return false;

		case BACKOFF_FAILED:

		default:
			osDelay(pdMS_TO_TICKS(10000));
			Backoff_Reset(&cred_backoff);
			return false;
	}

	bool result	= MQTT_Send_Credential_Request(creds);

	if(result)
	{
		MQTT_Unpack_Credentials(creds);
	}

	Backoff_Progress(&cred_backoff);

	return result;
}
//------------------------------------------------------------------------------
static bool MQTT_Connect(AWS_IoT_Client* client, SUREFLAP_CREDENTIALS* credentials, bool clean_connect)
{
	static const BACKOFF_SPECS	connect_backoff_specs =
	{
		MQTT_CONNECT_RETRY_BASE,
		MQTT_CONNECT_RETRY_MULT,
		MQTT_CONNECT_RETRY_JITTER,
		MQTT_CONNECT_RETRY_MAX
	};

	static BACKOFF	connect_backoff	= {0, MQTT_CONNECT_RETRY_BASE, 0, &connect_backoff_specs};
	static char		last_will[]		= "Hub offline (Last Will): 00 00 00 00\0";
	static char		online_msg[]	= "Hub online: 00 00 00 00\0";
	HERMES_TIME_GMT	gmt_time 		= {0, 0, 0, 0};

	BACKOFF_RESULT	backoff_result = Backoff_GetStatus(&connect_backoff);
	switch(backoff_result)
	{
		case BACKOFF_READY:
		case BACKOFF_FINAL_ATTEMPT:
			mqtt_printf("\tAttempting Connection... ");
			break;

		case BACKOFF_WAITING:
			vTaskDelay(pdMS_TO_TICKS(1000));
			return false;

		case BACKOFF_FAILED:
		default:
			vTaskDelay(pdMS_TO_TICKS(1000));
			Backoff_Reset(&connect_backoff);
			return false;
	}

	get_gmt(get_UTC(), &gmt_time);

	sprintf(strchr(last_will, ':'), ": %02x %02x %02x %02x\0", gmt_time.day, gmt_time.hour, gmt_time.minute, gmt_time.second);
	sprintf(strchr(online_msg, ':'), ": %02x %02x %02x %02x\0", gmt_time.day, gmt_time.hour, gmt_time.minute, gmt_time.second);

	IoT_Error_t result = AWS_Connect(client, credentials, last_will, clean_connect);

	if(result == AWS_SUCCESS)
	{
		Backoff_Reset(&connect_backoff);
		SERVER_MESSAGE	online_buffered_message = {(uint8_t*)online_msg, 0};
		xQueueSend(xBufferMessageMailbox, &online_buffered_message, 0);
		return true;
	}

	mqtt_printf("Failure: %d\r\n", result);
	Backoff_Progress(&connect_backoff);

	return false;
}
//------------------------------------------------------------------------------
static bool MQTT_Subscribe(AWS_IoT_Client* client, SUREFLAP_CREDENTIALS* credentials)
{
	const BACKOFF_SPECS	subscribe_backoff_specs	=
	{
		MQTT_SUBS_RETRY_BASE,
		MQTT_SUBS_RETRY_MULT,
		MQTT_SUBS_RETRY_JITTER,
		MQTT_SUBS_RETRY_MAX
	};

	BACKOFF subscribe_backoff = { 0, MQTT_SUBS_RETRY_BASE, 0, &subscribe_backoff_specs };

	BACKOFF_RESULT	backoff_result = Backoff_GetStatus(&subscribe_backoff);

	switch(backoff_result)
	{
		case BACKOFF_READY:
		case BACKOFF_FINAL_ATTEMPT:
			break;

		case BACKOFF_WAITING:
			vTaskDelay(pdMS_TO_TICKS(1000));
			return false;

		case BACKOFF_FAILED:
			mqtt_printf("\tSubscribe failed too many times. Disconnect and try again.\r\n");
			mqtt_connection_state = MQTT_STATE_DISCONNECT;

		default:
			vTaskDelay(pdMS_TO_TICKS(1000));
			return false;
	}

	IoT_Error_t result = AWS_Resubscribe(client);

	if(result == AWS_SUCCESS || result == MQTT_RX_BUFFER_TOO_SHORT_ERROR)
	{
		Backoff_Reset(&subscribe_backoff);
		return true;
	}

	Backoff_Progress(&subscribe_backoff);

	return false;
}
//------------------------------------------------------------------------------
static bool MQTT_Poll(AWS_IoT_Client* client, SUREFLAP_CREDENTIALS* credentials)
{
	extern QueueHandle_t xOutgoingMQTTMessageMailbox;

	static MQTT_MESSAGE outgoing_message;
	char signed_message[BASE_TOPIC_MAX_SIZE + MAX_INCOMING_MQTT_MESSAGE_SIZE_SMALL + SIGNATURE_LENGTH_ASCII + 1];
	static MQTT_MESSAGE* pending_message = NULL;

	IoT_Error_t result;
	EventBits_t ConnStatus = xEventGroupGetBits(xConnectionStatus_EventGroup);

	if(CONN_STATUS_NETWORK_UP != (ConnStatus & CONN_STATUS_NETWORK_UP))
	{
		aws_printf("\r\n--- Network Down\r\n");
		return false;
	}

	if(pending_message || (pdPASS == xQueueReceive(xOutgoingMQTTMessageMailbox, &outgoing_message, 50)))
	{
		// We now need to sign the outgoing message in accordance with the current
		// value of the Derived Key.
		// The signature is the first 65 bytes of the message body (64 bytes signature
		// and 1 byte space)
		// Note that we need to re-sign it each time we try to send it in case the Derived Key
		// has changed.
		snprintf(signed_message,
				(BASE_TOPIC_MAX_SIZE + MAX_INCOMING_MQTT_MESSAGE_SIZE_SMALL + SIGNATURE_LENGTH_ASCII + 1),
				"%s/messages%s%s",
				credentials->base_topic,
				outgoing_message.subtopic,
				outgoing_message.message);

		CalculateSignature((uint8_t *)signed_message, DERIVED_KEY_CURRENT, (uint8_t *)signed_message, strlen(signed_message));
		sprintf(&signed_message[64]," %s",outgoing_message.message);
		pending_message = &outgoing_message;

		result = AWS_Publish(client, credentials, pending_message->subtopic, signed_message, strlen(signed_message), QOS1);
		if(result == AWS_SUCCESS)
		{
			pending_message = NULL;
		}
	}

	result = aws_iot_mqtt_yield(client, AWS_YIELD_TIMEOUT);
	if((AWS_SUCCESS != result)
	&& (NETWORK_SSL_NOTHING_TO_READ != result)
	&& (MQTT_RX_BUFFER_TOO_SHORT_ERROR != result))
	{
		aws_printf("\r\n--- Yield Failed: %d\r\n", result);
		return false;
	}

	if( MQTT_RX_BUFFER_TOO_SHORT_ERROR == result )
	{
		aws_printf("\r\n--- Too long a message received. ---\r\n");
	}

	return true;
}
//------------------------------------------------------------------------------
static bool MQTT_Disconnect(AWS_IoT_Client* client)
{
	IoT_Error_t result = aws_iot_mqtt_disconnect(client);
	if((AWS_SUCCESS == result) || (NETWORK_DISCONNECTED_ERROR == result))
	{
		return true;
	}

	aws_iot_mqtt_yield(client, AWS_YIELD_TIMEOUT);

	return false;
}

/**************************************************************
 * Function Name   : mqtt_status_dump
 * Description     : Dumps to the console the status of the MQTT engine
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
void mqtt_status_dump(void)
{
	zprintf(CRITICAL_IMPORTANCE,"MQTT status:\r\nConnectionStatus: ");
	if ((xEventGroupGetBits(xConnectionStatus_EventGroup) & CONN_STATUS_NETWORK_UP) !=0)
	{
		zprintf(CRITICAL_IMPORTANCE,"CONN_STATUS_NETWORK_UP ");
	}

	if ((xEventGroupGetBits(xConnectionStatus_EventGroup) & CONN_STATUS_MQTT_UP) !=0)
	{
		zprintf(CRITICAL_IMPORTANCE,"CONN_STATUS_MQTT_UP ");
	}

	zprintf(CRITICAL_IMPORTANCE,"\r\nMQTT state machine: %s\r\n",mqtt_states[mqtt_connection_state]);
}

#else // MQTT_SERVER_SIMULATED
void mqtt_status_dump(void)
{
	zprintf(HIGH_IMPORTANCE,"mqttstat is not supported when in MQTT simulator mode\r\n");
}

#endif	// MQTT_SERVER_SIMULATED
