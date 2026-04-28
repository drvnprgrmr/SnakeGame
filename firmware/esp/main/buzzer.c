#include "pwm.h" // lib dependency
#include "buzzer.h"

#include <esp_log.h>
#include <esp_err.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TAG "buzzer"

static TaskHandle_t task_handle = nullptr;

static ledc_channel_t buzzer_channel;

static Note *melody_ptr;
static uint8_t melody_length;
static bool buzzer_active = false;

static void _buzzer_play(Note *note);

static void buzzer_task(void *params)
{
    ESP_LOGI(TAG, "starting buzzer task");
    while (true)
    {
        if (buzzer_active)
        {
            for (uint8_t i = 0; i < melody_length; i++)
            {
                _buzzer_play(&melody_ptr[i]);
            }

            buzzer_active = false;
        }

        vTaskDelay(1); // allow other tasks to run
    }
}

void buzzer_init(gpio_num_t pin)
{
    buzzer_channel = pwm_init(pin);

    BaseType_t status = xTaskCreate(
        buzzer_task,
        "buzzer_task",
        1024,
        nullptr,
        2,
        &task_handle);

    assert(status == pdPASS);

    ESP_LOGI(TAG, "Initialized buzzer on pin %i", pin);
}

static void _buzzer_play(Note *note)
{
    // Start the tone
    update_pwm_duty(buzzer_channel, 0.5);
    update_pwm_freq(note->tone);

    // delay
    vTaskDelay(note->duration / portTICK_PERIOD_MS);

    // stop the tone
    update_pwm_duty(buzzer_channel, 0);
}

void buzzer_play(Note *note)
{
    buzzer_active = true;

    melody_ptr = note;
    melody_length = 1;
}

void buzzer_play_melody(Note *melody, uint8_t length)
{
    buzzer_active = true;

    melody_ptr = melody;
    melody_length = length;
}