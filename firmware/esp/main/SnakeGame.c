#include <stdio.h>

#include "dot_matrix.h"

gpio_num_t allPins[ROWS + COLS] = {
    // Top
    GPIO_NUM_13, // col 8
    GPIO_NUM_12, // col 7
    GPIO_NUM_14,
    GPIO_NUM_27, // col 1
    GPIO_NUM_26, // col 6
    GPIO_NUM_25, // col 4
    GPIO_NUM_33,
    GPIO_NUM_32, //
    // Bottom
    GPIO_NUM_15,
    GPIO_NUM_2,
    GPIO_NUM_4,
    GPIO_NUM_16,
    GPIO_NUM_17,
    GPIO_NUM_5,
    GPIO_NUM_18,
    GPIO_NUM_19};

void app_main(void)
{
    scanMatrix(allPins);
}
