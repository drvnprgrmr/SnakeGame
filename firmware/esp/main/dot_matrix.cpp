#include "dot_matrix.h"

#define TAG "dot_matrix"

gpio_num_t allPins[ROWS + COLS] = {
    // Top
    GPIO_NUM_13, // col 8
    GPIO_NUM_12, // col 7
    GPIO_NUM_14, // row 2
    GPIO_NUM_27, // col 1
    GPIO_NUM_26, // row 4
    GPIO_NUM_25, // col 6
    GPIO_NUM_33, // col 4
    GPIO_NUM_32, // row 1
    // Bottom (with label, 1588AS)
    GPIO_NUM_15, // row 5
    GPIO_NUM_21, // row 7
    GPIO_NUM_4,  // col 2
    GPIO_NUM_16, // col 3
    GPIO_NUM_17, // row 8
    GPIO_NUM_5,  // col 5
    GPIO_NUM_18, // row 6
    GPIO_NUM_19  // row 3
};

// Run scanMatrix to figure these out
gpio_num_t rowPins[ROWS] = {allPins[7], allPins[2], allPins[15], allPins[4], allPins[8], allPins[14], allPins[9], allPins[12]};
gpio_num_t colPins[COLS] = {allPins[3], allPins[10], allPins[11], allPins[6], allPins[13], allPins[5], allPins[1], allPins[0]};

bool matrix[ROWS][COLS];

void initMatrixPins(int state = 0)
{
    for (int i = 0; i < ROWS + COLS; i++)
    {
        gpio_set_direction(allPins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(allPins[i], state);
    }
}

void scanMatrix()
{
    // Round 1: Set all pins low then trigger individual pins high
    initMatrixPins();
    for (int i = 0; i < ROWS + COLS; i++)
    {
        gpio_set_level(allPins[i], 1);
        ESP_LOGI(TAG, "Round 1. Pin #%i\n", i + 1);

        // user should use this time to read the appropriate row
        vTaskDelay(7 * 1000 / portTICK_PERIOD_MS);

        gpio_set_level(allPins[i], 0);
    }

    ESP_LOGI(TAG, "\nRound 2 starting soon.\n");

    // add longer delay between stages
    vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);

    // Round 2: Set all pins high then trigger individual pins low
    initMatrixPins(1);
    for (int i = 0; i < ROWS + COLS; i++)
    {
        gpio_set_level(allPins[i], 0);
        ESP_LOGI(TAG, "Round 2. Pin #%i\n", i + 1);

        // user should use this time to read the appropriate col
        vTaskDelay(7 * 1000 / portTICK_PERIOD_MS);

        gpio_set_level(allPins[i], 1);
    }
}

void displayCell(int row, int col)
{
    // activate specified row
    for (int r = 0; r < ROWS; r++)
    {
        if (r == row)
        {
            gpio_set_level(rowPins[r], 0);
        }
        else
        {
            gpio_set_level(rowPins[r], 1);
        }
    }

    // activate specified column
    for (int c = 0; c < COLS; c++)
    {
        if (c == col)
        {
            gpio_set_level(colPins[c], 1);
        }
        else
        {
            gpio_set_level(colPins[c], 0);
        }
    }
}

int64_t timer = 0;
void testMatrixCoordinates()
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            displayCell(r, c);

            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}

void clearMatrix()
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            // set all matrix cells to off
            matrix[r][c] = false;
        }
    }
}

void updateCell(Cell *cell, bool status)
{
    matrix[cell->r][cell->c] = status;
};

void updateCells(Cell *cells, int numCells, bool status)
{
    for (int i = 0; i < numCells; i++)
    {
        updateCell(&cells[i], status);
    }
}

// debug purposes
void printMatrix()
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            printf("%i ", (int)matrix[r][c]);
        }
        printf("\n");
    }
    printf("\n");
}

void drawMatrix()
{
    for (int r = 0; r < ROWS; r++)
    {
        gpio_set_level(rowPins[r], 0);

        for (int c = 0; c < COLS; c++)
        {
            // turn on specific cell if it's activated
            if (matrix[r][c])
            {
                gpio_set_level(colPins[c], 1);
            }
            else
            {
                gpio_set_level(colPins[c], 0);
            }
        }

        ets_delay_us(500); // might need to update to non-blocking solution later when adding wifi
        gpio_set_level(rowPins[r], 1);
    }
}
