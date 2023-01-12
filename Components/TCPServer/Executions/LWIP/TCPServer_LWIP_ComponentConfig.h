//==============================================================================
//module enable:

#include "TCPServer/TCPServer_ComponentConfig.h"
#ifdef TCP_SERVER_LWIP_COMPONENT_ENABLE
//==============================================================================
//header:

#ifndef _TCP_SERVER_LWIP_COMPONENT_CONFIG_H
#define _TCP_SERVER_LWIP_COMPONENT_CONFIG_H
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//==============================================================================
//includes:

#include "Components_Types.h"
//==============================================================================
//macros:


//==============================================================================
//import:


//==============================================================================
//defines:

#define TCP_SERVER_LWIP_RX_OBJECT_BUF_SIZE (0xfff + 1)
#define TCP_SERVER_LWIP_RX_CIRCLE_BUF_SIZE_MASK 0x7ff
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_TCP_SERVER_LWIP_COMPONENT_CONFIG_H
#endif //TCP_SERVER_LWIP_COMPONENT_ENABLE
