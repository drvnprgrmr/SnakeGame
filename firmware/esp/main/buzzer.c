#include "pwm.h" // lib dependency
#include "buzzer.h"

#include <esp_log.h>
#include <esp_err.h>
#include <esp_timer.h>
#include <driver/gptimer.h>

#define TAG "buzzer"

static ledc_channel_t buzzer_channel;

Note *melody_ptr;
uint8_t melody_idx = 0;
uint8_t melody_length = 1;

static gptimer_handle_t gptimer = NULL;
static gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,     // Select the default clock source
    .direction = GPTIMER_COUNT_UP,          // Counting direction is up
    .resolution_hz = 1UL * 1000UL * 1000UL, // Resolution is 1 KHz, i.e., 1 tick equals 1 millisecond
};

static bool stop_buzzer(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    // Stop the timer when the alarm occurs for the first time
    gptimer_stop(timer);

    // General process for handling event callbacks:
    // 1. Retrieve user context data from user_ctx (passed in from gptimer_register_event_callbacks)
    // 2. Get alarm event data from edata, such as edata->count_value
    // 3. Perform user-defined operations
    // -> update pwm duty cycle to 0 to turn off the buzzer
    if (melody_idx == melody_length - 1)
    {
        // stop playing sound
        update_pwm_duty(buzzer_channel, 0);

        // reset the melody idx and length
        melody_idx = 0;
        melody_length = 1;
    }
    else
    {
        // play next note in the melody
        buzzer_play(&melody_ptr[++melody_idx]);
    }
    // 4. Return whether a hgh-priority task was awakened during the above operations to notify the scheduler to switch tasks
    return false;
}

static gptimer_event_callbacks_t cbs = {
    .on_alarm = stop_buzzer, // Call the user callback function when the alarm event occurs
};

void buzzer_init(gpio_num_t pin) // todo: make to run in its own task
{
    buzzer_channel = pwm_init(pin);

    // Create a timer instance
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    // Register timer event callback functions, allowing user context to be carried
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    // Enable the timer
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    ESP_LOGI(TAG, "Initialized buzzer on pin %i", pin);
}

void buzzer_play(Note *note)
{
    // Start the tone
    update_pwm_duty(buzzer_channel, 0.5);
    update_pwm_freq(note->tone);

    // Start the timer
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    // Reset the timer count to 0
    gptimer_set_raw_count(gptimer, 0);

    // Configure the timeout for stopping the tone
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = note->duration * 1000UL, // Set the actual alarm period, since the resolution is 1ms, 1000 represents 1s

        .flags.auto_reload_on_alarm = false // Disable auto-reload function
    };

    // Update the timer's alarm action
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
}

void buzzer_play_melody(Note *melody, uint8_t length)
{
    melody_ptr = melody;
    melody_idx = 0;
    melody_length =  length;

    // play the first melody
    buzzer_play(melody);
}