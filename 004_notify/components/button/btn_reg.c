#include "button.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"

void button_callback(button_event_t event)
{
    //发送给led任务
    TaskHandle_t h = led_get_task_handle();
    if (h)
    {
        xTaskNotify(h, event, eSetValueWithOverwrite);
    }
}

void button_init(void)
{
    button_register(button_callback); // 注册事件
    xTaskCreatePinnedToCore(button_task, "btn", 4096, NULL, 5, &btn_handle, 1);
}