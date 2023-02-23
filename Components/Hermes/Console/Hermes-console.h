//==============================================================================
#ifndef _HERMES_CONSOL_H_
#define _HERMES_CONSOL_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//==============================================================================
//includes:

#include "Hermes-compiller.h"
//==============================================================================
//defines:


//==============================================================================
//types:

typedef enum
{
	ConsoleWriteCommonMode

} ConsoleWriteModes;
//==============================================================================
//functions:

void HermesConsoleInit();

int HermesConsoleWrite(void* in, uint16_t size, ConsoleWriteModes mode);
int HermesConsoleRead(void* out, uint16_t size, ConsoleWriteModes mode);
int HermesConsoleWriteString(char* in);

void HermesConsoleFlush();
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif
