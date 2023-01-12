//==============================================================================
#ifndef SERIAL_PORT_UART_COMPONENT_CONFIG_H
#define SERIAL_PORT_UART_COMPONENT_CONFIG_H
//==============================================================================
//module enable:

#include "SerialPort/SerialPort_ComponentConfig.h"
#ifdef SERIAL_PORT_UART_COMPONENT_ENABLE
//==============================================================================
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Components_Types.h"
//==============================================================================
//import:

extern DMA_HandleTypeDef hdma_usart3_rx;
//==============================================================================
//defines:

#define SERIAL_PORT_UART_RX_CIRCLE_BUF_SIZE_MASK 0x1ff
#define SERIAL_PORT_UART_RX_OBJECT_BUF_SIZE 0xfff
#define SERIAL_PORT_UART_TX_CIRCLE_BUF_SIZE_MASK 0x1ff

#define SERIAL_PORT_UART_REG USART3
#define SERIAL_PORT_UART_RX_DMA hdma_usart3_rx
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //SERIAL_PORT_UART_COMPONENT_CONFIG_H
#endif //SERIAL_PORT_UART_COMPONENT_ENABLE
