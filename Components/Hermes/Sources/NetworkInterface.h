//==============================================================================
#ifndef _NETWORK_INTERFACE_H_
#define _NETWORK_INTERFACE_H_
//==============================================================================
//includes:

#include "Hermes-compiller.h"
#include "lwip.h"
//==============================================================================
//defines:


//==============================================================================
//types:


//==============================================================================
//functions:

int xNetworkInterfaceInitialise();

void NetworkInterface_GetAddressConfiguration(uint32_t* ip,
		uint32_t* mask,
		uint32_t* gateway,
		uint32_t* dns);

bool NetworkInterface_IsActive();
//==============================================================================
#endif //_NETWORK_INTERFACE_H_
