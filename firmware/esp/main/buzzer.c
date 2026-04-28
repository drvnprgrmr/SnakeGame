#include "pwm.h" // lib dependency
#include "buzzer.h"

#include <esp_log.h>
#include <esp_err.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TAG "buzzer"

static ledc_channel_t buzzer_channel;

void buzzer_init(gpio_num_t pin) // todo: make to run in its own task
{
    buzzer_channel = pwm_init(pin);

    ESP_LOGI(TAG, "Initialized buzzer on pin %i", pin);
}

void buzzer_play(Note *note)
{
    // Start the tone
    update_pwm_duty(buzzer_channel, 0.5);
    update_pwm_freq(note->tone);

    // delay
    vTaskDelay(note->duration / portTICK_PERIOD_MS);

    // stop the tone
    update_pwm_duty(buzzer_channel, 0);
}

void buzzer_play_melody(Note *melody, uint8_t length)
{
    for (uint8_t i = 0; i < length; i++)
    {
        buzzer_play(&melody[i]);
    }
}