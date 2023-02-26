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
* Filename: hermes.c   
* Author:   Chris Cowdery
* Purpose:   
*   
* Main start point for Hermes firmware (after low level startup)
*            
**************************************************************************/
// Project wide includes that can affect anything!
#include "hermes.h"

// used by zprintf
#include <stdarg.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"

#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "timers.h"
#include "hermes-time.h"
#include "cmsis_os.h"
/* FreeRTOS-TCP includes */

// Other includes
#include "SureNet-interface.h"
#include "hermes-app.h"
#include "InternalFlashManager.h"
#include "../MQTT/MQTT.h"
#include "SNTP.h"
#include "leds.h"
#include "Hermes-test.h"
#include "resetHandler.h"
#include "HTTP_Helper.h"

// just needed for initialisation
#include "myMAC.h"
#include "Hermes-app.h"

// for INITAL_CHANNEL
#include "SureNet.h"

#include "HubFirmwareUpdate.h"
#include "Signing.h"
#include "Babel.h"
#include "LabelPrinter.h"
#include "netif.h"

#include "NetworkInterface.h"
#include "Hermes/Console/Hermes-console.h"

#include "wolfssl/wolfcrypt/asn_public.h"
//==============================================================================
//defines:

#define ZBUFFER_SIZE 384
//==============================================================================
//types:

//==============================================================================
//externs:

extern QueueHandle_t xNvStoreMailboxSend;
extern QueueHandle_t xNvStoreMailboxResp;
//==============================================================================
static uint8_t initial_RF_channel;
static const char SERIAL_NUMBER[] = "H201-0859935";

BaseType_t xTCstatus;
//------------------------------------------------------------------------------
//task handles:

TaskHandle_t xHermesTestTaskHandle;
TaskHandle_t xFM_hermesFlashTaskHandle;
TaskHandle_t xStartupTaskHandle;
TaskHandle_t xHTTPPostTaskHandle;
TaskHandle_t xHFUTaskHandle;
TaskHandle_t xLedTaskTaskHandle;
TaskHandle_t xSNTPTaskHandle;
TaskHandle_t xHermesAppTaskHandle;
TaskHandle_t xMQTTTaskHandle;
TaskHandle_t xWatchdogTaskHandle;
TaskHandle_t xLabelPrinterTaskHandle;
//------------------------------------------------------------------------------
//static tasks attributes:

StaticTask_t hermes_app_task_buffer HERMES_APP_TASK_STACK_MEM_SECTION;
StackType_t hermes_app_task_stack[HERMES_APP_TASK_STACK_SIZE] HERMES_APP_TASK_STACK_MEM_SECTION;

StaticTask_t hermes_mqtt_task_buffer MQTT_TASK_STACK_MEM_SECTION;
StackType_t hermes_mqtt_task_stack[MQTT_TASK_STACK_SIZE] MQTT_TASK_STACK_MEM_SECTION;

StaticTask_t hermes_sntp_task_buffer SNTP_TASK_STACK_MEM_SECTION;
StackType_t hermes_sntp_task_stack[SNTP_TASK_STACK_SIZE] SNTP_TASK_STACK_MEM_SECTION;

StaticTask_t hermes_led_task_buffer LED_TASK_STACK_MEM_SECTION;
StackType_t hermes_led_task_stack[LED_TASK_STACK_SIZE] LED_TASK_STACK_MEM_SECTION;

StaticTask_t hermes_http_post_task_buffer HTTP_POST_TASK_STACK_MEM_SECTION;
StackType_t hermes_http_post_task_stack[HTTP_POST_TASK_STACK_SIZE] HTTP_POST_TASK_STACK_MEM_SECTION;

StaticTask_t hermes_watch_dog_task_buffer WATCHDOG_TASK_STACK_MEM_SECTION;
StackType_t hermes_watch_dog_task_stack[WATCHDOG_TASK_STACK_SIZE] WATCHDOG_TASK_STACK_MEM_SECTION;

StaticTask_t hermes_test_task_buffer TEST_TASK_STACK_MEM_SECTION;
StackType_t hermes_test_task_stack[TEST_TASK_STACK_SIZE] TEST_TASK_STACK_MEM_SECTION;

StaticTask_t hermes_label_printer_task_buffer LABEL_PRINTER_TASK_STACK_MEM_SECTION;
StackType_t hermes_label_printer_task_stack[LABEL_PRINTER_TASK_STACK_SIZE] LABEL_PRINTER_TASK_STACK_MEM_SECTION;
//------------------------------------------------------------------------------

//Global variable with some useful product information within. Initialised from Flash on boot.
bool shouldIPackageFlag = false;
bool shouldICryptFlag = false;
bool amILocked = true;

// turned on by messagedump CLI command
bool global_message_trace_flag = false;

// This is a RAM copy of the product info from Flash.
PRODUCT_CONFIGURATION product_configuration;

uint8_t printLevel = LOW_IMPORTANCE;
bool immediate_firmware_update = false;

// global because a pointer to this variable is used by the RF Stack
uint64_t rfmac = 0x982782e01fdc;

bool rf_channel_override = false;

SemaphoreHandle_t RandMutex = 0;

DebugCounterT DebugCounter;
//==============================================================================
//prototypes:

void init_watchdog();

/* The CLI commands are defined in CLI-commands.c. */
void vRegisterBasicCLICommands(void);
void vRegisterCLICommands(void);

void write_product_configuration(void);
static void watchdog_task(void *pvParameters);
static void service_watchdog(void);

void Core_detect(void);
//==============================================================================
//functions:

int HermesStart_HFU_Task()
{
	return 0;
}
//------------------------------------------------------------------------------

int HermesStart_TestTask()
{
	return 0;
}
//------------------------------------------------------------------------------

int HermesStart_SNTP_Task()
{
	xSNTPTaskHandle =
			xTaskCreateStatic(SNTP_Task, // Function that implements the task.
								"SNTP Task", // Text name for the task.
								SNTP_TASK_STACK_SIZE, // Number of indexes in the xStack array.
								NULL, // Parameter passed into the task.
								SNTP_TASK_PRIORITY, // Priority at which the task is created.
								hermes_sntp_task_stack, // Array to use as the task's stack.
								&hermes_sntp_task_buffer);

	return xHTTPPostTaskHandle == NULL ? 0 : -1;
}
//------------------------------------------------------------------------------

int HermesStart_LabelPrinterTask()
{
	xLabelPrinterTaskHandle =
			xTaskCreateStatic(label_task, // Function that implements the task.
								"label printer task", // Text name for the task.
								LABEL_PRINTER_TASK_STACK_SIZE, // Number of indexes in the xStack array.
								NULL, // Parameter passed into the task.
								LABEL_PRINTER_TASK_PRIORITY, // Priority at which the task is created.
								hermes_label_printer_task_stack, // Array to use as the task's stack.
								&hermes_label_printer_task_buffer);

	return xLedTaskTaskHandle == NULL ? 0 : -1;
}
//------------------------------------------------------------------------------

int HermesStart_LedTask()
{
	xLedTaskTaskHandle =
			xTaskCreateStatic(led_task, // Function that implements the task.
								"led_task", // Text name for the task.
								LED_TASK_STACK_SIZE, // Number of indexes in the xStack array.
								NULL, // Parameter passed into the task.
								LED_TASK_PRIORITY, // Priority at which the task is created.
								hermes_led_task_stack, // Array to use as the task's stack.
								&hermes_led_task_buffer);

	return xLedTaskTaskHandle == NULL ? 0 : -1;
}
//------------------------------------------------------------------------------

int HermesStart_AppTask()
{
	xHermesAppTaskHandle =
			xTaskCreateStatic(hermes_app_task, // Function that implements the task.
								"Hermes Application", // Text name for the task.
								HERMES_APP_TASK_STACK_SIZE, // Number of indexes in the xStack array.
								NULL, // Parameter passed into the task.
								HERMES_APP_TASK_PRIORITY, // Priority at which the task is created.
								hermes_app_task_stack, // Array to use as the task's stack.
								&hermes_app_task_buffer);

	return xHermesAppTaskHandle == NULL ? 0 : -1;
}
//------------------------------------------------------------------------------

int HermesStart_HTTP_PostTask()
{
	xHTTPPostTaskHandle =
			xTaskCreateStatic(HTTPPostTask, // Function that implements the task.
								"HTTP Post", // Text name for the task.
								HTTP_POST_TASK_STACK_SIZE, // Number of indexes in the xStack array.
								NULL, // Parameter passed into the task.
								HTTP_POST_TASK_PRIORITY, // Priority at which the task is created.
								hermes_http_post_task_stack, // Array to use as the task's stack.
								&hermes_http_post_task_buffer);

	return xHTTPPostTaskHandle == NULL ? 0 : -1;
}
//------------------------------------------------------------------------------

int HermesStart_MQTT_Task()
{
	xMQTTTaskHandle =
			xTaskCreateStatic(MQTT_Task, // Function that implements the task.
								"MQTT", // Text name for the task.
								MQTT_TASK_STACK_SIZE, // Number of indexes in the xStack array.
								NULL, // Parameter passed into the task.
								MQTT_TASK_PRIORITY, // Priority at which the task is created.
								hermes_mqtt_task_stack, // Array to use as the task's stack.
								&hermes_mqtt_task_buffer);

	return xMQTTTaskHandle == NULL ? 0 : -1;
}
//------------------------------------------------------------------------------

int HermesStart_WatchdogTask()
{
	xWatchdogTaskHandle =
			xTaskCreateStatic(watchdog_task, // Function that implements the task.
								"watchdog_task", // Text name for the task.
								WATCHDOG_TASK_STACK_SIZE, // Number of indexes in the xStack array.
								NULL, // Parameter passed into the task.
								WATCHDOG_TASK_PRIORITY, // Priority at which the task is created.
								hermes_watch_dog_task_stack, // Array to use as the task's stack.
								&hermes_watch_dog_task_buffer);

	return xWatchdogTaskHandle == NULL ? 0 : -1;
}
//------------------------------------------------------------------------------

void HermesInit_ProductNotConfiguredMode()
{
	// should never get here!!

	shouldICryptFlag = false;
	shouldIPackageFlag = false;
	printLevel = LOW_IMPORTANCE;

	zprintf(MEDIUM_IMPORTANCE,"\r\n\r\nSure Petcare Hub - 'Hermes'\r\n---------------------------\r\n");
	zprintf(MEDIUM_IMPORTANCE,"PRODUCT NOT CONFIGURED\r\n");
}
//------------------------------------------------------------------------------

void HermesInit_ProductBlankMode()
{
	// This is the state after a reset after firmware programming. It needs to support 'test'

	shouldICryptFlag = false;
	shouldIPackageFlag = false;

	// Limit the CLI messages when communicating with Programmer
	printLevel = MEDIUM_IMPORTANCE;

	HermesStart_LedTask();
	HermesStart_TestTask();
}
//------------------------------------------------------------------------------

void HermesInit_ProductTestedMode()
{
	// Now I want to talk to the Label Printer

	shouldICryptFlag = false;
	shouldIPackageFlag = false;

	printLevel = LOW_IMPORTANCE;

	zprintf(MEDIUM_IMPORTANCE,"\r\n\r\nSure Petcare Hub - 'Hermes'\r\n---------------------------\r\n");
	zprintf(MEDIUM_IMPORTANCE,"PRODUCT_TESTED - expecting Label Printer\r\n");

	HermesStart_LedTask();
	HermesStart_LabelPrinterTask();
}
//------------------------------------------------------------------------------

void HermesInit_ProductConfiguredMode()
{
	// everything set up, now I am ready for a customer

	shouldIPackageFlag = SHOULD_I_PACKAGE;

	//true for AES Encryption
	shouldICryptFlag = false;

	#ifdef DEBUG
	printLevel = LOW_IMPORTANCE;
	#else
	printLevel = CRITICAL_IMPORTANCE;
	#endif

	zprintf(CRITICAL_IMPORTANCE,"\r\n\r\nSure Petcare Hub - 'Hermes'\r\n---------------------------\r\n");

	#ifdef DEBUG
	zprintf(CRITICAL_IMPORTANCE,"DEBUG build - NOT FOR RELEASE!\r\n");
	#endif

	// normal operation
	DisplayResetStatus();
	Core_detect();

	// everything set up, now I am ready for a customer
/*
	if(immediate_firmware_update)
	{
		return;
	}
*/

	surenet_init(&rfmac, product_configuration.rf_pan_id, initial_RF_channel);

	HermesStart_SNTP_Task();

	HermesStart_LedTask();
	HermesStart_AppTask();

	HermesStart_HTTP_PostTask();
	HermesStart_MQTT_Task();

	// from nvm
	set_led_brightness(get_led_brightness(), false);

	update_led_view();
}
//------------------------------------------------------------------------------

void HermesInit_FirmwareUpdatedMode()
{
	// So we don't need to do a lot here
	LED_Request_Pattern(LED_MODE_NORMAL, COLOUR_RED, LED_PATTERN_SOLID, 0);

	// Now we need to wait for DHCP, then trigger a SNTP fetch, and finally
	// kick off a Hub Firmware Update
	extern EventGroupHandle_t xConnectionStatus_EventGroup;

	xEventGroupWaitBits(xConnectionStatus_EventGroup, CONN_STATUS_NETWORK_UP, false, false, portMAX_DELAY);

	while(!SNTP_AwaitUpdate(true, portMAX_DELAY))
	{

	}

	HFU_trigger(true);
}
//------------------------------------------------------------------------------

void HermesInitSelectedMode()
{
	HermesInit_ProductConfiguredMode();

	return;

	switch (product_configuration.product_state)
	{
		case PRODUCT_NOT_CONFIGURED:
			HermesInit_ProductNotConfiguredMode();
			break;

		case PRODUCT_BLANK:
			HermesInit_ProductBlankMode();
			break;

		case PRODUCT_TESTED:
			HermesInit_ProductTestedMode();
			break;

		case PRODUCT_CONFIGURED:
			HermesInit_ProductConfiguredMode();
			break;

		default:
			// Not sure what to do here - something's gone wrong with Flash
			printLevel = LOW_IMPORTANCE;

			zprintf(MEDIUM_IMPORTANCE,"\r\n\r\nSure Petcare Hub - 'Hermes'\r\n---------------------------\r\n");
			zprintf(MEDIUM_IMPORTANCE,"CONFIGURATION CORRUPTED\r\n");

			break;
	}
}
//------------------------------------------------------------------------------

void HermesComponentInit()
{
	HermesConsoleInit();

	HermesFlashInit();
	HermesFlashReadData();
	HermesFlashReadProductConfig(&product_configuration);

	RandMutex = xSemaphoreCreateMutex();

	// Register the command line commands with the CLI
	vRegisterBasicCLICommands();

	osDelay(pdMS_TO_TICKS(100));

	MX_LWIP_Init();

	xNetworkInterfaceInitialise();

	wolfSSL_Init();
	wc_SetTimeCb(wc_time);

	if(product_configuration.rf_pan_id == 0xff
	|| product_configuration.rf_mac == 0xffffffffffffffff
	|| product_configuration.ethernet_mac[0] == 0xff)
	{
		product_configuration.rf_pan_id = SUREFLAP_PAN_ID;
		product_configuration.rf_mac = 0x982782e01fdc;

		product_configuration.ethernet_mac[0] = heth.Init.MACAddr[0];
		product_configuration.ethernet_mac[1] = heth.Init.MACAddr[1];
		product_configuration.ethernet_mac[2] = heth.Init.MACAddr[2];
		product_configuration.ethernet_mac[3] = heth.Init.MACAddr[3];
		product_configuration.ethernet_mac[4] = heth.Init.MACAddr[4];
		product_configuration.ethernet_mac[5] = heth.Init.MACAddr[5];

		memcpy(product_configuration.serial_number, SERIAL_NUMBER, sizeof(SERIAL_NUMBER));
		memcpy(product_configuration.secret_serial, mySecretSerial, sizeof(mySecretSerial));

		product_configuration.sanity_state = PRODUCT_CONFIGURED;
		product_configuration.product_state = PRODUCT_CONFIGURED;
		product_configuration.rf_mac_mangle = true;

		HermesFlashSetProductConfig(&product_configuration);
		HermesFlashSaveData();
	}

	// we mangle it by putting 0xfffe in the middle, and use the 6 byte Ethernet one around it
	if(product_configuration.rf_mac_mangle)
	{
		// This is because Pet Doors reject Hub MAC addresses that do not have 0xfffe in the middle
		((uint8_t *)&rfmac)[7] = product_configuration.ethernet_mac[0];
		((uint8_t *)&rfmac)[6] = product_configuration.ethernet_mac[1];
		((uint8_t *)&rfmac)[5] = product_configuration.ethernet_mac[2];
		((uint8_t *)&rfmac)[4] = 0xFF;
		((uint8_t *)&rfmac)[3] = 0xFE;
		((uint8_t *)&rfmac)[2] = product_configuration.ethernet_mac[3];
		((uint8_t *)&rfmac)[1] = product_configuration.ethernet_mac[4];
		((uint8_t *)&rfmac)[0] = product_configuration.ethernet_mac[5];
	}
	else
	{
		// the apparent swapping of endianness is because the RF mac is stored as a uint64_t
		// but the Ethernet mac is stored as a byte array.
		rfmac = product_configuration.rf_mac;
	}

	 // swap RF channels if button held down for >5 secs
	if(!rf_channel_override)
	{
		initial_RF_channel = INITIAL_CHANNEL;
	}
	else
	{
		initial_RF_channel = RF_CHANNEL1 ? RF_CHANNEL3 : RF_CHANNEL1;
	}

	initial_RF_channel = INITIAL_CHANNEL;

	// generate a truly random Shared Secret to be shared with the Server
	GenerateSharedSecret(SHARED_SECRET_CURRENT);

	// Used for signing data transfers between Hub and Server.
	GenerateDerivedKey(DERIVED_KEY_CURRENT);

	osDelay(pdMS_TO_TICKS(100));

	hermes_app_init();
	led_driver_init();
	SNTP_Init();
	HTTPPostTask_init();

	shouldIPackageFlag = SHOULD_I_PACKAGE;

	//true for AES Encryption
	shouldICryptFlag = false;

	osDelay(pdMS_TO_TICKS(100));

	//HermesStart_WatchdogTask();

	HermesInitSelectedMode();
}
//------------------------------------------------------------------------------
/**************************************************************
 * Function Name   : set_product_state
 * Description     : Sets product state and writes to flash. To be called
 *                 : by production tester and label printer
 * Inputs          : one of PRODUCT_BLANK, PRODUCT_TESTED or PRODUCT_CONFIGURED
 * Outputs         :
 * Returns         :
 **************************************************************/
void set_product_state(PRODUCT_STATE state)
{
	product_configuration.product_state = state;
	write_product_configuration();
}

/**************************************************************
 * Function Name   : Sanitise Product Configuration
 * Description     : Writes PRODUCT_NOT_CONFIGURED to sanity_state, which
 *                 : will cause the NVM to be refreshed with sane values
 *                 : on the next processor reset. Useful if the format of
 *                 : NVM has changed and it needs initialising
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
void sanitise_product_config(void)
{
	/*
	SEND_TO_FM_MSG	nvNvmMessage;    // Outgoing message	to Flash Manager
    uint32_t		notifyValue;

	nvNvmMessage.ptrToBuf	= (uint8_t *)&product_configuration;
	nvNvmMessage.type		= FM_PRODUCT_CONFIG;
	nvNvmMessage.dataLength	= sizeof(PRODUCT_CONFIGURATION);
	nvNvmMessage.action		= FM_GET;
	nvNvmMessage.xClientTaskHandle = xTaskGetCurrentTaskHandle();

    xQueueSend(xNvStoreMailboxSend, &nvNvmMessage, 0);	// get device table into RAM

    if( xTaskNotifyWait(0, 0, &notifyValue, portMAX_DELAY) == pdTRUE )
    {
		if( notifyValue == FM_ACK )
		{	// Got the values into product_configuration
			product_configuration.sanity_state = PRODUCT_NOT_CONFIGURED;
			product_configuration.product_state = PRODUCT_NOT_CONFIGURED;
			write_product_configuration();	// write 0xff into configuration
		}
	}
	*/
}

/**************************************************************
 * Function Name   : write_product_configuration
 * Description     : Store product_configuration in NV Store
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
void write_product_configuration(void)
{
	HermesFlashSetProductConfig(&product_configuration);
	HermesFlashSaveData();
}
//------------------------------------------------------------------------------
/* Initialise watchdog - only do once, at startup */
void init_watchdog(void)
{

}
//------------------------------------------------------------------------------
static void service_watchdog(void)
{

}
/**************************************************************
 * Function Name   : watchdog_task
 * Description     : Kicks the 'dog every 5 seconds
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
static void watchdog_task(void *pvParameters)
{ 	
	uint32_t				mqtt_reboot_timestamp;
	MQTT_CONNECTION_STATE	mqtt_connection_state;
	
	mqtt_reboot_timestamp = get_microseconds_tick();	
	
    while(1)
    {
		service_watchdog();
		mqtt_connection_state = get_mqtt_connection_state();

		// This is brutal. If we fall outside MQTT_STATE_CONNECTED for too
		// long, we just reboot the Hub !!
		if((mqtt_connection_state == MQTT_STATE_CONNECTED)
		|| (mqtt_connection_state == MQTT_STATE_STOP))
		{
			mqtt_reboot_timestamp = get_microseconds_tick();
		}
		else if((get_microseconds_tick() - mqtt_reboot_timestamp) > MQTT_REBOOT_TIMEOUT)
		{
			zprintf(CRITICAL_IMPORTANCE,"MQTT detected lack of traffic - restarting system...\r\n");

			// never returns
			NVIC_SystemReset();
		}

		// give up CPU for 5 seconds
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
//------------------------------------------------------------------------------
// Note this breaks Alohomora, so should only be used with a terminal
void zprintf(ZPRINTF_IMPORTANCE importanceLevel, const char *arg0, ...)
{
	static uint32_t zprintfCorrelationIDCounter = 0;
	uint32_t len;
    va_list args;
	char *zbuffer;
	HERMES_UART_HEADER *header;

	return;

	if(!strlen(arg0))
	{
		return;
	}

    if((importanceLevel <= MAX_PRINT_LEVEL) && (importanceLevel >= printLevel))
	{
		if(shouldIPackageFlag == true)
		{
    		va_start(args, arg0);

    		// find out how long the string would be, excluding terminating \0
			len = vsnprintf(NULL, 0, arg0, args) + 1;

			va_end(args);	
			#ifdef TIMESTAMP_ZPRINTF_OUTPUT
			// allow for extra HH:MM:SS that would be prepended.
			len += 9;
			#endif

			// string too long
			if(len > ZBUFFER_SIZE)
			{
				return;
			}

			header = pvPortMalloc(len + sizeof(HERMES_UART_HEADER));
	
			// drop output if cannot allocate buffer
			if(!header)
			{
				return;
			}

			zbuffer = (char*)header + sizeof(HERMES_UART_HEADER);
			
			len = 0;

			#ifdef TIMESTAMP_ZPRINTF_OUTPUT
			len += time_string(zbuffer);
			zbuffer += len;
			#endif
			
			va_start(args, arg0);
			len += vsprintf(zbuffer, arg0, args);
			va_end(args);	
			
			header->sync_word = 0x6863615A;
			if(importanceLevel == NOT_ENCRYPT_IMPORTANCE)
			{
				header->type = MESSAGE_GREETING;
			}
			else
			{
				header->type = MESSAGE_RESPONSE;
			}

			header->check = header->type ^ 0xFF;

			// this is probably broken
			if(shouldICryptFlag)
			{
				uint32_t TotalWidth = (((strlen(zbuffer) + 15)/16)*16);

				if(TotalWidth == 0)
				{
					header->correlation_ID++;
				}
				else
				{
					sprintf(zbuffer, "%-*s", TotalWidth, zbuffer);
				}
			}

			header->size = len;
			header->timestamp = get_microseconds_tick();
			header->correlation_ID = zprintfCorrelationIDCounter++;

			// single push of whole message
			HermesConsoleWrite(header, sizeof(HERMES_UART_HEADER) + len, 0);

			vPortFree(header);	
		}
		else
		{
			// Not packaging, so straight printout
			va_start(args, arg0);
			// find out how long the string would be, excluding terminating \0
			len = vsnprintf(NULL, 0, arg0, args) + 1;
			va_end(args);

			#ifdef TIMESTAMP_ZPRINTF_OUTPUT
			// allow for extra HH:MM:SS that would be prepended.
			len += 9;
			#endif

			if(len > ZBUFFER_SIZE)
			{
				return;
			}

			zbuffer = pvPortMalloc(len);

			if(!zbuffer)
			{
				return;
			}

			len = 0;
			#ifdef TIMESTAMP_ZPRINTF_OUTPUT
			len += time_string(zbuffer);
			#endif

			va_start(args, arg0);
			len += vsprintf(zbuffer+len, arg0, args);
			va_end(args);

			HermesConsoleWrite(zbuffer, len, 0);

			vPortFree(zbuffer);
		}
	}
}
/**************************************************************
 * Function Name   : Core_detect
 * Description     : Check revision of CPU core
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
void Core_detect(void)
{
	uint32_t cpuid = SCB->CPUID;
	uint32_t var, pat;

	zprintf(LOW_IMPORTANCE,"Detected Core: ");

	pat = (cpuid & 0x0000000F);
	var = (cpuid & 0x00F00000) >> 20;

	if ((cpuid & 0xFF000000) == 0x41000000) // ARM
	{
		switch((cpuid & 0x0000FFF0) >> 4)
		{
			case 0xC20 : zprintf(LOW_IMPORTANCE,"Cortex M0 r%dp%d\r\n", var, pat); break;
			case 0xC60 : zprintf(LOW_IMPORTANCE,"Cortex M0+ r%dp%d\r\n", var, pat); break;
			case 0xC21 : zprintf(LOW_IMPORTANCE,"Cortex M1 r%dp%d\r\n", var, pat); break;
			case 0xC23 : zprintf(LOW_IMPORTANCE,"Cortex M3 r%dp%d\r\n", var, pat); break;
			case 0xC24 : zprintf(LOW_IMPORTANCE,"Cortex M4 r%dp%d\r\n", var, pat); break;
			case 0xC27 : zprintf(LOW_IMPORTANCE,"Cortex M7 r%dp%d\r\n", var, pat); break;

			default : zprintf(LOW_IMPORTANCE,"Unknown CORE\r\n");
		}
	}
	else
	{
		zprintf(LOW_IMPORTANCE,"Unknown CORE IMPLEMENTER\r\n");
	}
}
/**************************************************************
 * Function Name   : initSureNetByProgrammer
 * Description     : Used by the Programmer to control SureNet initialisation.
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
void initSureNetByProgrammer(void)
{
	// This initialises stack variables and starts the surenet task.
    surenet_init(&product_configuration.rf_mac, product_configuration.rf_pan_id, initial_RF_channel);
}

/**************************************************************
 * Function Name   : connectToServer
 * Description     : Used by the test framework to register a
 *                 : blank mode Hub2 with SurePetCare server.
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
void connectToServer(void)
{
	/*
    HermesStart_SNTP_Task();
    HermesStart_AppTask();
    HermesStart_MQTT_Task();
    HermesStart_HTTP_PostTask();
    HermesStart_HFU_Task();
    */
}

/**************************************************************
 * Function Name   : hermes_rand()
 * Description     : Thread safe version of rand()
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
int hermes_rand(void)
{
	#include "rng.h"

	int value = -1;

	if (!RandMutex)
	{
		HAL_RNG_GenerateRandomNumber(&hrng, (uint32_t*)&value);
		return value;
	}

	xSemaphoreTake(RandMutex, portMAX_DELAY);

	HAL_RNG_GenerateRandomNumber(&hrng, (uint32_t*)&value);

	xSemaphoreGive(RandMutex);

	return value;
}

/**************************************************************
 * Function Name   : hermes_srand()
 * Description     : Thread safe version of srand()
 * Inputs          :
 * Outputs         :
 * Returns         :
 **************************************************************/
void hermes_srand(int seed)
{
	portENTER_CRITICAL();
	srand(seed);
	portEXIT_CRITICAL();
}
//==============================================================================
