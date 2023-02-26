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
* Filename: DeviceFirmwareUpdate.h    
* Author:   Chris Cowdery 26/02/2020
* Purpose:  Handles Device Firmware Update
*
*****************************************************************************/
#ifndef __DEVICEFIRMWAREUPDATE__
#define __DEVICEFIRMWAREUPDATE__
//==============================================================================
//includes:
#include "Hermes-compiller.h"

#include "devices.h"
//==============================================================================
//defines:

#define FIRST_SEGMENT_SIZE				72 // has 8 byte header + 64bytes of data
#define SECOND_SEGMENT_SIZE				64 // just data
#define CHUNK_SIZE						(FIRST_SEGMENT_SIZE + SECOND_SEGMENT_SIZE)

#define DEVICE_FIRMWARE_CACHE_ENTRIES	4
#define DEVICE_MAX_SIMULTANEOUS_FIRMWARE_UPDATES	(DEVICE_FIRMWARE_CACHE_ENTRIES)
//==============================================================================
//types:

// used to queue up firmware chunk requests from devices
typedef HERMES__PACKED_PREFIX struct
{
	// we grab these parameters when the request comes in just in case
	uint64_t device_mac;

	// the Device Table gets changed whilst we are mid-process and
	T_DEVICE_TYPE device_type;

	// find we cannot look them up any longer!
	uint16_t chunk_address;

	uint16_t attempts;
	bool in_use;

} HERMES__PACKED_POSTFIX DEVICE_RCVD_SEGS_PARAMETERS_QUEUE;
//==============================================================================
//functions:

void DFU_init(void);
void DFU_Handler(void);
//==============================================================================
#endif //__DEVICEFIRMWAREUPDATE__


