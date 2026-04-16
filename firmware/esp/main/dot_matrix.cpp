#include "dot_matrix.h"

#define TAG "dot_matrix"

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
        // Round 1: Set all pins low then trigger individual pins high
        initMatrix(allPins);
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
        initMatrix(allPins, 1);
        for (int i = 0; i < ROWS + COLS; i++)
        {
            gpio_set_level(allPins[i], 0);
            ESP_LOGI(TAG, "Round 2. Pin #%i\n", i + 1);

            // user should use this time to read the appropriate col
            vTaskDelay(7 * 1000 / portTICK_PERIOD_MS);

            gpio_set_level(allPins[i], 1);
        }
    }
}
