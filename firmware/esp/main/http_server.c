#include "http_server.h"

#define TAG "http server"

extern const char captive_start[] asm("_binary_captive_html_start");
extern const char captive_end[] asm("_binary_captive_html_end");

#pragma region // Captive portal
static esp_err_t captive_get_handler(httpd_req_t *req)
{
    const uint32_t captive_len = captive_start - captive_end;

    ESP_LOGI(TAG, "Serve captive portal page");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, captive_start, captive_len);

    return ESP_OK;
}

static const httpd_uri_t captive = {
    .uri = "/captive",
    .method = HTTP_GET,
    .handler = captive_get_handler};

#pragma endregion

#pragma region // Captive portal Redirect
esp_err_t captive_redirect_handler(httpd_req_t *req, httpd_err_code_t err)
{
    // Set status
    httpd_resp_set_status(req, "303 See Other");
    // Redirect to the "/captive" directory
    httpd_resp_set_hdr(req, "Location", "/captive");
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

    ESP_LOGI(TAG, "Redirecting captive portal");
    return ESP_OK;
}
#pragma endregion

httpd_handle_t start_webserver()
{
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    esp_err_t err = httpd_start(&server, &cfg);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start server!.\n%s", esp_err_to_name(err));
    }

    ESP_LOGI(TAG, "http server started successfully!");
    return server;
}

void register_softap_uris(httpd_handle_t server)
{
    ESP_LOGI(TAG, "Registering URI handlers for softAP mode.");
    httpd_register_uri_handler(server, &captive);
    httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, captive_redirect_handler);
}