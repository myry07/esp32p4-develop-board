#ifndef LED_H_
#define LED_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_NUM GPIO_NUM_51
#define DUTY_CHANGE 70
#define LEDC_FREQUENCY 5000

extern TaskHandle_t led_task_handle;
extern TaskHandle_t pwm_handle;

TaskHandle_t led_get_task_handle(void);

void gpio_init();
void timer_channel_init();
void led_on();
void led_off();
void pwm_task(void *param);
void led_init();

#endif