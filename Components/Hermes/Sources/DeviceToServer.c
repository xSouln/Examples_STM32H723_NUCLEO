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
* Filename: DeviceToServer.c   
* Author:   Chris Cowdery
* Purpose:  19/11/2019   
*
* This file handles messages received from a device, and passes them to the server buffer.
* It handles the necessary format translation from T_MESSAGE to a string formatted for the server        
*             
**************************************************************************/
#include "hermes.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "hermes-time.h"
#include "SureNet-Interface.h"
#include "Server_Buffer.h"
#include "DeviceToServer.h"
#include "message_parser.h"

// This is 2 bytes for command, 2 for length and 1 for parity
#define MESSAGE_OVERHEAD	5

// Local functions
void packet_dump(RECEIVED_PACKET *rx_packet);

extern uint8_t hub_debug_mode;
/**************************************************************
 * Function Name   : surenet_data_received_cb
 * Description     : Process messages received over SureNet from a device
 * Inputs          : Pointer to a packet.
 * Outputs         :
 * Returns         : TRUE if the message could be processed (SureNet will return an ACK), FALSE if not (NACK).
 **************************************************************/
SN_DATA_RECEIVED_RESPONSE surenet_data_received_cb(RECEIVED_PACKET *rx_packet)
{
	T_MESSAGE *rx_message;	
	SERVER_MESSAGE server_message;
	// the largest message that can arrive from a device is
	uint8_t message_body[30 + T_MESSAGE_PAYLOAD_SIZE*3];
	// a THALAMUS_MULTIPLE which could be MESSAGE_PAYLOAD_SIZE
    server_message.message_ptr = message_body;
	uint16_t registerNumber,length;
	uint16_t i,pos;
	static uint8_t server_msg_index = 0;
	SN_DATA_RECEIVED_RESPONSE retval = SN_ACCEPTED;
	
	// zprintf(LOW_IMPORTANCE, "surenet_data_received_cb():\r\n");
	// packet_dump(rx_packet);
	
	// we can do a sanity check here as we know that we only receive packets 
	// of type T_MESSAGE. Because we know that T_MESSAGE contains a length, and
	// so does RECEIVED_PACKET, we can check for consistency:
	if((rx_packet->packet.header.packet_length - 23) != rx_packet->packet.payload[2])
	{
		return SN_CORRUPTED;
	}
	
	//skip past SureNet Header
	rx_message = (T_MESSAGE *)((uint8_t *)rx_packet+sizeof(HEADER));
	registerNumber = (uint16_t)((rx_message->payload[1] & 0xff) + (rx_message->payload[0] << 8));
	length = (uint16_t)((rx_message->payload[3] & 0xff) + (rx_message->payload[2] << 8));
			
	switch(rx_message->command)
	{
		case COMMAND_GET_REG:
			// Device asking the server for register values
			// arguments are MSG_GET_REG_RANGE reg_address num_of_registers
			sprintf((char *)message_body,"%d %d %d",MSG_GET_REG_RANGE, registerNumber, length);
			server_message.source_mac = rx_packet->packet.header.source_address;

			// attempt to queue message
			if(!server_buffer_add(&server_message))
			{
				// couldn't be queued.
				retval = SN_REJECTED;
			}
			break;

		case COMMAND_SET_REG:
			// Device sending one or more registers to the server
			// arguments are MSG_REG_VALUES_INDEX_ADDED index reg_address num_of_registers val1 val2 val3 etc.
			// here we screen out time update COMMAND_SET_REG messages if hub_debug_mode bit 2 is not set.
			// Bit 7 of payload[6] = minutes is set if the device is requesting the server to send the time.
			// These messages need to be sent. Otherwise hourly time messages get discarded
			// and the only time messages sent are the ones generated by the hub based on the device awake messages.
			// Don't know what is going on here. Removing this allows the Hourly messgaes to get through,
			// which they do with Hub1...?!?!
			// if ( ((registerNumber==33&&length==3&&(rx_message->payload[6]<128)) || (registerNumber==634&&length==16)) &&
			// (hub_debug_mode!=HUB_SEND_TIME_UPDATES_FROM_DEVICES))  //discard PET DOOR hourly messages unless send time bit is set
			// {
			// discard message
			// } else if( MOVEMENT_DUMMY_REGISTER == registerNumber)
		
			if(MOVEMENT_DUMMY_REGISTER == registerNumber)
			{
				// This is a Pet Door Movement event, which is a group of registers sent atomically. Length is assumed to be correct!
				pos = sprintf((char*)message_body,"%d",MSG_MOVEMENT_EVENT);

				// build string containing list of values
				for (i=0; i<length; i++)
				{
					pos+=sprintf((char *)&message_body[pos]," %02x",rx_message->payload[4+i]);
				}
				server_message.source_mac = rx_packet->packet.header.source_address;
				
				// attempt to queue message
				if(!server_buffer_add(&server_message))
				{
					// couldn't be queued.
					retval = SN_REJECTED;
				}

			}
			else
			{
				pos = sprintf((char *)message_body,"%d %d %d %d",
						MSG_REG_VALUES_INDEX_ADDED, server_msg_index++, registerNumber, length);

				// build string containing list of values
				for(i = 0; i<length; i++)
				{
					pos+=sprintf((char *)&message_body[pos]," %02x",rx_message->payload[4+i]);
				}

				server_message.source_mac = rx_packet->packet.header.source_address;

				// attempt to queue message
				if(!server_buffer_add(&server_message))
				{
					// couldn't be queued.
					retval = SN_REJECTED;
				}
			}
			break;

		case COMMAND_THALAMUS:
			// Device is sending a single Thalamus format message
			// arguments are MSG_HUB_THALAMUS va1 val2 val3 etc.
			pos = sprintf((char *)message_body,"%d",MSG_HUB_THALAMUS);

			// build string containing list of values
			for(i = 0; i < ((rx_message->length) - MESSAGE_OVERHEAD); i++)
			{
				pos += sprintf((char *)&message_body[pos]," %02x",rx_message->payload[i]);
			}
			server_message.source_mac = rx_packet->packet.header.source_address;

			// attempt to queue message
			if(!server_buffer_add(&server_message))
			{
				// couldn't be queued.
				retval = SN_REJECTED;
			}
			else
			{

			}
			break;

		case COMMAND_THALAMUS_MULTIPLE:
			// Device is sending a multiple Thalamus format message
			// arguments are MSG_HUB_THALAMUS va1 val2 val3 etc.
			pos = sprintf((char *)message_body,"%d",MSG_HUB_THALAMUS_MULTIPLE);

			// build string containing list of values
			for(i = 0; i < ((rx_message->length) - MESSAGE_OVERHEAD); i++)
			{
				pos += sprintf((char *)&message_body[pos]," %02x",rx_message->payload[i]);
			}

			server_message.source_mac = rx_packet->packet.header.source_address;

			if(!server_buffer_add(&server_message))
			{
				// attempt to queue message
				// couldn't be queued.
				retval = SN_REJECTED;
			}
			break;

		default:
			zprintf(MEDIUM_IMPORTANCE, "Received unhandled message command 0x%02X - dropping it\r\n", rx_message->command);
			break;
		
	}
	
	// the server buffer will gracefully deal with being full up, so there is no
	// requirement to tell our caller.
    return retval;
}

/**************************************************************
 * Function Name   : packet_dump
 * Description     : Dumps a bit of info out regarding a RECEIVED_PACKET
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
void packet_dump(RECEIVED_PACKET *rx_packet)
{
    uint16_t len;
    uint64_t addr;
    uint16_t i;
	
    // length of payload
    len = rx_packet->packet.header.packet_length - sizeof(HEADER);
    addr = rx_packet->packet.header.source_address;

    zprintf(LOW_IMPORTANCE, "packet type = %02d\r\n",rx_packet->packet.header.packet_type);
    zprintf(LOW_IMPORTANCE, "source addr = ");
    zprintf(LOW_IMPORTANCE, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
    		(uint8_t)(addr>>56),(uint8_t)(addr>>48),(uint8_t)(addr>>40),(uint8_t)(addr>>32),
			(uint8_t)(addr>>24),(uint8_t)(addr>>16),(uint8_t)(addr>>8),(uint8_t)(addr));

    zprintf(LOW_IMPORTANCE, "\r\nPayload: ");
    for(i = 0; i < len; i++)
    {
        zprintf(LOW_IMPORTANCE, "%02X ", rx_packet->packet.payload[i]);
    }
    zprintf(LOW_IMPORTANCE, "\r\n");
}
