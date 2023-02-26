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
* Filename: Signing.h   
* Author:   Chris Cowdery 02/06/2021
* Purpose:  Signing of communications between Server and Hub
*             
**************************************************************************/
#ifndef __SIGNING_H__
#define __SIGNING_H__
//==============================================================================
//includes:

#include "Hermes-compiller.h"
//==============================================================================
//defines:

#define SHARED_SECRET_LENGTH	16
#define DERIVED_KEY_LENGTH		32 // 256bits
#define	SIGNATURE_LENGTH		32 // 256bits, because of SHA256
#define SIGNATURE_LENGTH_ASCII	64 // signature in text form
#define PBKDF2_ITERATIONS		1000	

// Maximum amount of time between now and a server message
// being sent before it's rejected (stops replay attacks)
#define	MAX_TIME_DISCREPANCY_MS	300000
//==============================================================================
//types:

typedef enum
{
	// Don't use a key
	DERIVED_KEY_NONE,

	// use the current Derived Key
	DERIVED_KEY_CURRENT,

	// Use the pending Derived Key
	DERIVED_KEY_PENDING,

	// Use the Derived Key stored in Flash (which is the key in use before a power cycle)
	DERIVED_KEY_FLASH,

} DERIVED_KEY_SOURCE;
//------------------------------------------------------------------------------
typedef enum
{
	// not used
	SHARED_SECRET_NONE,

	// use the current shared secret
	SHARED_SECRET_CURRENT,

	// use the pending shared secret
	SHARED_SECRET_PENDING,

} SHARED_SECRET_SOURCE;
//==============================================================================
//functions:

// Generates a new Shared Secret. Returns a pointer to it
uint8_t *GenerateSharedSecret(SHARED_SECRET_SOURCE src);

// Generates a new Derived Key. Returns a pointer to it.
uint8_t *GenerateDerivedKey(DERIVED_KEY_SOURCE src);

// returns a pointer to the Shared Secret
uint8_t *GetSharedSecret(void);

// returns a pointer to the Pending Shared Secret
uint8_t *GetPendingSharedSecret(void);

// returns a pointer to the Derived Key
uint8_t *GetDerivedKey(void);

// returns a pointer to a hex string form of the Derived Key
char *GetDerivedKey_text(void);

// Store DerivedKey in Flash (in product_configuration)
void StoreDerivedKey(void);

// see if the DerivedKey stored in Flash is present
bool checkFlashDerivedKey(void);

// Calculate digital signature
bool CalculateSignature(uint8_t *sig, DERIVED_KEY_SOURCE src, uint8_t *buf, uint32_t buflen);

// Dump the keys to the terminal
void key_dump(void);

// Copy the pending secret and derived key to current and Flash
void key_copy_pending_to_current(void);

// Share the shared secret with the Server
void send_shared_secret(void);

// Check an incoming MQTT message is correctly signed
bool check_recd_mqtt_signature( char *message,char *subtopic);
//==============================================================================
#endif //__SIGNING_H__
