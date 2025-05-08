#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

#define LED_NUM GPIO_NUM_51
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define DUTY_CHANGE 70
#define LEDC_FREQUENCY 5000

void gpio_simple_init(int NUM)
{
    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << NUM),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE};

    ESP_ERROR_CHECK(gpio_config(&cfg));

    gpio_set_level(NUM, 0);
}

void timer_channel_simple_init()
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

void pwm_blink_task(void *param)
{
    gpio_simple_init(LED_NUM);
    timer_channel_simple_init();

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

void app_main(void)
{
    xTaskCreatePinnedToCore(pwm_blink_task, "pwm blink", 1024 * 5, NULL, 3, NULL, 1);
}
