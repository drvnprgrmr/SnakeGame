#include <stdio.h>
#include "dot_matrix.h"

#define TAG "snake"

typedef enum
{
    RIGHT,
    DOWN,
    LEFT,
    UP,
} Direction;

// Load Screen
const uint8_t MaxRightCol = COLS - 1;
uint8_t currentRightColLimit = COLS - 1;

const uint8_t MaxDownRow = ROWS - 1;
uint8_t currentDownRowLimit = ROWS - 1;

const uint8_t MinLeftCol = 0;
uint8_t currentLeftColLimit = 0;

const uint8_t MinUpRow = 0;
uint8_t currentUpRowLimit = 0;

Direction currentDirection = RIGHT;
Cell loadScreenUpdatePointer = {0, 0};
bool cellStatusValue = true;

int64_t loadScreenTimer = 0;
const int64_t loadScreenInterval = 250 * 1000; // 100ms

void resetSnakeLoadScreen()
{
    // reset direction
    currentDirection = RIGHT;
    // reset pointer
    loadScreenUpdatePointer.r = 0, loadScreenUpdatePointer.c = 0;
    // reset limits
    currentRightColLimit = MaxRightCol, currentDownRowLimit = MaxDownRow, currentLeftColLimit = MinLeftCol, currentUpRowLimit = MinUpRow;
    // toggle cell status value to be updated
    cellStatusValue = !cellStatusValue;
    // update the start cell
    updateCell(&loadScreenUpdatePointer, cellStatusValue);
}
void updateSnakeLoadScreen()
{
    if (esp_timer_get_time() - loadScreenTimer >= loadScreenInterval)
    {
        // reset timer
        loadScreenTimer = esp_timer_get_time();

        if (currentDirection == RIGHT)
        {
            if (loadScreenUpdatePointer.c < currentRightColLimit)
            {
                updateCell(&loadScreenUpdatePointer, cellStatusValue);
                loadScreenUpdatePointer.c++;
            }
            else
            {
                // Check if there's somewhere to turn to
                if (loadScreenUpdatePointer.r + 1 >= currentDownRowLimit)
                {
                    // if not, reset
                    resetSnakeLoadScreen();
                }
                else
                {
                    // The load screen has reached the rightmost end, we should reduce the right limit and turn now
                    currentRightColLimit--;
                    currentDirection = DOWN;
                    loadScreenUpdatePointer.r++;
                    updateCell(&loadScreenUpdatePointer, cellStatusValue);
                }
            }
        }

        else if (currentDirection == DOWN)
        {
            if (loadScreenUpdatePointer.r < currentDownRowLimit)
            {
                loadScreenUpdatePointer.r++;
                updateCell(&loadScreenUpdatePointer, cellStatusValue);
            }
            else
            {
                if (loadScreenUpdatePointer.c - 1 <= currentLeftColLimit)
                {
                    resetSnakeLoadScreen();
                }
                else
                {
                    currentDownRowLimit--;
                    currentDirection = LEFT;
                    loadScreenUpdatePointer.c--;
                    updateCell(&loadScreenUpdatePointer, cellStatusValue);
                }
            }
        }

        else if (currentDirection == LEFT)
        {
            if (loadScreenUpdatePointer.c > currentLeftColLimit)
            {
                loadScreenUpdatePointer.c--;
                updateCell(&loadScreenUpdatePointer, cellStatusValue);
            }
            else
            {
                if (loadScreenUpdatePointer.r - 1 <= currentUpRowLimit)
                {
                    resetSnakeLoadScreen();
                }
                else
                {
                    currentLeftColLimit++;
                    currentDirection = UP;
                    loadScreenUpdatePointer.r--;
                    updateCell(&loadScreenUpdatePointer, cellStatusValue);
                }
            }
        }

        else if (currentDirection == UP)
        {
            if (loadScreenUpdatePointer.r > currentUpRowLimit)
            {
                loadScreenUpdatePointer.r--;
                updateCell(&loadScreenUpdatePointer, cellStatusValue);
            }
            else
            {
                if (loadScreenUpdatePointer.c + 1 >= currentRightColLimit)
                {
                    resetSnakeLoadScreen();
                }
                else
                {
                    currentUpRowLimit++;
                    currentDirection = RIGHT;
                    loadScreenUpdatePointer.c++;
                    updateCell(&loadScreenUpdatePointer, cellStatusValue);
                }
            }
        }
    }
}
// Load Screen

void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    initMatrixPins(1);
    // testMatrixCoordinates();

    // resetMatrix();
    // drawMatrix();
    // ESP_LOGD("main", );
    vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);


    gpio_set_level(rowPins[1], 0);
    vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);
    gpio_set_level(colPins[0], 0);
    gpio_set_level(colPins[1], 0);
    gpio_set_level(colPins[7], 0);

    // while (true)
    // {
    //     drawMatrix();
    //     updateSnakeLoadScreen();
    //     vTaskDelay(1 / portTICK_PERIOD_MS);
    // }
}
