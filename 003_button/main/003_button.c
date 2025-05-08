#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

typedef enum
{
    BTN_IDLE,
    BTN_DEBOUNCE_PRESS,
    BTN_PRESSED,
    BTN_LONG_PRESSED,
    BTN_DEBOUNCE_RELEASE
} button_state_t;

#define BUTTON_GPIO GPIO_NUM_35
#define DEBOUNCE_TIME_MS 30
#define LONG_PRESS_TIME_MS 2500
#define DOUBLE_CLICK_TIME_MS 500

static button_state_t btn_state = BTN_IDLE;
static int64_t press_time = 0;
static int click_count = 0;
static int64_t last_release_time = 0;
static bool long_press_handled = false;

static const char *TAG = "Button";

void button_task(void *arg)
{
    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .pull_up_en = GPIO_PULLUP_ONLY,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE};

    ESP_ERROR_CHECK(gpio_config(&cfg));

    while (1)
    {
        int level = gpio_get_level(BUTTON_GPIO);
        int64_t now = esp_timer_get_time() / 1000; // ms

        switch (btn_state)
        {
        case BTN_IDLE:
            if (level == 0)
            {
                btn_state = BTN_DEBOUNCE_PRESS;
                press_time = now;
            }
            else if (click_count == 1 && (now - last_release_time) > DOUBLE_CLICK_TIME_MS)
            {
                ESP_LOGI(TAG, "Single Click (timeout) detected");
                click_count = 0;
            }
            break;

        case BTN_DEBOUNCE_PRESS:
            if ((now - press_time) >= DEBOUNCE_TIME_MS)
            {
                if (gpio_get_level(BUTTON_GPIO) == 0)
                {
                    btn_state = BTN_PRESSED;
                    press_time = now;
                }
                else
                {
                    btn_state = BTN_IDLE;
                }
            }
            break;

        case BTN_PRESSED:
            if (level == 1)
            {
                btn_state = BTN_DEBOUNCE_RELEASE;
                press_time = now;
            }
            else if ((now - press_time) >= LONG_PRESS_TIME_MS)
            {
                btn_state = BTN_LONG_PRESSED;
                long_press_handled = true;
                ESP_LOGI(TAG, "Long Press detected");
            }
            break;

        case BTN_LONG_PRESSED:
            if (level == 1)
            {
                btn_state = BTN_DEBOUNCE_RELEASE;
                press_time = now;
            }
            break;

        case BTN_DEBOUNCE_RELEASE:
            if ((now - press_time) >= DEBOUNCE_TIME_MS)
            {
                if (gpio_get_level(BUTTON_GPIO) == 1)
                {
                    btn_state = BTN_IDLE;

                    if (!long_press_handled)
                    {
                        if (click_count == 0)
                        {
                            click_count = 1;
                            last_release_time = now;
                        }
                        else if ((now - last_release_time) <= DOUBLE_CLICK_TIME_MS)
                        {
                            ESP_LOGI(TAG, "Double Click detected");
                            click_count = 0;
                        }
                        else
                        {
                            ESP_LOGI(TAG, "Single Click detected");
                            click_count = 1;
                            last_release_time = now;
                        }
                    }
                    long_press_handled = false;
                }
                else
                {
                    btn_state = BTN_PRESSED;
                }
            }
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    xTaskCreate(button_task, "btn", 2048, NULL, 5, NULL);
}