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
#include <stdlib.h>

#include "Components_Config.h"
#include "Components_Types.h"
//==============================================================================
//defines:

#define HERMES__PACKED_PREFIX
#define HERMES__PACKED_POSTFIX __packed
//------------------------------------------------------------------------------
//memories sections:

#define MQTT_STORED_CERTIFICATE_MEM_SECTION __attribute__((section("._user_ram3_ram")))
#define MQTT_STORED_PRIVATE_KEY_MEM_SECTION __attribute__((section("._user_ram3_ram")))
#define MQTT_SUREFLAP_CREDENTIALS_MEM_SECTION __attribute__((section("._user_ram3_ram")))
#define MQTT_CERTIFICATE_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))
#define MQTT_AWS_CLIENT_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))

#define DEVICE_STATUS_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))
#define DEVICE_STATUS_EXTRA_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))

#define SECURITY_KEYS_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))
#define SECRET_KEYS_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))

#define SERVER_BUFFER_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))
#define SERVER_BUFFER_MESSAGE_MEM_SECTION// __attribute__((section("._user_dtcmram_ram")))

#define DEVICE_LIST_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))
#define HFU_RECEIVED_PAGE_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))
#define DEVICE_BUFFER_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))
#define DEVICE_MAX_SIMULTANEOUS_FIRMWARE_UPDATES_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))

#define ACKNOWLEDGE_QUEUE_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))
#define DATA_ACKNOWLEDGE_QUEUE_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))
#define RECEIVED_SEQUENCE_NUMBERS_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))

#define SN_RX_PACKET_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))
#define SN_RX_BUFFER_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))
#define SN_DUMMY_CHUNK_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))

//when determining memory, there may be problems with binding functions
#define HUB_REGISTER_BANK_MEM_SECTION// __attribute__((section("._user_dtcmram_ram")))

#define HERMES_FLASH_OPERATION_BUFFER_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))
//------------------------------------------------------------------------------
// Task stack sizes in words:

#define LABEL_PRINTER_TASK_STACK_SIZE (0x800/4)
#define TEST_TASK_STACK_SIZE (0x400/4) //(0x800/4)

#define SHELL_TASK_STACK_SIZE (0x800/4) //(0x1000/4)

#define LED_TASK_PRIORITY osPriorityNormal
#define LED_TASK_STACK_SIZE (0x400/4) //(0x800/4)
#define LED_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))

#define SNTP_TASK_PRIORITY osPriorityNormal
#define SNTP_TASK_STACK_SIZE (0x400/4) //(0x400/4)
#define SNTP_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))

#define MQTT_TASK_PRIORITY osPriorityNormal
#define MQTT_TASK_STACK_SIZE (0x4000/4) //(0x2000/4)
#define MQTT_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))

#define HTTP_TASK_PRIORITY osPriorityNormal
#define HTTP_POST_TASK_STACK_SIZE (0x2000/4) //(0x800/4)
#define HTTP_POST_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))

#define HFU_TASK_PRIORITY osPriorityNormal
#define HFU_TASK_STACK_SIZE (0x400/4) //(0x800/4)
#define HFU_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))

#define WATCHDOG_TASK_PRIORITY osPriorityNormal
#define WATCHDOG_TASK_STACK_SIZE (0x200/4)
#define WATCHDOG_TASK_STACK_MEM_SECTION __attribute__((section("._user_dtcmram_ram")))

#define HERMES_TASK_PRIORITY osPriorityNormal
#define HERMES_APPLICATION_TASK_STACK_SIZE (0x3000/4) //(0x800/4)
#define HERMES_APPLICATION_TASK_STACK_MEM_SECTION __attribute__((section("._user_itcmram_stack")))

#define SURENET_TASK_PRIORITY osPriorityNormal
#define SURENET_TASK_STACK_SIZE (0x2000/4) //(0x800/4)
#define SURENET_TASK_STACK_MEM_SECTION __attribute__((section("._user_itcmram_stack")))
//------------------------------------------------------------------------------


//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_HERMES_COMPILLER_H
