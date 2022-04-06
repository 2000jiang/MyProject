#ifndef MOTOR_H
#define MOTOR_H

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

#define GPIO_PWM0A_OUT 13 //设置 GPIO 13 作为 PWM0A
#define GPIO_PWM0B_OUT 14 //设置 GPIO 14 作为 PWM0B

#define GPIO_PWM1A_OUT 15 //设置 GPIO 15 作为 PWM1A
#define GPIO_PWM1B_OUT 16 //设置 GPIO 16 作为 PWM1B

typedef enum //占空比设置
{
    duty_cycle_mcpwm0 = 0,
    duty_cycle_mcpwm100 = 70,
} duty_cycle_set_t;

void brushed_motor_forward(mcpwm_unit_t mcpwm_num0, mcpwm_timer_t timer_num0, mcpwm_unit_t mcpwm_num1, mcpwm_timer_t timer_num1, duty_cycle_set_t duty_cycle);
void brushed_motor_backward(mcpwm_unit_t mcpwm_num0, mcpwm_timer_t timer_num0, mcpwm_unit_t mcpwm_num1, mcpwm_timer_t timer_num1, duty_cycle_set_t duty_cycle);
void brushed_motor_left(mcpwm_unit_t mcpwm_num0, mcpwm_timer_t timer_num0, mcpwm_unit_t mcpwm_num1, mcpwm_timer_t timer_num1, duty_cycle_set_t duty_cycle);
void brushed_motor_right(mcpwm_unit_t mcpwm_num0, mcpwm_timer_t timer_num0, mcpwm_unit_t mcpwm_num1, mcpwm_timer_t timer_num1, duty_cycle_set_t duty_cycle);
void brushed_motor_stop(mcpwm_unit_t mcpwm_num0, mcpwm_timer_t timer_num0, mcpwm_unit_t mcpwm_num1, mcpwm_timer_t timer_num1, duty_cycle_set_t duty_cycle);

void motor_init(void);
#endif