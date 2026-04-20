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

// sta
#define STA_MAX_RECONNECT 3

void wifi_init_softap(void);
void wifi_init_sta(char *ssid, char *pass);
void wifi_init_captive_mode(void);