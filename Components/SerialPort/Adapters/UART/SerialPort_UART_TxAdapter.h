//==============================================================================
//header:

#ifndef SERIAL_PORT_UART_TX_ADAPTER_H
#define SERIAL_PORT_UART_TX_ADAPTER_H
//==============================================================================
//module enable:

#include "SerialPort/Adapters/SerialPort_AdapterConfig.h"
#ifdef SERIAL_PORT_UART_ADAPTER_ENABLE
//==============================================================================
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "SerialPort_UART_Adapter.h"
//==============================================================================
//functions:

xResult SerialPortUART_TxAdapterInit(SerialPortT* serial_port, SerialPortUART_AdapterT* adapter);
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //SERIAL_PORT_UART_TX_ADAPTER_H
#endif //SERIAL_PORT_UART_ADAPTER_ENABLE
