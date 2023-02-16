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
* Filename: SureNetDriver.c
* Author:   Chris Cowdery 
* 
* SureNet Driver top level file - effectively wraps Atmel Stack.
* It provides the following facilities:
* 1. Association with Devices
* 2. Sending and receiving messages to / from devices.
* We access the SPI via a FreeRTOS API, but the nRST, INT and SP signals are controlled directly
*           
**************************************************************************/
#include "hermes.h"
#include "hermes-time.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

 // isxdigit()
#include <ctype.h>

/* FreeRTOS includes. */
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

// SureNet
#include "Surenet-Interface.h"
#include "SureNetDriver.h"

// for MAX_NUMBER_OF_DEVICES
#include "SureNet.h"

// for one cheeky call to find out if is_mac_in_pairing_table()
#include "Devices.h"

#include "mac_api.h"
#include "tal.h"
#include "bmm.h"
#include "pal.h"
#include "app_config.h"

#include "debug.h"
//==============================================================================
#define PAIRING_MODE_TIME					(90 * usTICK_SECONDS)
#define	PAIRING_MODE_TIME_BEACON_REQUEST 	(10 * usTICK_SECONDS)

// Defines the short address of the coordinator.
// forces usage of long MAC always
#define COORD_SHORT_ADDR MAC_NO_SHORT_ADDR_VALUE
//==============================================================================
// Private typedefs
// This is used to store the type of remote device requesting a Beacon
typedef enum
{
    INVALID_DEVICE,
    NON_THALAMUS_BASED_DEVICE,
    THALAMUS_BASED_DEVICE,

} DEVICE_PLATFORM;
//------------------------------------------------------------------------------
// This is used to store a couple of attributes of a remote device requesting a Beacon
typedef struct
{
    uint64_t    mac_addr;

    // Thalamus or not
    DEVICE_PLATFORM device_platform;

    // RF Channel
    uint8_t device_channel;

} BEACON_REQUEST_DEVICE_DATA;
//------------------------------------------------------------------------------
/** This type definition of a structure can store the short address and the extended address of a device. */
typedef struct associated_device_tag
{
	uint16_t short_addr;
	uint64_t ieee_addr;

}associated_device_t;
//------------------------------------------------------------------------------
// Note that beacon_request_device_data.xx is not actually used, it is just stored. A hook could be added
// to pass it out higher up the stack if a need was found for the information.
BEACON_REQUEST_DEVICE_DATA beacon_request_device_data =
{
	.mac_addr = 0,
	.device_channel = 0,
	.device_platform = INVALID_DEVICE
};
//------------------------------------------------------------------------------

typedef enum
{
	BEACON_PAYLOAD_SUREFLAP_PROTOCOL_ID,
	BEACON_PAYLOAD_SUREFLAP_VERSION,
	BEACON_PAYLOAD_SOLE_PAN_COORDINATOR,

} BEACON_PAYLOAD_INDEX;
//------------------------------------------------------------------------------

typedef struct
{
	uint32_t 	timestamp;
	uint32_t	timeout;
	bool 		active;

} PAIRING_MODE_TIMEOUT;
//------------------------------------------------------------------------------

static uint8_t Beacon_Payload[] =
{
	SUREFLAP_HUB,
	HUB_SUPPORTS_THALAMUS,
	HUB_IS_SOLE_PAN_COORDINATOR
};
//------------------------------------------------------------------------------
static PAIRING_REQUEST requested_pairing_mode =
{
	0,
	false,
	PAIRING_REQUEST_SOURCE_UNKNOWN
};
//------------------------------------------------------------------------------
//local copy of pan_id
uint16_t pan_id;
uint64_t request_mac_addr;

PAIRING_MODE_TIMEOUT pairing_mode_timeout = { 0, false };

// used to indicate whether an update to the beacon payload is part of the
// initialisation (false) (and therefore the callback it triggers performs the next
// part of the sequence), or whether it's a 'run time' update of the payload (true)
// and should NOT perform the next part of the sequence.
volatile bool beacon_payload_update;

RX_BUFFER rx_buffer SN_RX_BUFFER_MEM_SECTION;

// stored when the association request arrives, for use if the request is successful
static ASSOCIATION_SUCCESS_INFORMATION assoc_info;

static uint8_t reentrancy_count = 0;
static volatile int irq_receive;
static int irq_accepted;

// this is the master reference.
static uint8_t current_channel = RF_CHANNEL1;
static uint8_t current_channel_page = 0;
//This array stores all device related information.
static associated_device_t device_list[MAX_NUMBER_OF_DEVICES] DEVICE_LIST_MEM_SECTION;

// ------------------------------0-------------------10--------15--------20----------26
int8_t TX_Power_Per_Channel[] = {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,-8};
//==============================================================================
// private functions:

static bool assign_new_short_addr(uint64_t addr64, uint16_t *addr16);
//==============================================================================
//functions:

// This is called really early on, when tasks are being created.
BaseType_t snd_init(uint64_t *mac_addr, uint16_t panid, uint8_t channel)
{
    BaseType_t xReturn = pdPASS;

	pan_id = panid;

	request_mac_addr = *mac_addr;

	snd_stack_init();

	for (uint8_t i = 0; i < 10; i++)
	{
		snd_stack_task();
		osDelay(10);
	}

	wpan_mlme_set_req(macIeeeAddress, &request_mac_addr);

	for (uint8_t i = 0; i < 10; i++)
	{
		snd_stack_task();
		osDelay(10);
	}

	snd_set_channel(channel);

    return xReturn;
}
//------------------------------------------------------------------------------
// This task is the SureNet Driver task. All activity relating to the Atmel AT86RF233 IC is handled in this task except
// that ISR's aren't, instead they are handled in rfisr_task and communicated with this one via Notifications
// Note that this function is called by sn_task() i.e. from the higher levels of the stack, and
// is also called during transmit_packet().
void snd_stack_task(void)
{
	if(reentrancy_count > 3)
	{
		zprintf(CRITICAL_IMPORTANCE,"wpan_task() reentered %c times", reentrancy_count + 0x30);
	}

	//if(irq_accepted < irq_receive)
	if(irq_receive)
	{
		//irq_update_flag = false;
		//irq_accepted++;
		irq_receive = 0;

		trx_irq_handler();
	}

	reentrancy_count++;
	wpan_task();
	reentrancy_count--;
	
	// handle timeout of pairing mode
	if(pairing_mode_timeout.active)
	{
		if((get_microseconds_tick() - pairing_mode_timeout.timestamp) > pairing_mode_timeout.timeout)
		{
			pairing_mode_timeout.active = false;	
			PAIRING_REQUEST request	= { 0, false, PAIRING_REQUEST_SOURCE_TIMEOUT };
			// turn off pairing mode
			snd_pairing_mode(request);
			zprintf(LOW_IMPORTANCE,"Pairing mode has timed out\r\n");
			// Note that this instructs the stack to change pairing mode. It will ripple
			// down through the stack, and back up again, and eventually reach surenet_pairing_mode_change_cb()
			// which will call the application to turn off the flashing LEDs
		}
	}	
}
//------------------------------------------------------------------------------
// brief ISR for transceiver's main interrupt
// overrides weakly linked handler in stm32h7xx_it.c
void RF_IRQ_HANDLER(void)
{
	extern bool tal_awake_end_flag;

    tal_awake_end_flag = true;

    //irq_receive++;

    irq_receive = 1;
}
//------------------------------------------------------------------------------
// Initialises the RF Stack
void snd_stack_init(void)
{
	// initialise timers in the PAL
    sw_timer_init();

    // initialise MAC and children.
    if (MAC_SUCCESS != wpan_init())
    {
        zprintf(HIGH_IMPORTANCE,"RF Stack initialisation FAIL\r\n");

        // suicide
        vTaskDelete(NULL);
    }

    // This starts a chain of calls and callbacks to configure the stack.
    wpan_mlme_reset_req(true);
}
/**************************************************************
 * Function Name   : snd_set_channel
 * Description     : Sets RF channel
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
void snd_set_channel(uint8_t ucChannel)
{
    wpan_mlme_set_req(phyCurrentChannel,&ucChannel);
   	wpan_mlme_set_req(phyTransmitPower,&TX_Power_Per_Channel[ucChannel]);		
    current_channel = ucChannel;
    // Note we will get a callback to usr_mlme_set_conf() when this change has taken effect
}

/**************************************************************
 * Function Name   : snd_get_channel()
 * Description     : Returns local copy as by design this is the master copy. We could
 *                 : call usr_mlme_get_req() and wait for the usr_mlme_get_conf()
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
uint8_t snd_get_channel(void)
{
    return current_channel;
}

/**************************************************************
 * Function Name   : snd_pairing_mode
 * Description     : This is the entry point for all calls to change pairing mode
 *                 : ALL request for changes to pairing mode come through here.
 *                 : The source of the request is specified in the function parameter.
 *                 : We record the source of the request so we can pass it back out
 *                 : to the application.
 *                 : Furthermore, when an association request comes in, we can
 *                 : utilise the recorded source of the original pairing mode request
 *                 : so we can alert the user accordingly.
 *                 : We set a timeout here which is checked in snd_stack_task() to fall out
 *                 : of pairing mode eventually.
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
void snd_pairing_mode(PAIRING_REQUEST pairing)
{
	// here we handle the dropping of requests from the Server to go into pairing mode
	// if we have recently completed a successful pairing.
	// Note that a record of the most recent result is still in requested_pairing_mode

	if(pairing.enable && !pairing_mode_timeout.active)
	{
		// start timeout if the request is to activate pairing mode, and we're not already
		// in a timeout period, i.e. don't retrigger it.
		pairing_mode_timeout.timestamp = get_microseconds_tick();
		// this timeout is monitored in Surenet_Interface_Handler()
		pairing_mode_timeout.active = true;
		if(pairing.source == PAIRING_REQUEST_SOURCE_BEACON_REQUEST)
		{
			pairing_mode_timeout.timeout = PAIRING_MODE_TIME_BEACON_REQUEST;
		}
		else
		{
			pairing_mode_timeout.timeout = PAIRING_MODE_TIME;
		}
	}	
	
	// request from the server
	// the request is to go into pairing mode
	// && the last request was less than 10 seconds ago
	// && the last request was to end pairing mode (i.e. successful pairing probably)
	if((PAIRING_REQUEST_SOURCE_SERVER == pairing.source)
	&& pairing.enable
	&& ((get_microseconds_tick() - requested_pairing_mode.timestamp) < PAIRING_SERVER_REQUEST_LOCKOUT_TIME)
	&& (!requested_pairing_mode.enable))
	{
		// zprintf(CRITICAL_IMPORTANCE,"Dropping too-soon pairing mode request from the Server\r\n");
		// drop the request as it may be a delayed message from the server
		return;
	}
	// SureNetDriver record of most recent request
    requested_pairing_mode = pairing;
	requested_pairing_mode.timestamp = get_microseconds_tick();
    wpan_mlme_set_req(macAssociationPermit, &pairing.enable);
}

/**************************************************************
 * Function Name   : snd_transmit_packet()
 * Description     :
 * Inputs          :
 * Outputs         :
 * Returns         : true if the packet could be queued for transmission, false otherwise
 **************************************************************/
bool snd_transmit_packet(TX_BUFFER *pcTxBuffer)
{
    wpan_addr_spec_t xDestinationAddress;
    uint8_t tx_options;

    xDestinationAddress.PANId = pan_id;
    xDestinationAddress.AddrMode = WPAN_ADDRMODE_LONG;
    xDestinationAddress.Addr.long_address=pcTxBuffer->uiDestAddr;

    if (pcTxBuffer->xRequestAck==true)
    {
        tx_options = WPAN_TXOPT_ACK;
    }
    else
    {
        tx_options = WPAN_TXOPT_OFF;
    }

    return wpan_mcps_data_req(WPAN_ADDRMODE_LONG,
    		&xDestinationAddress,
			pcTxBuffer->ucBufferLength,
			pcTxBuffer->pucTxBuffer,
			0xcc,
			tx_options);
}
//------------------------------------------------------------------------------
// What follows are callbacks etc. from the MAC layer
// These handle the various protocol sequences via the method of:
// Callbacks come out of the stack, which trigger new calls to the stack.
// Repeated many times.
// Called back when a transmit message has gone
void usr_mcps_data_conf(uint8_t msduHandle, uint8_t status)
{
	if(msduHandle != 0xCC)
    {
        zprintf(MEDIUM_IMPORTANCE, "WEIRD - RECEIVED CALLBACK FOR UNKNOWN HANDLE\r\n");
    }
    // status should be MAC_SUCCESS
    if (MAC_SUCCESS != status)
    {
        zprintf(LOW_IMPORTANCE, "Something went wrong transmitting a message, error 0x%02x\r\n", status);
    }
    else
    {
        sn_mark_transmission_complete();
    }
}
//------------------------------------------------------------------------------
// this is a callback from wpan_mlme_reset_req
void usr_mlme_reset_conf(uint8_t status)
{
	if(status == MAC_SUCCESS)
	{
		wpan_mlme_get_req(phyCurrentPage);
	}
	else
	{
		/* something went wrong; restart */
		wpan_mlme_reset_req(true);
	}
}
//------------------------------------------------------------------------------
// This is a callback from wpan_mlme_get_req()
void usr_mlme_get_conf(uint8_t status, uint8_t PIBAttribute, void *PIBAttributeValue)
{
	if((status == MAC_SUCCESS) && (PIBAttribute == phyCurrentPage))
    {
		current_channel_page = *(uint8_t *)PIBAttributeValue;
		// will cause a callback back to this function.
		wpan_mlme_get_req(phyChannelsSupported);
	}
	else if((status == MAC_SUCCESS) && (PIBAttribute == phyChannelsSupported))
    {
        uint8_t short_addr[2];
        short_addr[0] = (uint8_t)COORD_SHORT_ADDR; /* low byte */
        short_addr[1] = (uint8_t)(COORD_SHORT_ADDR >> 8); /*high byte */
        wpan_mlme_set_req(macShortAddress, short_addr);
	}
	else if((status == MAC_SUCCESS) && (PIBAttribute == phyCurrentChannel))
    {
		// response from call to get current channel. So put it in a mailbox and set event group
        current_channel = *(uint8_t *)PIBAttributeValue;
	}
	else
    {
		/* Something went wrong; restart */
        zprintf(HIGH_IMPORTANCE, "Unexpected attribute get callback - restarting stack\r\n");
		wpan_mlme_reset_req(true);
	}
}
/**************************************************************
 * Function Name   : set_beacon_payload
 * Description     : Sets the beacon payload.
 * Inputs          : Payload, and update flag.
 *                 : update == true is for when just the payload needs changing
 *                 : update == false is for when the payload is being initialised
 *                 : as part of the initialisation sequence.
 *                 : This matters because setting the payload triggers a callback
 *                 : from the stack, which triggers the next step in the
 *                 : initialisation sequence. We need to suppress triggering the
 *                 : next step if we are just updating the payload.
 * Outputs         :
 * Returns         :
 **************************************************************/
void set_beacon_payload(void *payload, bool update)
{
	// will call back to usr_mlme_set_conf()
	wpan_mlme_set_req(macBeaconPayload,payload);
	beacon_payload_update = update;
}
//------------------------------------------------------------------------------
// callback from wpan_mlme_set_req()
void usr_mlme_set_conf(uint8_t status, uint8_t PIBAttribute)
{
	if((status == MAC_SUCCESS) && (PIBAttribute == macShortAddress))
    {
		// Set length of Beacon Payload  - have to do this before setting beacon payload
		uint8_t beacon_payload_len = sizeof(Beacon_Payload);
		// will call back to usr_mlme_set_conf()
		wpan_mlme_set_req(macBeaconPayloadLength, &beacon_payload_len);
	}
	else if((status == MAC_SUCCESS) && (PIBAttribute == macBeaconPayloadLength))
    {
		// Set Beacon Payload
		set_beacon_payload(Beacon_Payload, false);
	}
	else if((status == MAC_SUCCESS) && (PIBAttribute == macBeaconPayload))
    {
		// Set RX on when idle to enable the receiver as default.
		// Use: bool wpan_mlme_set_req(uint8_t PIBAttribute, void *PIBAttributeValue);
		if(beacon_payload_update == false)
		{
			bool rx_on_when_idle = true;

			// will call back to usr_mlme_set_conf()
			wpan_mlme_set_req(macRxOnWhenIdle, &rx_on_when_idle);
		}
	}
	else if((status == MAC_SUCCESS) && (PIBAttribute == macRxOnWhenIdle))
    {

		// Start a nonbeacon-enabled network
		// Use: bool wpan_mlme_start_req(uint16_t PANId,
		//                               uint8_t LogicalChannel,
		//                               uint8_t ChannelPage,
		//                               uint8_t BeaconOrder,
		//                               uint8_t SuperframeOrder,
		//                               bool PANCoordinator,
		//                               bool BatteryLifeExtension,
		//                               bool CoordRealignment)
		//
		// This request leads to a start confirm message ->
		// usr_mlme_start_conf

		// In theory we don't need to set the channel here as we already set it in the TAL initialisation
		// where it is set via TAL_CURRENT_CHANNEL_DEFAULT. But we do it here to ensure consistency
		// between SureNetDriver and the MAC. The 'master' value is now current_channel which is
		// in SureNetDriver, and this is manipulated via snd_set_channel() and snd_get_channel()
		//wpan_mlme_start_req(pan_id, current_channel, current_channel_page, 15, 15, true, false, false);
		wpan_mlme_start_req(pan_id, current_channel, current_channel_page, 15, 15, true, false, false);
    	wpan_mlme_set_req(phyTransmitPower,&TX_Power_Per_Channel[current_channel]);			
	}
	else if((status == MAC_SUCCESS) && (PIBAttribute == macAssociationPermit))
    {
		// this callback occurs whenever AssociationPermit is changed via a call to wpan_mlme_set_req()
        sn_association_changed(requested_pairing_mode);
    }
	else if((status == MAC_SUCCESS) && (PIBAttribute == phyCurrentChannel))
    {
        // do nothing
    }
	else if((status == MAC_SUCCESS) && (PIBAttribute == phyTransmitPower))
    {
        // do nothing
    }
	else
    {
		/* something went wrong; restart */
        zprintf(HIGH_IMPORTANCE, "Unexpected attribute change %d, restarting stack\r\n",PIBAttribute);
		wpan_mlme_reset_req(true);
	}
}
//------------------------------------------------------------------------------
// called as a callback from wpan_mlme_start_req(pan_id,current_channel,current_channel_page,15, 15,true, false, false);
void usr_mlme_start_conf(uint8_t status)
{
    if (status != MAC_SUCCESS)
    {
        zprintf(CRITICAL_IMPORTANCE,"RF Stack initialisation FAIL\r\n");
    }
}
//------------------------------------------------------------------------------
bool usr_mac_process_associate_request(uint64_t mac_address)
{
	// && are_we_paired_with_source(mac_address);
	return snd_have_we_seen_beacon(mac_address);
}
//------------------------------------------------------------------------------
// called when the MAC receives an association request
// Note that devices that have already been paired to us in the past will continue to send us
// Association Requests even if they are not in pairing mode. So we need to reject them here
// if they are not in our pairing table.
void usr_mlme_associate_ind(uint64_t DeviceAddress,
							uint8_t CapabilityInformation,
							uint8_t dev_type,
							uint8_t dev_rssi)
{
    // store the characteristics of the device attempting to associate, so if the pairing
    // is successful, we can add the info to the pairing table
    assoc_info.association_addr = DeviceAddress;
    assoc_info.association_dev_type = dev_type;
    assoc_info.association_dev_rssi = dev_rssi;

    zprintf(LOW_IMPORTANCE,
    		"ASSOCIATION REQUEST from %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X with capability 0x%02X type %d rssi 0x%02X\r\n",
			(uint8_t)(DeviceAddress>>56),
			(uint8_t)(DeviceAddress>>48),
			(uint8_t)(DeviceAddress>>40),
			(uint8_t)(DeviceAddress>>32),
			(uint8_t)(DeviceAddress>>24),
			(uint8_t)(DeviceAddress>>16),
			(uint8_t)(DeviceAddress>>8),
			(uint8_t)DeviceAddress,
			CapabilityInformation,
			dev_type,
			dev_rssi);
	/*
	 * Any device is allowed to join the network
	 * Use: bool wpan_mlme_associate_resp(uint64_t DeviceAddress,
	 *                                    uint16_t AssocShortAddress,
	 *                                    uint8_t status);
	 *
	 * This response leads to comm status indication ->
	 * usr_mlme_comm_status_ind
	 * Get the next available short address for this device
	 */
	uint16_t associate_short_addr = macShortAddress_def;
//    if (is_mac_in_pairing_table(mac_address)==true)

	if (assign_new_short_addr(DeviceAddress, &associate_short_addr))
    {
		wpan_mlme_associate_resp(DeviceAddress, associate_short_addr, ASSOCIATION_SUCCESSFUL);
	}
    else
    {
        zprintf(HIGH_IMPORTANCE, "PAN FULL\r\n");
        // PAN_ACCESS_DENIED
		wpan_mlme_associate_resp(DeviceAddress, associate_short_addr, PAN_AT_CAPACITY);
	}

}

//------------------------------------------------------------------------------
void usr_mlme_associate_conf(uint16_t AssocShortAddress, uint8_t status)
{

}

//------------------------------------------------------------------------------
void usr_mlme_beacon_notify_ind(uint8_t BSN,
		wpan_pandescriptor_t *PANDescriptor,
		uint8_t PendAddrSpec,
		uint8_t *AddrList,
		uint8_t sduLength,
		uint8_t *sdu)
{

}

//------------------------------------------------------------------------------
// Callback function usr_mlme_disassociate_conf
// @param status             Result of requested disassociate operation.
// @param DeviceAddrSpec     Pointer to wpan_addr_spec_t structure for device
//                           that has either requested disassociation or been
// @return void
void usr_mlme_disassociate_conf(uint8_t status, wpan_addr_spec_t *DeviceAddrSpec)
{

}

//------------------------------------------------------------------------------
void usr_mlme_disassociate_ind(uint64_t DeviceAddress, uint8_t DisassociateReason)
{

}

//------------------------------------------------------------------------------
void usr_mlme_sync_loss_ind(uint8_t LossReason,
		uint16_t PANId,
		uint8_t LogicalChannel,
		uint8_t ChannelPage)
{

}

//------------------------------------------------------------------------------
void usr_mlme_scan_conf(uint8_t status,
		uint8_t ScanType,
		uint8_t ChannelPage,
		uint32_t UnscannedChannels,
		uint8_t ResultListSize,
		void *ResultList)
{

}

//------------------------------------------------------------------------------
void usr_mlme_orphan_ind(uint64_t OrphanAddress)
{

}

//------------------------------------------------------------------------------
void usr_mlme_poll_conf(uint8_t status)
{

}
//------------------------------------------------------------------------------
// This gets called when a data message is received over the RF Interface.
// Note that the buffer is freed immediately after this function returns, so
// we have to copy the data out.
// putting this variable outside the function means the debugger can see it out of context.
void usr_mcps_data_ind(wpan_addr_spec_t *SrcAddrSpec,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t mpduLinkQuality,
		uint8_t DSN)
{

    // Copy data into rx_buffer
    rx_buffer.uiSrcAddr = SrcAddrSpec->Addr.long_address;
    rx_buffer.uiDstAddr = DstAddrSpec->Addr.long_address;
    rx_buffer.ucBufferLength = msduLength;
    rx_buffer.ucRSSI = mpduLinkQuality;

    // copy data out of stack buffer into ours
    if(msduLength < sizeof(rx_buffer.ucRxBuffer))
    {
        memcpy(rx_buffer.ucRxBuffer, msdu, msduLength);
        sn_process_received_packet(&rx_buffer);
    }
    else
    {
        zprintf(HIGH_IMPORTANCE, "Received message too large...\r\n");
    }

    // Note that calling function frees buffer so we don't have to worry about it
}
//------------------------------------------------------------------------------
// Probably pointless, but part of the reference code.
// Assigns a 16bit address for every new 64bit address. But we don't
// really care about those, so just have to go through the motion.
static bool assign_new_short_addr(uint64_t addr64, uint16_t *addr16)
{
	uint8_t i;

	/* Check if device has been associated before */
	for (i = 0; i < MAX_NUMBER_OF_DEVICES; i++)
	{
		if (device_list[i].short_addr == 0x0000)
		{
			// If the short address is 0x0000, it has not been used
			// before
			continue;
		}

		if (device_list[i].ieee_addr == addr64)
		{
			// Assign the previously assigned short address again
			*addr16 = device_list[i].short_addr;
			return true;
		}
	}

	for (i = 0; i < MAX_NUMBER_OF_DEVICES; i++)
	{
		if (device_list[i].short_addr == 0x0000)
		{
			*addr16 = CPU_ENDIAN_TO_LE16(i + 0x0001);
			//get next short address
			device_list[i].short_addr = CPU_ENDIAN_TO_LE16(i + 0x0001);
			 // store extended address
			device_list[i].ieee_addr = addr64;

			return true;
		}
	}

	// If we are here, no short address could be assigned.
	return false;
}
//------------------------------------------------------------------------------
//Note this gets called for a variety of reasons, indicated by STATUS.
//Status is probably set to one of retval_t
void usr_mlme_comm_status_ind(wpan_addr_spec_t *SrcAddrSpec,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t status)
{
	if(status == MAC_SUCCESS)
	{
		/*
		 * Now the association of the device has been successful and its
		 * information, like address, could  be stored.
		 * But for the sake of simple handling it has been done
		 * during assignment of the short address within the function
		 * assign_new_short_addr()
		 */
		PAIRING_REQUEST mode = { 0, false, PAIRING_REQUEST_SOURCE_UNKNOWN };

		// remember who put us in pairing mode in the first place
		mode.source = requested_pairing_mode.source;

		assoc_info.source = requested_pairing_mode.source;
		// zprintf(CRITICAL_IMPORTANCE,"Association success, from pairing mode source %d\r\n",mode.source);
		// exit pairing mode
        snd_pairing_mode(mode);

        if(DstAddrSpec->Addr.long_address == assoc_info.association_addr)
        {
        	// this should always be true
        	// call back to say association successful
            sn_device_pairing_success(&assoc_info);
        }
        else
        {
        	// This is probably because an already paired device has re-associated, so we don't
			// have any record in assoc_info.association_Addr of this association negotiation.
        }
    }
	else if (status == MAC_TRANSACTION_EXPIRED)
    {
        zprintf(HIGH_IMPORTANCE,"usr_mlme_comm_status_ind() called with status = MAC_TRANSACTION_EXPIRED (0xf0)\r\n");
    }
	else
    {
        zprintf(HIGH_IMPORTANCE,"usr_mlme_comm_status_ind() called with status=0x%02X\r\n",status);
    }
	/* Keep compiler happy. */
	SrcAddrSpec = SrcAddrSpec;
	DstAddrSpec = DstAddrSpec;
}
/**************************************************************
 * Function Name   : snd_have_we_seen_beacon
 * Description     : Checks to see if the supplied mac address is the same as that seen on the most recent beacon request
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
bool snd_have_we_seen_beacon(uint64_t mac_address)
{
    if (beacon_request_device_data.mac_addr == mac_address)
	{
        return true;
	}
    return false;
}
//------------------------------------------------------------------------------
// This is called from mac_beacon.c when a BEACON_REQUEST has arrived and has been parsed.
// It gives us an opportunity to do three things:
// 1. Record whether the beacon request was from a Thalamus device or pre-Thalamus device.
// 2. Record the channel (although surely we know that anyway?!)
// 3. Set whether we are going to allow Association from this device.
// We also decide here if we are going to allow a BEACON to be sent out in response.
// We return true to indicate that a beacon should be sent.
// We send a BEACON if:
// - We are in pairing mode
// - The MAC of device sending the BEACON_REQUEST is in our device table
// Note that if the MAC of the source device is in our device table, then
// we set the stack into pairing mode to allow the rest of the association
// process to complete.
bool set_beacon_request_data(uint64_t mac_address, uint8_t src_address_mode, uint8_t data)
{
	 // No source address, so reject.
	if(FCF_NO_ADDR == src_address_mode)
	{
		return false;
	}

    PAIRING_REQUEST mode = sn_get_hub_pairing_mode();
	bool send_beacon = mode.enable;

    zprintf(LOW_IMPORTANCE,"BEACON REQUEST from %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
    		(uint8_t)(mac_address>>56),
			(uint8_t)(mac_address>>48),
			(uint8_t)(mac_address>>40),
			(uint8_t)(mac_address>>32),
			(uint8_t)(mac_address>>24),
			(uint8_t)(mac_address>>16),
			(uint8_t)(mac_address>>8),
			(uint8_t)mac_address);

	if(send_beacon)
	{
		zprintf(LOW_IMPORTANCE,"Already in pairing mode\r\n");
	}
	else
	{
		zprintf(LOW_IMPORTANCE,"Not in pairing mode\r\n");
	}

    beacon_request_device_data.device_channel = data & 0x1f;

    // store this for use in deciding how to respond to an ASSOCIATION_REQUEST
    beacon_request_device_data.mac_addr = mac_address;
    if ((data & 0x20) == 0)
	{
        beacon_request_device_data.device_platform = NON_THALAMUS_BASED_DEVICE;
		Beacon_Payload[BEACON_PAYLOAD_SUREFLAP_VERSION] = HUB_DOES_NOT_SUPPORT_THALAMUS;
		set_beacon_payload (Beacon_Payload, true);
	}
    else
	{
        beacon_request_device_data.device_platform = THALAMUS_BASED_DEVICE;
		Beacon_Payload[BEACON_PAYLOAD_SUREFLAP_VERSION] = HUB_SUPPORTS_THALAMUS;
		set_beacon_payload (Beacon_Payload, true);
	}

    if(are_we_paired_with_source(mac_address))
    {
    	// We are already paired with this device, so set ASSOCIATION_PERMIT
        // Note it might already be set if we are in pairing mode.
        send_beacon = true;
		PAIRING_REQUEST mode = { 0, true, PAIRING_REQUEST_SOURCE_BEACON_REQUEST };
		zprintf(LOW_IMPORTANCE,"BEACON_REQUEST triggering snd_pairing_mode(PAIRING_REQUEST_SOURCE_BEACON_REQUEST)\r\n");
	    snd_pairing_mode(mode);
    }

    // if true, caller should send a beacon. If false, it shouldn't as we're not in pairing mode
    return send_beacon;
}
//------------------------------------------------------------------------------
bool usr_mac_process_beacon_request(uint64_t mac_address, uint8_t src_address_mode, uint8_t data)
{
	return set_beacon_request_data(mac_address, src_address_mode, data);
}
//==============================================================================
/* EOF */
