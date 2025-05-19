#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "led.h"

#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_MAX_DUTY (1 << LEDC_DUTY_RES) - 1

TaskHandle_t led_task_handle = NULL;
TaskHandle_t pwm_handle = NULL;

void gpio_init()
{
    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << LED_NUM),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE};

    ESP_ERROR_CHECK(gpio_config(&cfg));
}

void timer_channel_init()
{
    ledc_timer_config_t ledc_timer_cfg = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_cfg));

    ledc_channel_config_t ledc_channel_cfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .gpio_num = LED_NUM,
        .duty = 0,
        .intr_type = LEDC_INTR_DISABLE};

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_cfg));
}

TaskHandle_t led_get_task_handle(void)
{
    return led_task_handle;
}

void led_on(void)
{
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void led_off(void)
{
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_MAX_DUTY);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void pwm_task(void *param)
{
    while (1)
    {
        for (int duty = 0; duty < (1 << LEDC_DUTY_RES); duty += DUTY_CHANGE)
        {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        for (int duty = (1 << LEDC_DUTY_RES); duty > 0; duty -= DUTY_CHANGE)
        {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}