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
* Filename: Device_Buffer.h    
* Author:   Chris Cowdery 9/10/2019
* Purpose:  Handles messages for Devices
*             
*             
*
**************************************************************************/
#ifndef __DEVICE_BUFFER_H__
#define __DEVICE_BUFFER_H__
//==============================================================================
//includes:

#include "Hermes-compiller.h"
#include "SureNet-Interface.h"
//==============================================================================
//functions:

// initialise our buffer
void device_buffer_init(void);

// store a message in the buffer
void device_buffer_add(uint64_t mac, T_MESSAGE *message);

// get a pointer to the oldest message for this device
T_MESSAGE *device_buffer_get_next(uint64_t mac_address, uint32_t *index);

// clear an entry from the buffer
void device_buffer_clear_entry(uint32_t index);

// dump the buffer to a terminal
void device_buffer_dump(void);

bool device_message_is_new(uint64_t mac, T_MESSAGE *message);
//==============================================================================
#endif //__DEVICE_BUFFER_H__
