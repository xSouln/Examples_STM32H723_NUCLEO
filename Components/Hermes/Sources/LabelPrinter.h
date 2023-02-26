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
* Filename: LabelPrinter.h   
* Author:   Chris Cowdery 15/06/2020
* Purpose:  Label Printer handler for Production Test   
*             
**************************************************************************/
#ifndef __LABEL_PRINTER__
#define __LABEL_PRINTER__
//==============================================================================
//includes:

#include "Hermes-compiller.h"
//==============================================================================
//functions:

void label_task(void *pvParameters);

// this is a bit naughty, but only needed for testing...
void restart_label_print(void);
//==============================================================================
#endif //__LABEL_PRINTER__
