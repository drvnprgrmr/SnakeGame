#pragma once

#include <esp_http_server.h>
#include <esp_log.h>
#include <sys/param.h>
#include <esp_err.h>
#include <esp_check.h>

#define MAX_CONTENT_LEN 128

httpd_handle_t start_webserver();
void register_softap_uris(httpd_handle_t server);
void register_station_uris(httpd_handle_t server);