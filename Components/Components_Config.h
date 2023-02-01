//==============================================================================
#ifndef _COMPONENTS_CONFIG_H
#define _COMPONENTS_CONFIG_H
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

//==============================================================================
//components:

#define TERMINAL_COMPONENT_ENABLE 1
#define SERIAL_PORT_COMPONENT_ENABLE 1
//#define TCP_SERVER_COMPONENT_ENABLE 1
//#define ZIGBEE_COMPONENT_ENABLE 1
//#define SUREFLAP_COMPONENT_ENABLE 1
#define HERMES_COMPONENT_ENABLE 1
//==============================================================================
//defines:
/*
#elif defined(STM32H723xx)
	#define WOLFSSL_STM32H7
	#define HAL_CONSOLE_UART huart3
*/

/*
	ip_addr_t add1;
	osDelay(3000);

	add1.addr = PP_HTONL(LWIP_MAKEU32(213, 184, 225, 37));//213.184.225.37 //82, 209, 240, 241
	dns_setserver(0, &add1);

	add1.addr = PP_HTONL(LWIP_MAKEU32(213, 184, 224, 254));//213.184.224.254 //82, 209, 243, 241
	dns_setserver(1, &add1);

	// Modification start
	.user_ram_itcm :
	{
		*(.user_reg_2)
	} >ITCMRAM
	// Modification end
 */

//==============================================================================
//macros:

//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif

