#pragma once

#include <driver/gpio.h>
#include <esp_timer.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#define ROWS 8
#define COLS 8

struct Cell
{
    uint8_t r;
    uint8_t c;
};

#ifdef __cplusplus
extern "C"
{
#endif

    void initMatrixPins(uint8_t state);
    void resetMatrix();
    void scanMatrix();
    void testMatrixCoordinates();
    void updateCell(Cell *cell, bool status);
    void updateCells(Cell *cells, uint8_t numCells, bool status);

#ifdef __cplusplus
}
#endif