/* wifi_connect.h
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */
#ifndef _TLS_WIFI_H_
#define _TLS_WIFI_H_

#include "esp_idf_version.h"
#include "esp_log.h"
#include "esp_wifi.h"
#if ESP_IDF_VERSION_MAJOR >= 4
#include "esp_event.h"
#else
#include "esp_event_loop.h"
#endif

#define DEFAULT_PORT                     11111

#define TLS_SMP_SERVER_TASK_NAME         "tls_sever_example"
#define TLS_SMP_SERVER_TASK_WORDS        10240
#define TLS_SMP_SERVER_TASK_PRIORITY     8

#define TLS_SMP_WIFI_SSID                CONFIG_WIFI_SSID
#define TLS_SMP_WIFI_PASS                CONFIG_WIFI_PASSWORD

#endif
