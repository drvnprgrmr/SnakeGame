#pragma once

#include <driver/gpio.h>
#include <esp_timer.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#define ROWS 8
#define COLS 8

#ifdef __cplusplus
extern "C"
{
#endif

    void initMatrix(uint8_t state);
    void scanMatrix();
    void testMatrixCoordinates();

#ifdef __cplusplus
}
#endif