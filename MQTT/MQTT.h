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
*
*
**************************************************************************/
#ifndef __MQTT_H__
#define __MQTT_H__
//==============================================================================
//includes:

//#define MQTT_SERVER_SIMULATED

#ifdef MQTT_SERVER_SIMULATED
#include "MQTT-Simulator.h"
#include "MQTT-Synapse.h"
#endif
//==============================================================================
//defines:

#define MQTT_REBOOT_TIMEOUT	(10 * usTICK_MINUTE)

// minimum duration that the LEDs flash green for when connecting
#define MIN_GREEN_FLASH_DURATION	5

// give it long enough for network watchdog to try to fix basic connection issues
#define CONNECT_ATTEMPTS_TIMEOUT	(usTICK_MINUTE * 3)

#define INCOMING_MQTT_MESSAGE_QUEUE_DEPTH_SMALL	16
#define INCOMING_MQTT_MESSAGE_QUEUE_DEPTH_LARGE	2

// Large enough for Thalamus messages + 64byte sig
#define MAX_INCOMING_MQTT_MESSAGE_SIZE_SMALL 	512

// Could be up to 82 chars.
#define MAX_INCOMING_MQTT_TOPIC_SIZE 			100

#define AWS_CLIENT_ID_MIN_LENGTH	5
#define MQTT_MIN_CRED_LENGTH		3000

// measured chunked response size was 3638, so 4096 has a bit of spare capacity
#define CREDENTIAL_BUFFER_SIZE		4096

#define PRINT_MQTT	false

#if PRINT_MQTT
#define mqtt_printf(...)	zprintf(MEDIUM_IMPORTANCE, __VA_ARGS__)
#else
#define mqtt_printf(...)
#endif
//==============================================================================
//types:

typedef enum
{
	MQTT_STATE_INITIAL,
	MQTT_STATE_GET_TIME,
	MQTT_STATE_GET_CREDENTIALS,
	MQTT_STATE_DELAY,
	MQTT_STATE_CONNECT,
	MQTT_STATE_SUBSCRIBE,
	MQTT_STATE_CONNECTED,
	MQTT_STATE_DISCONNECT,
	MQTT_STATE_STOP,
	MQTT_STATE_BACKSTOP

} MQTT_CONNECTION_STATE;
//------------------------------------------------------------------------------

typedef enum
{
    MQTT_MESSAGE_FAILED_A_FEW_TIMES,
    MQTT_MESSAGE_FAILED_TOO_MANY_TIMES,

} MQTT_ALARM;
//------------------------------------------------------------------------------

typedef struct
{
    char message[MAX_INCOMING_MQTT_MESSAGE_SIZE_SMALL];
    char subtopic[MAX_INCOMING_MQTT_TOPIC_SIZE];

} MQTT_MESSAGE;
//==============================================================================
//functions:

void MQTT_Alarm(MQTT_ALARM alarm_code, MQTT_MESSAGE* alarming_message);
void mqtt_status_dump(void);

#ifdef MQTT_SERVER_SIMULATED
#define MQTT_Task	MQTT_Simulator_task
#else
void MQTT_Task(void *pvParameters);
void MQTT_notify_no_creds(void);
MQTT_CONNECTION_STATE get_mqtt_connection_state(void);
#endif //MQTT_SERVER_SIMULATED
//==============================================================================
#endif //__MQTT_H__
