#include "nvs_helpers.h"

#include <esp_log.h>

#define TAG "nvs_helper"

void init_nvs()
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

void open_nvs_handle(nvs_handle_t *handle)
{
    // Open NVS handle
    ESP_LOGI(TAG, "\nOpening Non-Volatile Storage (NVS) handle...");
    esp_err_t err = nvs_open("storage", NVS_READWRITE, handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }
}

void nvs_write_str(nvs_handle_t *handle, char *k, char *v)
{
    // Store and read a string
    ESP_LOGI(TAG, "\nWriting string to NVS...");
    esp_err_t err = nvs_set_str(*handle, k, v);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to write string! %s", esp_err_to_name(err));
    }
}

void nvs_read_str(nvs_handle_t *handle, char *k, char **v)
{
    // Read back the string
    size_t required_size = 0;
    ESP_LOGI(TAG, "\nReading string from NVS...");
    esp_err_t err = nvs_get_str(*handle, k, NULL, &required_size);
    if (err == ESP_OK)
    {
        *v = malloc(required_size);
        err = nvs_get_str(*handle, k, *v, &required_size);
        if (err == ESP_OK)
        {
            ESP_LOGI(TAG, "Read string: %s", *v);
        }
    }
}