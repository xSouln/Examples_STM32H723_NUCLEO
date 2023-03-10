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
* Filename: CLI-Commands-Simulator.h    
* Author:   Neil Barnes
* Purpose:
**************************************************************************/
#ifndef __CLI_COMMANDS_SIMULATOR_H_
#define __CLI_COMMANDS_SIMULATOR_H_
//==============================================================================
//functions:

uint64_t read_mac_address(char* tmac);
__weak void vRegisterSimulatorCLICommands(void);
//==============================================================================
#endif //__CLI_COMMANDS_SIMULATOR_H_
