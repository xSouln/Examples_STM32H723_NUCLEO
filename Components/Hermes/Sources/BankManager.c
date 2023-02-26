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
* Filename: Babel.c    
* Author:   Tom Monkhouse
* Purpose:     

**************************************************************************/
 /*
 * Bank Manager (Griphook) arranges the separate firmware banks, their encryption,
 * switching, and confirmation.
 * Tom Monkhouse
 * 09/06/2020
 *
 */
//==============================================================================
///includes:

#include "BankManager.h"
#include "hermes.h"
#include "FreeRTOS.h"
//==============================================================================
//variables:

const volatile BANK_DESCRIPTOR*	CurrentDescriptor;
//==============================================================================
//functions:

BM_BANK BM_GetCurrentBank(void)
{
	return BM_BANK_UNKONWN;
}
//------------------------------------------------------------------------------
BM_BANK BM_ReportBank(void)
{

	return BM_BANK_UNKONWN;
}
//------------------------------------------------------------------------------
bool BM_EnscribeData(uint8_t* data, uint32_t bank_address, uint32_t size)
{
	
	return true;
}
//------------------------------------------------------------------------------
bool BM_TranscribeBank(void)
{
	return true;
}
//------------------------------------------------------------------------------
bool BM_Init(void)
{

	return true;
}
//------------------------------------------------------------------------------
bool BM_Encrypt(uint8_t* destination, uint8_t* source, uint32_t size)
{

	return true;
}
//------------------------------------------------------------------------------
//!!!
// This sets the _other_ bank to be active.
// Note it also zeroes the watchdog reset counter.
void BM_ConfirmBank(bool force_encrypted)
{
	/*
	BANK_DESCRIPTOR	new_descriptor;
	memcpy(&new_descriptor, CurrentDescriptor->opposite, sizeof(BANK_DESCRIPTOR));
	
	if( true == force_encrypted ){ new_descriptor.encrypted = true; }
	new_descriptor.bank_mark = (BANK_MARK)((uint32_t)CurrentDescriptor->bank_mark + 1);
	if( 0 >= new_descriptor.bank_mark ){ new_descriptor.bank_mark = (BANK_MARK)1; }
	new_descriptor.watchdog_resets = 0;	// clear watchdog reset counter
	
	new_descriptor.descriptor_hash = 0;
	for( uint32_t i = 4; i < sizeof(BANK_DESCRIPTOR) / sizeof(uint32_t); i++ )
	{
		new_descriptor.descriptor_hash ^= *(((uint32_t*)&new_descriptor) + i);
	}
	
	hermesFlashRequestErase(CurrentDescriptor->opposite, sizeof(BANK_DESCRIPTOR), true);
	hermesFlashRequestWrite((uint8_t*)&new_descriptor, CurrentDescriptor->opposite, sizeof(BANK_DESCRIPTOR), true);
    DCACHE_InvalidateByRange((uint32_t )CurrentDescriptor->opposite, sizeof(BANK_DESCRIPTOR));	
    */
}

/**************************************************************
 * Function Name   : BM_SetBankMark
 * Description     : Sets the bank mark to a value specified as a parameter
 *                 : Intended to set the bank mark to BANK_MARK_UPDATING or BANK_MARK_ROLLED_BACK
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
//!!!
void BM_SetBankMark(BANK_MARK new_mark)
{
	/*
	BANK_DESCRIPTOR	new_descriptor;
	memcpy(&new_descriptor, CurrentDescriptor->opposite, sizeof(BANK_DESCRIPTOR));
	
	new_descriptor.bank_mark = new_mark;
	
	new_descriptor.descriptor_hash = 0;
	for( uint32_t i = 4; i < sizeof(BANK_DESCRIPTOR) / sizeof(uint32_t); i++ )
	{
		new_descriptor.descriptor_hash ^= *(((uint32_t*)&new_descriptor) + i);
	}
	
	hermesFlashRequestErase(CurrentDescriptor->opposite, sizeof(BANK_DESCRIPTOR), true);
	hermesFlashRequestWrite((uint8_t*)&new_descriptor, CurrentDescriptor->opposite, sizeof(BANK_DESCRIPTOR), true);
    DCACHE_InvalidateByRange((uint32_t )CurrentDescriptor->opposite, sizeof(BANK_DESCRIPTOR));
    */
}

//------------------------------------------------------------------------------
// Switches banks!
void BM_BankSwitch(bool bank_b, bool encrypted)
{

}
//------------------------------------------------------------------------------
// This function is used just after reset to establish which bank is to be run.
// It then selects the appropriate bank.
void BM_DetermineBankToBoot(void)
{

}

// Work out from how the BEE is set up which bank we are currently in,
// and set up CurrentDescriptor accordingly.
void BM_ResolveBankUse(void)
{


}
//==============================================================================
