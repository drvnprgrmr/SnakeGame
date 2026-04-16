#include <stdio.h>

#include "dot_matrix.h"

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
gpio_num_t rowPins[ROWS] = {GPIO_NUM_32, GPIO_NUM_14, GPIO_NUM_19, GPIO_NUM_26, GPIO_NUM_15, GPIO_NUM_18, GPIO_NUM_21, GPIO_NUM_17};
gpio_num_t colPins[COLS] = {GPIO_NUM_27, GPIO_NUM_4, GPIO_NUM_16, GPIO_NUM_33, GPIO_NUM_5, GPIO_NUM_25, GPIO_NUM_12, GPIO_NUM_13};

void app_main(void)
{
    scanMatrix(allPins);
}
