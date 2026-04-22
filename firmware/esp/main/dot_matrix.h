#pragma once
#include <rom/ets_sys.h>
#include <driver/gpio.h>
#include <esp_timer.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

// todo: use kconfig
#define ROWS 8
#define COLS 8

typedef struct
{
    int r;
    int c;
} Cell;

#ifdef __cplusplus
extern "C"
{
#endif

    void initMatrixPins(int state);
    void clearMatrix();
    void scanMatrix();
    void printMatrix();
    void drawMatrix();
    void testMatrixCoordinates();
    void updateCell(Cell *cell, bool status);
    void updateCells(Cell *cells, int numCells, bool status);

#ifdef __cplusplus
}
#endif