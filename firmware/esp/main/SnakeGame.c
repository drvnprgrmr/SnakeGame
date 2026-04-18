#include <esp_random.h>

#include "dot_matrix.h"
#include "wifi_man.h"
#include "http_server.h"
#include "dns_server.h"

#define TAG "snake"
#define MAX_SNAKE_LENGTH (ROWS + COLS)

typedef enum
{
    RIGHT,
    DOWN,
    LEFT,
    UP,
} Direction;

typedef enum
{
    LOAD_SCREEN,
    GAMEPLAY,
    PAUSE
} GameState;

/* ------------------------------- Load Screen ------------------------------ */
bool drawLoadScreen = false;
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
/* -------------------------------------------------------------------------- */

/* -------------------------- Snake and Food Logic -------------------------- */
bool drawSnake = true;
Cell snake[MAX_SNAKE_LENGTH] = {{0, 0}};
int snakeLength = 1;
Direction snakeDirection = LEFT;
int64_t snakeUpdateTimer = 0;
const int64_t snakeUpdateInterval = 500 * 1000;

Cell food;

void printSnake()
{
    printf("Snake: ");
    for (int i = 0; i < snakeLength; i++)
    {
        printf("(%i,%i) ", snake[i].r, snake[i].c);
    }
    printf("\n");
}

void updateSnake()
{
    if (esp_timer_get_time() - snakeUpdateTimer >= snakeUpdateInterval)
    {
        // printSnake();
        snakeUpdateTimer = esp_timer_get_time();

        // store previous head position
        Cell previousHead;
        previousHead.r = snake[0].r;
        previousHead.c = snake[0].c;

        // shift entire body to the neighbors position
        for (int i = snakeLength - 1; i > 0; i--)
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

        case DOWN:
        {
            // move head down
            snake[0].r = (previousHead.r + 1) % ROWS;
        }
        break;
        case LEFT:
        {
            // move head to the left
            snake[0].c = (previousHead.c + COLS - 1) % COLS;
        }
        break;
        case UP:
        {
            // move head up
            snake[0].r = (previousHead.r + ROWS - 1) % ROWS;
        }
        break;

        default:
            break;
        }

        clearMatrix();
        updateCells(snake, snakeLength, 1);
    }
}

int64_t randomUpdateTimer = 0;
static const int64_t randomUpdateInterval = 5 * 1000 * 1000;
void randomUpdateSnakeDirection()
{
    if (esp_timer_get_time() - randomUpdateTimer >= randomUpdateInterval)
    {
        randomUpdateTimer = esp_timer_get_time();

        // get random integer
        int32_t rand = esp_random() % 4; // one of the four directions

        switch (rand)
        {
        case RIGHT:
        {
            // make sure you can't turn 180deg
            if (currentDirection == LEFT)
            {
                currentDirection = DOWN;
            }
            else
            {
                currentDirection = rand;
            }
        }
        break;
        case DOWN:
        {
            // make sure you can't turn 180deg
            if (currentDirection == UP)
            {
                currentDirection = LEFT;
            }
            else
            {
                currentDirection = rand;
            }
        }
        break;
        case LEFT:
        {
            // make sure you can't turn 180deg
            if (currentDirection == RIGHT)
            {
                currentDirection = UP;
            }
            else
            {
                currentDirection = rand;
            }
        }
        break;
        case UP:
        {
            // make sure you can't turn 180deg
            if (currentDirection == DOWN)
            {
                currentDirection = LEFT;
            }
            else
            {
                currentDirection = rand;
            }
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
    // esp_log_level_set("*", ESP_LOG_DEBUG);
    /*
        Turn of warnings from HTTP server as redirecting traffic will yield
        lots of invalid requests
    */
    esp_log_level_set("httpd_uri", ESP_LOG_ERROR);
    esp_log_level_set("httpd_txrx", ESP_LOG_ERROR);
    esp_log_level_set("httpd_parse", ESP_LOG_ERROR);

    initMatrixPins(0);

    // initialize wifi
    initNvs();
    wifi_init_softap();

    // Configure DNS-based captive portal
    dhcp_set_captiveportal_url();

    // start http server
    httpd_handle_t server = start_webserver();
    register_softap_uris(server);

    // Start the DNS server that will redirect all queries to the softAP IP
    dns_server_config_t config = DNS_SERVER_CONFIG_SINGLE("*" /* all A queries */, "WIFI_AP_DEF" /* softAP netif ID */);
    start_dns_server(&config);

    while (true)
    {
        drawMatrix();
        if (drawLoadScreen)
        {
            updateLoadScreen();
        }
        if (drawSnake)
        {
            updateSnake();
            // randomUpdateSnakeDirection();
        }
        vTaskDelay(1);
    }
}
