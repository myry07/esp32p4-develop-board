#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_NUM GPIO_NUM_51

static const char *TAG = "LED";
int led_state = 0;

void gpio_init(int NUM)
{
    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << NUM),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE};

    ESP_ERROR_CHECK(gpio_config(&cfg));
}

void led_task(void *param)
{
    while (1)
    {
        led_state = !led_state;
        gpio_set_level(LED_NUM, led_state);
        ESP_LOGI(TAG, "GPIO NUM: %d, is %d", LED_NUM, led_state);
        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}

void app_main(void)
{
    gpio_init(LED_NUM);
    xTaskCreate(led_task, "led", 2048, NULL, 3, NULL);
}

