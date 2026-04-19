#pragma once

#include <nvs.h>
#include <nvs_flash.h>


void init_nvs();
void open_nvs_handle(nvs_handle_t *handle);
void nvs_write_str(nvs_handle_t *handle, char *k, char *v);
void nvs_read_str(nvs_handle_t *handle, char *k, char **v);