#include <esp_random.h>
#include <cJSON.h>

#include "wifi_man.h"
#include "http_server.h"
#include "nvs_helpers.h"
#include "dot_matrix.h"
#include "buzzer.h"

#define TAG "snake"
#define MAX_SNAKE_LENGTH ((ROWS * COLS) / 2) // limit the snake to half the real estate

Note start_note = {
    .tone = T_C4,
    .duration = 2 * 1000,
};

Note eat_melody[4] = {
    {T_C5, 100}, // Start mid
    {T_E5, 100}, // Up
    {T_G5, 100}, // Up
    {T_C6, 250}  // High note, held longer
};

Note lose_melody[4] = {
    {T_G3, 300},  // Start low
    {T_FS3, 300}, // Down a semi-tone
    {T_F3, 300},  // Down a semi-tone
    {T_E3, 600}   // End on a long, low note
};

extern QueueHandle_t serverQueue;

nvs_handle_t handle;

void onWin(void);
void onLose(void);

typedef enum
{
    RIGHT,
    DOWN,
    LEFT,
    UP,
} Direction;

#pragma region // Load Screen
// TODO: Use #defines
bool drawLoadScreen = true;
const int MaxRightCol = COLS - 1;
int currentRightColLimit = COLS - 1;

const int MaxDownRow = ROWS - 1;
int currentDownRowLimit = ROWS - 1;

const int MinLeftCol = 0;
int currentLeftColLimit = 0;

const int MinUpRow = 1;
int currentUpRowLimit = 1;

Direction currentDirection = RIGHT;
Cell loadScreenUpdatePointer = {0, 0};
bool cellStatusValue = true;

int64_t loadScreenTimer = 0;
const int64_t loadScreenInterval = 70 * 1000; // 70ms

void resetLoadScreen()
{
    drawLoadScreen = true;

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
#pragma endregion

#pragma region // Snake and Food Update Logic
#define DRAW_SNAKE false
#define SNAKE_LENGTH 1
#define SNAKE_DIRECTION RIGHT
#define SNAKE_UPDATE_TIMER 0
#define SNAKE_UPDATE_INTERVAL 300LL * 1000LL

bool drawSnake = DRAW_SNAKE;
Cell snake[MAX_SNAKE_LENGTH] = {{0, 0}}; // maybe malloc instead at init?
int snakeLength = SNAKE_LENGTH;
Direction snakeDirection = SNAKE_DIRECTION;
int64_t snakeUpdateTimer = SNAKE_UPDATE_TIMER;

Cell food = {ROWS - 1, COLS - 1}; // Set food at the end corner of the matrix initially

void resetSnake()
{
    drawSnake = DRAW_SNAKE;
    snake[0].r = 0, snake[0].c = 0;
    snakeLength = 1;
    snakeDirection = SNAKE_DIRECTION;
    snakeUpdateTimer = 0;
    food.r = ROWS - 1, food.c = COLS - 1;
}

void updateFood()
{
    bool spotFound = false;

    while (!spotFound)
    {
        // choose random position for food
        food.r = esp_random() % ROWS, food.c = esp_random() % COLS;

        // check to ensure that cell isn't taken by a snake
        spotFound = true;
        for (int i = 0; i < snakeLength; i++)
        {
            // check if food is on the snake
            if (food.r == snake[i].r && food.c == snake[i].c)
            {
                spotFound = false;
                break;
            }

            // check if food is in the snake's immediate vicinity (i.e. one cell in any manhattan direction from the entire body)
            if (
                (food.r == ((snake[i].r + 1) % ROWS) && food.c == snake[i].c) ||        // check cell down
                (food.r == ((snake[i].r - 1 + ROWS) % ROWS) && food.c == snake[i].c) || // check cell up
                (food.r == snake[i].r && food.c == ((snake[i].c + 1) % COLS)) ||        // check cell right
                (food.r == snake[i].r && food.c == ((snake[i].c - 1 + COLS) % COLS)))   // check cell left
            {
                spotFound = false;
                break;
            }
        }
    }

    // play eat melody after eating food
    buzzer_play_melody(eat_melody, 4);
}

void printSnake()
{
    printf("Snake: ");
    for (int i = 0; i < snakeLength; i++)
    {
        printf("(%i,%i) ", snake[i].r, snake[i].c);
    }
    printf("\n");
}

void shiftSnake()
{
    // shift entire body to the neighbors position
    for (int i = snakeLength - 1; i > 0; i--)
    {
        snake[i].r = snake[i - 1].r, snake[i].c = snake[i - 1].c;
    }
}

void updateSnakeHead(Cell *head)
{
    // set new head
    snake[0].r = head->r, snake[0].c = head->c;
}

void updateGame()
{
    if (esp_timer_get_time() - snakeUpdateTimer >= SNAKE_UPDATE_INTERVAL)
    {
        // printSnake();
        snakeUpdateTimer = esp_timer_get_time();

        // store head position
        Cell head;
        head.r = snake[0].r;
        head.c = snake[0].c;

        // calculate new head position based on direction
        switch (snakeDirection)
        {
        case RIGHT:
        {
            // move head to the right
            head.c = (head.c + 1) % COLS;
        }
        break;

        case DOWN:
        {
            // move head down
            head.r = (head.r + 1) % ROWS;
        }
        break;
        case LEFT:
        {
            // move head to the left
            head.c = (head.c + COLS - 1) % COLS;
        }
        break;
        case UP:
        {
            // move head up
            head.r = (head.r + ROWS - 1) % ROWS;
        }
        break;

        default:
            break;
        }

        // check if head is on the food
        if (head.r == food.r && head.c == food.c)
        {
            if (++snakeLength == MAX_SNAKE_LENGTH)
            {
                onWin(); // congrats! you won.
                return;
            }

            // update snake
            shiftSnake();
            updateSnakeHead(&head);

            // add new food
            updateFood();
        }
        else
        {
            // just update snake
            shiftSnake();
            updateSnakeHead(&head);
        }

        clearMatrix();

        updateCell(&food, 1);               // draw food in the matrix
        updateCells(snake, snakeLength, 1); // draw snake in the matrix
    }
}

#pragma endregion

#pragma region // Gamesore and win or loose logic
void checkSnakeCollision()
{
    bool collided = false;
    for (uint i = 0; i < snakeLength - 1; i++)
    {
        if (collided)
            break;
        for (uint j = i + 1; j < snakeLength; j++)
        {
            // check if the two cells have the same value
            if (snake[i].r == snake[j].r && snake[i].c == snake[j].c)
            {
                onLose();

                collided = true;
                break;
            }
        }
    }
}

void revealScore()
{
    clearMatrix();

    uint mark = 0;
    for (int r = 0; r < ROWS; r++)
    {
        if (mark == snakeLength)
        {
            break;
        }
        for (int c = 0; c < COLS; c++)
        {
            Cell cell = {r, c};
            updateCell(&cell, 1);

            if (++mark == snakeLength)
            {
                break;
            }
        }
    }

    uint64_t timer = esp_timer_get_time();
    uint64_t timeout = 5 * 1000 * 1000;

    // show score by turning on corresponding dots in the matrix
    // todo: play buzzer tone
    while (esp_timer_get_time() - timer < timeout)
    {
        drawMatrix();
        vTaskDelay(1);
    }

    clearMatrix();
}

void resetGame()
{
    resetLoadScreen(); // reset load screen variables
    resetSnake();
}

void onWin()
{
    revealScore();
    resetGame();
}

void onLose()
{
    revealScore();
    resetGame();

    // play lose melody
    buzzer_play_melody(lose_melody, 4);
}

#pragma endregion // Gamesore and win or loose logic

#pragma region // HTTP server data manage

void handle_server_data()
{
    char *data = " ";

    if (xQueueReceive(serverQueue, (void *)&data, 0) != pdPASS)
    {
        return;
    }

    ESP_LOGI(TAG, "Data received from server queue: %s", data);

    cJSON *root = cJSON_Parse(data);
    char *ctx = cJSON_GetObjectItem(root, "ctx")->valuestring;

    if (strcmp(ctx, "captive") == 0)
    {
        // get ssid and password
        char *ssid = cJSON_GetObjectItem(root, "ssid")->valuestring;
        char *password = cJSON_GetObjectItem(root, "password")->valuestring;

        ESP_LOGI(TAG, "ssid: %s, password: %s", ssid, password);

        nvs_write_str(&handle, "sta_en", "y"); // indicates whether the esp should restart in station mode
        nvs_write_str(&handle, "sta_ssid", ssid);
        nvs_write_str(&handle, "sta_pass", password);

        // restart the esp
        nvs_close(handle);
        esp_restart();
    }
    else if (strcmp(ctx, "start") == 0)
    {
        resetLoadScreen();
        drawLoadScreen = false;
        drawSnake = true;
    }
    else if (strcmp(ctx, "pause") == 0)
    {
        drawLoadScreen = true;
        drawSnake = false;
    }
    else if (strcmp(ctx, "move") == 0)
    {
        char *direction = cJSON_GetObjectItem(root, "direction")->valuestring;

        if (strcmp(direction, "UP") == 0)
        {
            snakeDirection = UP;
        }
        else if (strcmp(direction, "DOWN") == 0)
        {
            snakeDirection = DOWN;
        }
        else if (strcmp(direction, "LEFT") == 0)
        {
            snakeDirection = LEFT;
        }
        else if (strcmp(direction, "RIGHT") == 0)
        {
            snakeDirection = RIGHT;
        }
    }

    cJSON_Delete(root);

    free(data); // make sure to free the initial memory assigned
}

#pragma endregion

void app_main(void)
{
    // esp_log_level_set("*", ESP_LOG_DEBUG);

    // init buzzer
    buzzer_init(GPIO_NUM_22);
    buzzer_play(&start_note);

    // init dot matrix
    initMatrixPins(0);

    // init NVS
    init_nvs(&handle);

    // initialise wifi
    wifi_init_captive_mode();

    while (true)
    {
        drawMatrix();
        if (drawLoadScreen)
        {
            updateLoadScreen();
        }
        else if (drawSnake)
        {
            updateGame();
            checkSnakeCollision();
        }

        // handle data gotten from the server
        handle_server_data();

        vTaskDelay(1);
    }
}
