#include "dot_matrix.h"

#define TAG "snake"
#define MAX_SNAKE_LENGTH (ROWS + COLS)

typedef enum
{
    RIGHT,
    DOWN,
    LEFT,
    UP,
} Direction;

// Load Screen
bool drawLoadScreen = false;
const uint8_t MaxRightCol = COLS - 1;
uint8_t currentRightColLimit = COLS - 1;

const uint8_t MaxDownRow = ROWS - 1;
uint8_t currentDownRowLimit = ROWS - 1;

const uint8_t MinLeftCol = 0;
uint8_t currentLeftColLimit = 0;

const uint8_t MinUpRow = 1;
uint8_t currentUpRowLimit = 1;

Direction currentDirection = RIGHT;
Cell loadScreenUpdatePointer = {0, 0};
bool cellStatusValue = true;

int64_t loadScreenTimer = 0;
const int64_t loadScreenInterval = 70 * 1000; // 70ms

void resetLoadScreen()
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
void updateLoadScreen()
{
    if (esp_timer_get_time() - loadScreenTimer >= loadScreenInterval)
    {
        // reset timer
        loadScreenTimer = esp_timer_get_time();

        switch (currentDirection)
        {
        case RIGHT:
        {
            if (loadScreenUpdatePointer.c < currentRightColLimit)
            {
                loadScreenUpdatePointer.c++;
                updateCell(&loadScreenUpdatePointer, cellStatusValue);
            }
            else
            {
                // Check if there's somewhere to turn to
                if (loadScreenUpdatePointer.r >= currentDownRowLimit)
                {
                    // if not, reset
                    resetLoadScreen();
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

            break;
        }

        case DOWN:
        {
            if (loadScreenUpdatePointer.r < currentDownRowLimit)
            {
                loadScreenUpdatePointer.r++;
                updateCell(&loadScreenUpdatePointer, cellStatusValue);
            }
            else
            {
                if (loadScreenUpdatePointer.c <= currentLeftColLimit)
                {
                    resetLoadScreen();
                }
                else
                {
                    currentDownRowLimit--;
                    currentDirection = LEFT;
                    loadScreenUpdatePointer.c--;
                    updateCell(&loadScreenUpdatePointer, cellStatusValue);
                }
            }

            break;
        }

        case LEFT:
        {
            if (loadScreenUpdatePointer.c > currentLeftColLimit)
            {
                loadScreenUpdatePointer.c--;
                updateCell(&loadScreenUpdatePointer, cellStatusValue);
            }
            else
            {
                if (loadScreenUpdatePointer.r <= currentUpRowLimit)
                {
                    resetLoadScreen();
                }
                else
                {
                    currentLeftColLimit++;
                    currentDirection = UP;
                    loadScreenUpdatePointer.r--;
                    updateCell(&loadScreenUpdatePointer, cellStatusValue);
                }
            }

            break;
        }

        case UP:
        {
            if (loadScreenUpdatePointer.r > currentUpRowLimit)
            {
                loadScreenUpdatePointer.r--;
                updateCell(&loadScreenUpdatePointer, cellStatusValue);
            }
            else
            {
                if (loadScreenUpdatePointer.c >= currentRightColLimit)
                {
                    resetLoadScreen();
                }
                else
                {
                    currentUpRowLimit++;
                    currentDirection = RIGHT;
                    loadScreenUpdatePointer.c++;
                    updateCell(&loadScreenUpdatePointer, cellStatusValue);
                }
            }
            break;
        }

        default:
            break;
        }
    }
}
// Load Screen

/* -------------------------- Snake and Food Logic -------------------------- */
bool drawSnake = true;
Cell snake[MAX_SNAKE_LENGTH] = {{1, 2}, {0,2}, {0,1}};
uint8_t snakeLength = 3;
Direction snakeDirection = RIGHT;
int64_t snakeUpdateTimer = 0;
const int64_t snakeUpdateInterval = 2000 * 1000;

Cell food;

void printSnake() {
    printf("Snake: ");
    for (int i = 0; i < snakeLength; i++) {
        printf("(%i,%i) ", snake[i].r, snake[i].c);
    }
    printf("\n");
}

void updateSnake()
{
    if (esp_timer_get_time() - snakeUpdateTimer >= snakeUpdateInterval)
    {
        printSnake();
        snakeUpdateTimer = esp_timer_get_time();

        // store previous head position
        Cell previousHead;
        previousHead.r = snake[0].r;
        previousHead.c = snake[0].c;

        // shift entire body to the neighbors position
        for (uint8_t i = snakeLength - 1; i > 0; i--)
        {
            snake[i].r = snake[i - 1].r, snake[i].c = snake[i - 1].c;
        }

        switch (snakeDirection)
        {
        case RIGHT:
        {
            // move head to the right
            snake[0].c = (previousHead.c + 1) % COLS;
        }
        break;

        default:
            break;
        }
    }
}
/* -------------------------------------------------------------------------- */

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_DEBUG);

    initMatrixPins(0);
    testMatrixCoordinates();

    resetMatrix();
    drawMatrix();

    while (true)
    {
        drawMatrix();
        if (drawLoadScreen)
        {
            updateLoadScreen();
        }
        if (drawSnake) {
            updateSnake();
        }
        vTaskDelay(1);
    }
}
