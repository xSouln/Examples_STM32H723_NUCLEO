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
* Filename: hermes.h   
* Author:   Chris Cowdery
* Purpose:   
*   
**************************************************************************/
#ifndef __HERMES_H__
#define __HERMES_H__
//==============================================================================
//includes:

#include "Hermes-compiller.h"

#include "Hermes/Console/Hermes-console.h"

#include "Signing.h"
#include "cmsis_os.h"
//==============================================================================
//defines:

#define SHOULD_I_PACKAGE	true

// Number of watchdog resets required to cause a bank switch
#define MAX_WATCHDOG_RESETS	5

#define HUB_HARDWARE_REVISION_MAJOR 3
// this is populated with the binary weighted hardware resistor values on the PCB
#define HUB_HARDWARE_REVISION_MINOR 0

#define GPT1_ONE_SECOND 1000000

#define MYUSERNAME ""

// default is every hour
#define HUB_SEND_TIME_UPDATES_FROM_DEVICES_EVERY_MINUTE	0x01

// default is every hour
#define HUB_SEND_HUB_STATUS_UPDATES_EVERY_MINUTE 		0x02

#define HUB_SEND_TIME_UPDATES_FROM_DEVICES				0x04
//------------------------------------------------------------------------------

#define SUREFLAP_PAN_ID	0x3421

/* Default IP address configuration.  Used in ipconfigUSE_DNS is set to 0, or
ipconfigUSE_DNS is set to 1 but a DNS server cannot be contacted. */
#define configIP_ADDR0		192
#define configIP_ADDR1		168
#define configIP_ADDR2		0
#define configIP_ADDR3		10

/* Default gateway IP address configuration.  Used in ipconfigUSE_DNS is set to
0, or ipconfigUSE_DNS is set to 1 but a DNS server cannot be contacted. */
#define configGATEWAY_ADDR0	192
#define configGATEWAY_ADDR1	168
#define configGATEWAY_ADDR2	0
#define configGATEWAY_ADDR3	1

/* Default DNS server configuration.  OpenDNS addresses are 208.67.222.222 and
208.67.220.220.  Used in ipconfigUSE_DNS is set to 0, or ipconfigUSE_DNS is set
to 1 but a DNS server cannot be contacted.*/
#define configDNS_SERVER_ADDR0 	192
#define configDNS_SERVER_ADDR1 	168
#define configDNS_SERVER_ADDR2 	0
#define configDNS_SERVER_ADDR3 	1

/* Default netmask configuration.  Used in ipconfigUSE_DNS is set to 0, or
ipconfigUSE_DNS is set to 1 but a DNS server cannot be contacted. */
#define configNET_MASK0		255
#define configNET_MASK1		255
#define configNET_MASK2		255
#define configNET_MASK3		0

/* DHCP has an option for clients to register their hostname.  */
#define HUB2_HOSTNAME       "Sure Petcare Hub"

#define MIN_PRINT_LEVEL 0
#define MAX_PRINT_LEVEL 11

#define TERMINAL_PRINT_ENABLED	false
//------------------------------------------------------------------------------
// Stringification!
#define XSTR(s)	STR(s)
#define STR(s)	#s
#define GREETING_STRING "cjctamzjvc"
//------------------------------------------------------------------------------
#define __MEMORY_BARRIER()\
do\
{\
	__DSB();\
	if (SCB->CCR & SCB_CCR_DC_Msk)\
	{\
		SCB_CleanDCache();\
	}\
	__ISB();\
} while(0)
//==============================================================================
//types:

typedef struct
{
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
} CONTEXT;
//------------------------------------------------------------------------------

typedef enum
{
	// Unit has default values in PRODUCT_CONFIGURATION
	PRODUCT_BLANK,
	// Unit programmed and tested in the Programmer, but not seen the Label Printer yet
	PRODUCT_TESTED,
	// Unit been interrogated by the Label Printer and is now ready for shipping.
	PRODUCT_CONFIGURED,
	// used to indicate a non-configured unit, so some interrogation by the production equipment is allowed.
	PRODUCT_NOT_CONFIGURED = 0xff,

} PRODUCT_STATE;
//------------------------------------------------------------------------------

typedef struct
{
	uint8_t ethernet_mac[6];
	uint64_t rf_mac;
	uint16_t rf_pan_id;

	// H001-0000000\0
	uint8_t serial_number[13];

	PRODUCT_STATE product_state;

	// used to detect an uninitialised flash
	PRODUCT_STATE sanity_state;

	uint8_t secret_serial[33];
	bool rf_mac_mangle;
	uint8_t spare1;
	uint8_t spare2;
	uint8_t spare3;

	uint8_t DerivedKey[DERIVED_KEY_LENGTH];

} PRODUCT_CONFIGURATION;

//------------------------------------------------------------------------------
typedef HERMES__PACKED_PREFIX enum
{
	LOW_IMPORTANCE	= 2,
	MEDIUM_IMPORTANCE = 5,

	// Unexpected!
	HIGH_IMPORTANCE = 9,

	// What the absolute fuck?
	CRITICAL_IMPORTANCE	= 10,
	TERMINAL_IMPORTANCE = 10,

	//0xFF	// Print to debugger Terminal - faults if not debugging!
	NOT_ENCRYPT_IMPORTANCE = 11,

} HERMES__PACKED_POSTFIX ZPRINTF_IMPORTANCE;
//------------------------------------------------------------------------------
// N.B. Values whose first byte is 0x7F (127) are reserved for Pings.
typedef HERMES__PACKED_PREFIX enum
{
	// This enum should always be contiguous.
	MESSAGE_ACK,

	MESSAGE_NACK,
	MESSAGE_COMMAND,
	MESSAGE_RESPONSE,
	MESSAGE_GREETING,
	MESSAGE_BACKSTOP,

} HERMES__PACKED_POSTFIX HERMES_MESSAGE_TYPE;
//------------------------------------------------------------------------------

typedef HERMES__PACKED_PREFIX struct
{
	//zach
	uint32_t sync_word;

	HERMES_MESSAGE_TYPE type;

	//inverse of first two bytes
	uint8_t check;

  	uint16_t padding;

  	//length of the package in bytes
	uint32_t size;

	//quando (ms count)
	uint32_t timestamp;

	uint32_t correlation_ID;
	
} HERMES__PACKED_POSTFIX HERMES_UART_HEADER;
//------------------------------------------------------------------------------

typedef HERMES__PACKED_PREFIX struct
{
	HERMES_UART_HEADER header_in;

	//being naughty like chris. Coded by Thom
	char payload[256];

} HERMES__PACKED_POSTFIX HERMES_MESSAGE_RECEIVED;
//------------------------------------------------------------------------------

typedef HERMES__PACKED_PREFIX enum
{
	DEAL_NOT_ENOUGH_BYTES,
	DEAL_INVALID_HEADER = -1,

} HERMES__PACKED_POSTFIX MESSAGE_DEAL_RESULT;
//------------------------------------------------------------------------------

typedef struct
{
	int device_buffer_add;

	int aws_tx_errors_count;
	int aws_rx_errors_count;

	int aws_received_messages_count;
	int aws_publish_requests_count;
	int aws_publish_accepted_requests_count;

	int hermes_app_task_circle;
	int mqtt_task_circle;
	int sn_task_circle;

	int hermes_app_task_circle_per_second;
	int mqtt_task_circle_per_second;
	int sn_task_circle_per_second;

	int hub_state_conversation_send;

	int message_device_to_server;
	int message_device_to_server_add;

	int message_server_to_device;

} DebugCounterT;
//==============================================================================
//externs:

extern DebugCounterT DebugCounter;
//==============================================================================
//functions:

void HermesComponentInit();

int hermes_app_start(void);
void zprintf(ZPRINTF_IMPORTANCE test, const char *_Restrict, ...);
void set_product_state(PRODUCT_STATE state);
void sanitise_product_config(void);
void write_product_configuration(void);

// thread safe
void hermes_srand(int seed);

// thread safe
int hermes_rand(void);

void initSureNetByProgrammer(void);
void connectToServer(void);
//==============================================================================
#endif //__HERMES_H__
