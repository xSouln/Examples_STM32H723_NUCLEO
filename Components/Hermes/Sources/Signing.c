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
* Filename: Signing.c   
* Author:   Chris Cowdery 02/06/2021
* Purpose:  Signing of communications between Server and Hub
*             
**************************************************************************/
#include "hermes.h"
#include <stdarg.h>	
#include <stdio.h>	
#include "FreeRTOS.h"
#include "Signing.h"
#include "Server_Buffer.h"
#include "message_parser.h"
#include "InternalFlashManager.h"
#include "wolfssl/wolfcrypt/pwdbased.h"
#include "wolfssl/wolfcrypt/sha256.h"
#include "wolfssl/wolfcrypt/hmac.h"
#include "wolfssl/wolfcrypt/coding.h"
#include "credentials.h"
#include "rng.h"
//==============================================================================
//externs:

// serial numbers, MAC addresses, etc.
extern PRODUCT_CONFIGURATION product_configuration;
extern SUREFLAP_CREDENTIALS aws_credentials;
//==============================================================================
//variables:

// This is a random number that is generated by the Hub and shared with the Server
uint8_t SharedSecret[SHARED_SECRET_LENGTH];
uint8_t	PendingSharedSecret[SHARED_SECRET_LENGTH];
uint8_t	DerivedKey[DERIVED_KEY_LENGTH];
char DerivedKey_text[DERIVED_KEY_LENGTH * 2 + 1];
uint8_t PendingDerivedKey[DERIVED_KEY_LENGTH];
char PendingDerivedKey_text[DERIVED_KEY_LENGTH * 2 + 1];

bool flag = false;
//==============================================================================
//functions:

/**************************************************************
 * Function Name   : CalculateSignature()
 * Description     : Calculates the digital signature of a data block
 *                 : using the HMAC SHA256 algorithm and a previously
 *                 : calculated Derived Key.
 * Inputs          : Which key, (DERIVED_KEY_CURRENT, DERIVED_KEY_PENDING or DERIVED_KEY_FLASH)
 *                 : Pointer to 65 byte array to put the signature in (in text form)
 *                 : Pointer to buffer containing the data block, and its length
 * Returns         : True if success, false if an error occurred.
 **************************************************************/
bool CalculateSignature(uint8_t *sig, DERIVED_KEY_SOURCE src, uint8_t *buf, uint32_t buflen)
{
	Hmac		hmac;
	uint8_t 	*pKey = NULL;
	uint32_t	i;
	uint8_t		sigbin[SIGNATURE_LENGTH];
	
	switch(src)
	{	
		case DERIVED_KEY_CURRENT:
			pKey = DerivedKey; // live RAM key
			break;

		case DERIVED_KEY_PENDING:
			pKey = PendingDerivedKey; // Pending RAM key
			break;

		case DERIVED_KEY_FLASH:
			pKey = product_configuration.DerivedKey; // key from Flash
			break;
		default:
			break;
	}

	if(NULL == pKey)
	{
		if(DERIVED_KEY_NONE == src)
		{
			// this is OK, no signature required
			return true;
		}
		zprintf(CRITICAL_IMPORTANCE,"Unknown key source\r\n");
		return false;
	}
	
	if(wc_HmacSetKey(&hmac, WC_SHA256, pKey, DERIVED_KEY_LENGTH) != 0)
	{
		zprintf(CRITICAL_IMPORTANCE,"Failed to initialise HMAC\r\n");
		return false;
	}			
	
	if(wc_HmacUpdate(&hmac, buf, buflen) != 0)
	{
		zprintf(CRITICAL_IMPORTANCE,"Failed to update HMAC\r\n");
		return false;
	}			

	if(wc_HmacFinal(&hmac, sigbin) != 0)
	{
		zprintf(CRITICAL_IMPORTANCE,"Failed to compute signature\r\n");
		return false;
	}			
	
	// Now need to convert to ascii representation
	for( i=0; i< SIGNATURE_LENGTH; i++)
	{
		sig += sprintf((char *)sig,"%02x",sigbin[i]);
	}
	*sig = '\0'; // terminate it.

	// Dump everything about this signature
	if(flag)
	{
		zprintf(CRITICAL_IMPORTANCE,"Key                       : ");
		for(i = 0; i < DERIVED_KEY_LENGTH; i++)
		{
			zprintf(CRITICAL_IMPORTANCE,"%02X",pKey[i]);
			HermesConsoleFlush();
		}			
		zprintf(CRITICAL_IMPORTANCE,"\r\nPassword                  : %s%s0000%02X%02X%02X%02X%02X%02X\r\n",
				product_configuration.secret_serial, product_configuration.serial_number, 
				product_configuration.ethernet_mac[0], product_configuration.ethernet_mac[1],
				product_configuration.ethernet_mac[2], product_configuration.ethernet_mac[3],
				product_configuration.ethernet_mac[4], product_configuration.ethernet_mac[5]);	
		HermesConsoleFlush();

		zprintf(CRITICAL_IMPORTANCE,"Shared Secret             : ");
		for(i = 0; i < SHARED_SECRET_LENGTH; i++)
		{
			zprintf(CRITICAL_IMPORTANCE,"%02X",SharedSecret[i]);
			HermesConsoleFlush();
		}
		
		zprintf(CRITICAL_IMPORTANCE,"Signature :");
		for(i = 0; i < SIGNATURE_LENGTH; i++)
		{
			zprintf(CRITICAL_IMPORTANCE,"%02x",sigbin[i]);
			HermesConsoleFlush();
		}	
		zprintf(CRITICAL_IMPORTANCE,"\r\n");
	}

	return true;	
}
/**************************************************************
 * Function Name   : GenerateDerivedKey()
 * Description     : Generates a new Derived Key using PBKDF2.
 *                 : Inputs to PBKDF2 are:
 *                 : - Algorithm: HMAC SHA256
 *                 : - Concatenation of Private Serial, Public Serial, Ethernet MAC
 *                 : - The Shared Secret
 *                 : - Iterations 
 *                 : - Derived Key Length = 256 bits / 32 bytes.
 * Returns         : Pointer to the new Derived Key
 **************************************************************/
uint8_t *GenerateDerivedKey(DERIVED_KEY_SOURCE dest)
{
	int32_t 	ret;
	uint8_t 	*password;	//[32 + 12 + 16 + 1];
	char 		*pptr;
	uint8_t 	password_size;
	uint8_t 	i;
	uint8_t		*key;
	uint8_t		*secret;
	char 		*text;

	switch (dest)
	{
		case DERIVED_KEY_CURRENT: // use the current Derived Key
			key = DerivedKey;
			secret = SharedSecret;
			text = DerivedKey_text;
			break;

		case DERIVED_KEY_PENDING: // Use the pending Derived Key
			key = PendingDerivedKey;
			secret = PendingSharedSecret;
			text = PendingDerivedKey_text;
			break;

		default:
			zprintf(CRITICAL_IMPORTANCE,"Error - trying to make a key for unknown destination\r\n");
			return NULL;	// bad news
	}
		
		
	password_size = sizeof(product_configuration.secret_serial)-1 + \
					sizeof(product_configuration.serial_number)-1 + \
					((sizeof(product_configuration.ethernet_mac)+2)*2) + 1; // should be 61

	password = pvPortMalloc(password_size);	
	
	if( NULL == password)
	{
		zprintf(CRITICAL_IMPORTANCE,"Unable to generate new Derived Key - no memory\r\n");
		return DerivedKey;	// old one
	}
	
	pptr = (char *)password;
	pptr += sprintf(pptr,"%s",product_configuration.secret_serial);
	pptr += sprintf(pptr,"%s",product_configuration.serial_number);
	sprintf(pptr,"0000%02X%02X%02X%02X%02X%02X",product_configuration.ethernet_mac[0],
												product_configuration.ethernet_mac[1],
												product_configuration.ethernet_mac[2],
												product_configuration.ethernet_mac[3],
												product_configuration.ethernet_mac[4],
												product_configuration.ethernet_mac[5]);
		
//	zprintf(CRITICAL_IMPORTANCE,"Password = %s\r\n",password);
	//ret = wc_PKCS12_PBKDF(key,
	ret = wc_PBKDF2(key,
			password,
			password_size-1,
			secret,
			SHARED_SECRET_LENGTH,
			PBKDF2_ITERATIONS,
			DERIVED_KEY_LENGTH,
			WC_SHA256);
	
	vPortFree(password);
	
	if(ret)
	{
		zprintf(CRITICAL_IMPORTANCE, "Failed to generated Derived Key, error code %d\r\n",ret);
	}

	for (i = 0; i < DERIVED_KEY_LENGTH; i++)
	{
		text += sprintf(text,"%02x",key[i]);
	}
	
	return key;
}
//------------------------------------------------------------------------------
// return a pointer to the derived key
uint8_t *GetDerivedKey(void)
{
	return DerivedKey;
}
//------------------------------------------------------------------------------
// return a pointer to the text (lower case) form of the Derived Key.
char *GetDerivedKey_text(void)
{
	return (char *)DerivedKey_text;
}
/**************************************************************
 * Function Name   : StoreDerivedKey
 * Description     : Stores the DerivedKey in Flash as part of
 *                 : the product_configuration struct
 **************************************************************/
void StoreDerivedKey(void)
{
	uint8_t i;
	
	for(i = 0; i < DERIVED_KEY_LENGTH; i++)
	{
		product_configuration.DerivedKey[i] = DerivedKey[i];
	}
	
	//write_product_configuration();
}

/**************************************************************
 * Function Name   : checkFlashDerivedKey
 * Description     : Check the Derived Key in product_configuration
 *                 : which is a RAM copy of Flash and see if it has
 *                 : ever been written to, i.e. is not all 0xFF's
 * Returns         : true if a valid key is present.
 **************************************************************/
bool checkFlashDerivedKey(void)
{
	uint8_t i;
	bool 	retval = false;
	
	for(i = 0; i < DERIVED_KEY_LENGTH; i++)
	{
		if(product_configuration.DerivedKey[i] != 0xff)
		{
			retval = true;
		}
	}
	return retval;	
}

static void GenerateRandomNumber(uint8_t* out, uint16_t size)
{
	uint32_t value;

	while (size)
	{
		value = hermes_rand();

		uint8_t byte_number = 0;
		while(size && byte_number < sizeof(value))
		{
			*out = value & 0xff;

			value >>= 8;

			byte_number++;
			out++;
			size--;
		}
	}
}
/**************************************************************
 * Function Name   : GenerateSharedSecret()
 * Description     : Generates a truly random 128 bit Shared Secret
 *                 : This is shared with the Server to allow both ends
 *                 : to generate the same Derived Key for checking
 *                 : authenticity.
 * Returns         : Pointer to the new Shared Secret
 **************************************************************/
uint8_t *GenerateSharedSecret(SHARED_SECRET_SOURCE dest)
{
	uint8_t *buf;

	switch(dest)
	{
		case SHARED_SECRET_PENDING:
			buf = PendingSharedSecret;
			break;
		default:
			buf = SharedSecret;
			break;
	}

	GenerateRandomNumber(buf, SHARED_SECRET_LENGTH);

	return buf;
}
//------------------------------------------------------------------------------
// return a pointer to the shared secret
uint8_t *GetSharedSecret(void)
{
	return SharedSecret;
}
//------------------------------------------------------------------------------
uint8_t *GetPendingSharedSecret(void)
{
	return PendingSharedSecret;
}

/**************************************************************
 * Function Name   : key_copy_pending_to_current
 * Description     : Copy the pending shared secret and key into both
 *                 : the current and flash copies.
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
void key_copy_pending_to_current(void)
{
	memcpy(SharedSecret, PendingSharedSecret, SHARED_SECRET_LENGTH);
	memcpy(DerivedKey, PendingDerivedKey, DERIVED_KEY_LENGTH);
	StoreDerivedKey();	// store current key in Flash
}

/**************************************************************
 * Function Name   : send_shared_secret
 * Description     : Sends the Shared Secret to the Server, via MQTT messages
 *                 : It generates a new shared secret and derived key.
 *                 : It generates a suitable MQTT message and sticks it into
 *                 : the outgoing buffer.
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
void send_shared_secret(void)
{
	SERVER_MESSAGE 	message;
	uint8_t 		msg[(SHARED_SECRET_LENGTH*3)+3];
	uint8_t			i;
	uint8_t			*pmsg;
	
	message.source_mac = 0;	// it's from the hub
	message.message_ptr = msg;
	
	GenerateSharedSecret(SHARED_SECRET_PENDING); // Make a new shared secret to send to the Server
	GenerateDerivedKey(DERIVED_KEY_PENDING);	 // make a new key to check the signature of the reply

	pmsg = msg;
	pmsg += sprintf((char *)msg,"%d",MSG_CHANGE_SIGNING_KEY);
	
	for( i=0; i<SHARED_SECRET_LENGTH; i++)
	{
		pmsg += sprintf((char *)pmsg," %02x",PendingSharedSecret[i]);
	}
	
	if( false == server_buffer_add(&message) )
	{
		zprintf(LOW_IMPORTANCE,"Failed to add MSG_CHANGE_SIGNING_KEY to server buffer");
	}	
}

/**************************************************************
 * Function Name   : check_recd_mqtt_signature()
 * Description     : Calculates the signature for an incoming MQTT
 *                 : message and compares it with the received signature.
 *                 : Also checks using the pending key in case the key
 *                 : is in the process of being changed.
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
bool check_recd_mqtt_signature( char *message,char *subtopic)
{
	char				recd_signature[SIGNATURE_LENGTH_ASCII+1];
	char				signature_check_message[BASE_TOPIC_MAX_SIZE + MAX_INCOMING_MQTT_MESSAGE_SIZE_SMALL + SIGNATURE_LENGTH_ASCII + 1];
	char				*message_text;
	char				signature[SIGNATURE_LENGTH_ASCII+1];
	
	// Messages from the Server are now signed. This means that the message part
	// contains a 64 digit signature and a single space character at the start.	
	
	if( strlen(message)<SIGNATURE_LENGTH_ASCII)
	{
		return false;	// message too short
	}
	
	memcpy(recd_signature,message,SIGNATURE_LENGTH_ASCII);	// grab the signature
	recd_signature[SIGNATURE_LENGTH_ASCII]='\0';
	
	memmove(message,&message[SIGNATURE_LENGTH_ASCII+1],strlen(message)-SIGNATURE_LENGTH_ASCII); // move message down to start of array, overwriting the signature

	// Messages for a device should have a subtopic /messages/MAC but the /messages part has been removed by the AWS
	// receiver. So we need to add it back in for this calculation
	if( 0 == strncmp(subtopic,"messages",8) )
	{
		message_text = "\0";
	}
	else
	{
		message_text = "/messages";
	}
	
	snprintf(signature_check_message, (BASE_TOPIC_MAX_SIZE + MAX_INCOMING_MQTT_MESSAGE_SIZE_SMALL + SIGNATURE_LENGTH_ASCII + 1),
				 				"%s%s/%s%s", aws_credentials.base_topic, message_text,
												subtopic, message);
	CalculateSignature((uint8_t *)signature, DERIVED_KEY_CURRENT, (uint8_t *)signature_check_message, strlen(signature_check_message));
	//zprintf(LOW_IMPORTANCE,"Sig check : %s\r\nRecd sig : %s\r\nCalc sig : %s\r\n",signature_check_message,recd_signature,signature);		
	
	if( 0 != strcmp(recd_signature, signature))
	{	// the message could be confirmation of a key change, so speculatively check against the pending key
		CalculateSignature((uint8_t *)signature, DERIVED_KEY_PENDING, (uint8_t *)signature_check_message, strlen(signature_check_message));
		if( 0 != strcmp(recd_signature, signature))
		{		
			zprintf(MEDIUM_IMPORTANCE,"Rejecting received MQTT message - signature does not match : %s\r\n",signature_check_message);
			return false;
		}
	}	
	
	return true;
}
//------------------------------------------------------------------------------
void key_dump(void)
{
#ifdef KEY_DUMP
#warning "key_dump() should be removed for final build"	
	uint8_t 	base64buf[128];
	uint32_t 	base64len;
	int 		i;
	
	zprintf(CRITICAL_IMPORTANCE,"Shared Secret             : ");
	for( i=0; i<SHARED_SECRET_LENGTH; i++)
	{
		zprintf(CRITICAL_IMPORTANCE,"%02X",SharedSecret[i]);
	}
	zprintf(CRITICAL_IMPORTANCE,"\r\n");	
	DbgConsole_Flush();	
	zprintf(CRITICAL_IMPORTANCE,"Pending Shared Secret     : ");
	for( i=0; i<SHARED_SECRET_LENGTH; i++)
	{
		zprintf(CRITICAL_IMPORTANCE,"%02X",PendingSharedSecret[i]);
	}
	zprintf(CRITICAL_IMPORTANCE,"\r\n");
	
	zprintf(CRITICAL_IMPORTANCE,"Password                  : %s%s0000%02X%02X%02X%02X%02X%02X\r\n",
			product_configuration.secret_serial, product_configuration.serial_number, 
			product_configuration.ethernet_mac[0], product_configuration.ethernet_mac[1],
			product_configuration.ethernet_mac[2], product_configuration.ethernet_mac[3],
			product_configuration.ethernet_mac[4], product_configuration.ethernet_mac[5]);
	
	DbgConsole_Flush();
	zprintf(CRITICAL_IMPORTANCE,"Live Derived Key          : %s\r\n",DerivedKey_text);
	
	base64len = sizeof(base64buf);
	Base64_Encode(DerivedKey, DERIVED_KEY_LENGTH, base64buf, &base64len);	// it adds \r\n to the end
	base64buf[base64len]='\0';
	zprintf(CRITICAL_IMPORTANCE,"Live Derived Key (base64) : %s",base64buf);	
	DbgConsole_Flush();
	
	zprintf(CRITICAL_IMPORTANCE,"Flash Derived Key         : ");
	for (i=0; i<DERIVED_KEY_LENGTH; i++)
	{
		zprintf(CRITICAL_IMPORTANCE,"%02x",product_configuration.DerivedKey[i]);
	}		
	zprintf(CRITICAL_IMPORTANCE,"\r\n");
	base64len = sizeof(base64buf);
	Base64_Encode(product_configuration.DerivedKey, DERIVED_KEY_LENGTH, base64buf, &base64len);
	base64buf[base64len]='\0';
	zprintf(CRITICAL_IMPORTANCE,"Flash Derived Key (base64): %s\r\n",base64buf);
	DbgConsole_Flush();	

	zprintf(CRITICAL_IMPORTANCE,"Pending Derived Key       : %s\r\n",PendingDerivedKey_text);
	
	base64len = sizeof(base64buf);
	Base64_Encode(PendingDerivedKey, DERIVED_KEY_LENGTH, base64buf, &base64len);	// it adds \r\n to the end
	base64buf[base64len]='\0';
	zprintf(CRITICAL_IMPORTANCE,"Pend. Derived Key (base64): %s\r\n",base64buf);	
	DbgConsole_Flush();	
	
	//flag = true;
#else
	zprintf(CRITICAL_IMPORTANCE,"Key dump not supported in this build\r\n");
#endif
}
//==============================================================================
