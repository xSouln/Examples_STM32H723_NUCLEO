//==============================================================================
#ifndef _HERMES_COMPILLER_H
#define _HERMES_COMPILLER_H
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Components_Config.h"
#include "Components_Types.h"

//==============================================================================
//defines:

#define HERMES__PACKED_PREFIX
#define HERMES__PACKED_POSTFIX __packed

#define HERMES_PERIPHERAL_STABILIZATION_TIME_MS 4000
//------------------------------------------------------------------------------
//memories sections:

#define MQTT_STORED_CERTIFICATE_MEM_SECTION __attribute__((section("._user_ram3_section")))
#define MQTT_STORED_PRIVATE_KEY_MEM_SECTION __attribute__((section("._user_ram3_section")))
#define MQTT_SUREFLAP_CREDENTIALS_MEM_SECTION __attribute__((section("._user_ram3_section")))
#define MQTT_CERTIFICATE_MEM_SECTION __attribute__((section("._user_dtcmram_section")))
#define MQTT_AWS_CLIENT_MEM_SECTION __attribute__((section("._user_dtcmram_section")))
#define MQTT_INCOMING_MESSAGE_MAILBOX_STORAGE __attribute__((section("._user_dtcmram_section")));
#define MQTT_OUTGOING_MESSAGE_MAILBOX_STORAGE __attribute__((section("._user_dtcmram_section")));

#define DEVICE_STATUS_MEM_SECTION __attribute__((section("._user_dtcmram_section")))
#define DEVICE_STATUS_EXTRA_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

#define SECURITY_KEYS_MEM_SECTION __attribute__((section("._user_dtcmram_section")))
#define SECRET_KEYS_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

#define SERVER_BUFFER_MEM_SECTION __attribute__((section("._user_dtcmram_section")))
#define SERVER_BUFFER_MESSAGE_MEM_SECTION// __attribute__((section("._user_dtcmram_section")))

#define DEVICE_LIST_MEM_SECTION //__attribute__((section("._user_dtcmram_section")))
#define HFU_RECEIVED_PAGE_MEM_SECTION __attribute__((section("._user_dtcmram_section")))
#define DEVICE_BUFFER_MEM_SECTION __attribute__((section("._user_dtcmram_section")))
#define DEVICE_MAX_SIMULTANEOUS_FIRMWARE_UPDATES_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

#define ACKNOWLEDGE_QUEUE_MEM_SECTION __attribute__((section("._user_dtcmram_section")))
#define DATA_ACKNOWLEDGE_QUEUE_MEM_SECTION __attribute__((section("._user_dtcmram_section")))
#define RECEIVED_SEQUENCE_NUMBERS_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

#define SN_RX_PACKET_MEM_SECTION __attribute__((section("._user_dtcmram_section")))
#define SN_RX_BUFFER_MEM_SECTION __attribute__((section("._user_dtcmram_section")))
#define SN_DUMMY_CHUNK_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

//when determining memory, there may be problems with binding functions
#define HUB_REGISTER_BANK_MEM_SECTION// __attribute__((section("._user_dtcmram_section")))

#define HERMES_FLASH_OPERATION_BUFFER_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

#define HERMES_CONSOL_RX_BUFFER_MEM_LOCATION
#define HERMES_CONSOL_TX_BUFFER_MEM_LOCATION __attribute__((section("._user_dtcmram_section")));
//------------------------------------------------------------------------------
//Task stack sizes in words:

#define LED_TASK_PRIORITY osPriorityNormal
#define LED_TASK_STACK_SIZE (0x400/4) //(0x800/4)
#define LED_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

#define SNTP_TASK_PRIORITY osPriorityNormal
#define SNTP_TASK_STACK_SIZE (0x800/4) //(0x400/4)
#define SNTP_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

#define MQTT_TASK_PRIORITY osPriorityNormal
#define MQTT_TASK_STACK_SIZE (0x4000/4) //(0x2000/4)
#define MQTT_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

#define HTTP_POST_TASK_PRIORITY osPriorityNormal
#define HTTP_POST_TASK_STACK_SIZE (0x2000/4) //(0x800/4)
#define HTTP_POST_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

#define HFU_TASK_PRIORITY osPriorityNormal
#define HFU_TASK_STACK_SIZE (0x400/4) //(0x800/4)
#define HFU_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

#define WATCHDOG_TASK_PRIORITY osPriorityNormal
#define WATCHDOG_TASK_STACK_SIZE (0x200/4)
#define WATCHDOG_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

#define HERMES_APP_TASK_PRIORITY osPriorityNormal
#define HERMES_APP_TASK_STACK_SIZE (0x3000/4) //(0x800/4)
#define HERMES_APP_TASK_STACK_MEM_SECTION __attribute__((section("._user_itcmram_section")))

#define TEST_TASK_PRIORITY osPriorityNormal
#define TEST_TASK_STACK_SIZE (0x200/4)
#define TEST_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

#define LABEL_PRINTER_TASK_PRIORITY osPriorityNormal
#define LABEL_PRINTER_TASK_STACK_SIZE (0x1000/4)
#define LABEL_PRINTER_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_section")))

#define SURENET_TASK_PRIORITY osPriorityNormal
#define SURENET_TASK_STACK_SIZE (0x2000/4) //(0x800/4)
#define SURENET_TASK_STACK_MEM_SECTION __attribute__((section("._user_itcmram_section")))

#define HERMES_CONSOL_TX_TASK_SIZE (0x100)
#define HERMES_CONSOL_TX_TASK_PRIORITY osPriorityNormal
#define HERMES_CONSOL_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_section")));

#define HERMES_CONSOL_TASK_SIZE (0x400)
#define HERMES_CONSOL_TASK_PRIORITY osPriorityNormal
#define HERMES_CONSOL_TX_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_section")));
//------------------------------------------------------------------------------


//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_HERMES_COMPILLER_H
