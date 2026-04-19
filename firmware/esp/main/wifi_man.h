#pragma once

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/inet.h"

#define AP_SSID "SnakeGame"
#define AP_PASS "snakesss"
#define AP_CHANNEL 1
#define AP_MAX_STA_CONN 1

#if CONFIG_ESP_GTK_REKEYING_ENABLE
#define EXAMPLE_GTK_REKEY_INTERVAL CONFIG_ESP_GTK_REKEY_INTERVAL
#else
#define EXAMPLE_GTK_REKEY_INTERVAL 0
#endif


void wifi_init_softap(void);


#ifndef WIFI_MAN
void dhcp_set_captiveportal_url(void);
#endif