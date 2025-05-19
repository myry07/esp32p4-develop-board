#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "button.h"

static const char *TAG = "Led";

void led_task(void *param)
{
    gpio_init();
    timer_channel_init();
    led_off();
    
    bool led_state = false;
    while (1)
    {
        uint32_t event;
        if (xTaskNotifyWait(0, 0, &event, portMAX_DELAY))
        {
            switch (event)
            {
            case BUTTON_EVENT_SINGLE_CLICK:

                if (pwm_handle != NULL)
                {
                    vTaskDelete(pwm_handle);
                    pwm_handle = NULL;
                }

                led_on();

                ESP_LOGI(TAG, "LED ON");
                break;

            case BUTTON_EVENT_DOUBLE_CLICK:

                if (pwm_handle != NULL)
                {
                    vTaskDelete(pwm_handle);
                    pwm_handle = NULL;
                }

                led_off();
                ESP_LOGI(TAG, "LED OFF");
                break;

            case BUTTON_EVENT_LONG_PRESS:
                ESP_LOGI(TAG, "Start PWM");
                if (pwm_handle == NULL)
                {
                    xTaskCreate(pwm_task, "pwm", 2048, NULL, 2, &pwm_handle);
                }
                break;
            }
        }
    }
}

void led_init()
{
    xTaskCreate(led_task, "led", 2048 * 2, NULL, 3, &led_task_handle);
}