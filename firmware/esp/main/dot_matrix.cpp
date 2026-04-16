#include "dot_matrix.h"

// Run scanMatrix to figure these out
gpio_num_t rowPins[ROWS] = {GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC};
gpio_num_t colPins[COLS] = {GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC};

extern "C"
{
    void initMatrix(gpio_num_t allPins[ROWS + COLS], uint8_t state = 0)
    {
        for (int i = 0; i < ROWS + COLS; i++)
        {
            gpio_set_direction(allPins[i], GPIO_MODE_OUTPUT);
            gpio_set_level(allPins[i], state);
        }
    }

    void scanMatrix(gpio_num_t allPins[ROWS + COLS])
    {

        // Stage 1: Set all pins low then trigger individual pins high
        initMatrix(allPins);
        for (int i = 0; i < ROWS + COLS; i++)
        {
            gpio_set_level(allPins[i], 1);

            // user should use this time to read the appropriate row
            vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);

            gpio_set_level(allPins[i], 0);
        }

        // add longer delay between stages
        vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);

        // Stage 2: Set all pins high then trigger individual pins low
        initMatrix(allPins, 1);
        for (int i = 0; i < ROWS + COLS; i++)
        {
            gpio_set_level(allPins[i], 0);

            // user should use this time to read the appropriate col
            vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);

            gpio_set_level(allPins[i], 1);
        }
    }
}
