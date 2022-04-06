#ifndef CS_H
#define CS_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp32/rom/ets_sys.h"

//定义引脚
#define Trig 4
#define Echo 5
#define GPIO_OUTPUT_IO_LED 2


char distance[30];

void distance_task(void *pvParameters);
void timer_once(void *arg); //回调函数
void led_once(void);

#endif