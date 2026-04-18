#pragma once

#include <esp_http_server.h>
#include <esp_log.h>
#include <sys/param.h>

httpd_handle_t start_webserver();
void register_softap_uris(httpd_handle_t server);