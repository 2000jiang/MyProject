#include "motor.h"

/**
 * @brief 电机向前移动
 * 
 * @param mcpwm_num0 左边需要的MCPWM
 * @param timer_num0 左边定时器
 * @param mcpwm_num1 右边需要的MCPWM
 * @param timer_num1 右边定时器
 * @param duty_cycle 占空比
 */

 void brushed_motor_forward(mcpwm_unit_t mcpwm_num0, mcpwm_timer_t timer_num0, mcpwm_unit_t mcpwm_num1, mcpwm_timer_t timer_num1, duty_cycle_set_t duty_cycle)
{
    mcpwm_set_duty(mcpwm_num0, timer_num0, MCPWM_OPR_A, duty_cycle_mcpwm0);
    mcpwm_set_duty(mcpwm_num0, timer_num0, MCPWM_OPR_B, duty_cycle_mcpwm100);

    mcpwm_set_duty(mcpwm_num1, timer_num1, MCPWM_OPR_A, duty_cycle_mcpwm0);
    mcpwm_set_duty(mcpwm_num1, timer_num1, MCPWM_OPR_B, duty_cycle_mcpwm100);
}

/**
 * @brief 电机向后移动
 */
 void brushed_motor_backward(mcpwm_unit_t mcpwm_num0, mcpwm_timer_t timer_num0, mcpwm_unit_t mcpwm_num1, mcpwm_timer_t timer_num1, duty_cycle_set_t duty_cycle)
{
    mcpwm_set_duty(mcpwm_num0, timer_num0, MCPWM_OPR_A, duty_cycle_mcpwm100);
    mcpwm_set_duty(mcpwm_num0, timer_num0, MCPWM_OPR_B, duty_cycle_mcpwm0);

    mcpwm_set_duty(mcpwm_num1, timer_num1, MCPWM_OPR_A, duty_cycle_mcpwm100);
    mcpwm_set_duty(mcpwm_num1, timer_num1, MCPWM_OPR_B, duty_cycle_mcpwm0);
}

/**
 * @brief 电机向左移动
 */
void brushed_motor_left(mcpwm_unit_t mcpwm_num0, mcpwm_timer_t timer_num0, mcpwm_unit_t mcpwm_num1, mcpwm_timer_t timer_num1, duty_cycle_set_t duty_cycle)
{
    mcpwm_set_duty(mcpwm_num0, timer_num0, MCPWM_OPR_A, duty_cycle_mcpwm0);
    mcpwm_set_duty(mcpwm_num0, timer_num0, MCPWM_OPR_B, duty_cycle_mcpwm0);

    mcpwm_set_duty(mcpwm_num1, timer_num1, MCPWM_OPR_A, duty_cycle_mcpwm0);
    mcpwm_set_duty(mcpwm_num1, timer_num1, MCPWM_OPR_B, duty_cycle_mcpwm100);
}

/**
 * @brief 电机向右移动
 */
void brushed_motor_right(mcpwm_unit_t mcpwm_num0, mcpwm_timer_t timer_num0, mcpwm_unit_t mcpwm_num1, mcpwm_timer_t timer_num1, duty_cycle_set_t duty_cycle)
{
    mcpwm_set_duty(mcpwm_num0, timer_num0, MCPWM_OPR_A, duty_cycle_mcpwm0);
    mcpwm_set_duty(mcpwm_num0, timer_num0, MCPWM_OPR_B, duty_cycle_mcpwm100);

    mcpwm_set_duty(mcpwm_num1, timer_num1, MCPWM_OPR_A, duty_cycle_mcpwm0);
    mcpwm_set_duty(mcpwm_num1, timer_num1, MCPWM_OPR_B, duty_cycle_mcpwm0);
}

/**
 * @brief 电机停止
 */
void brushed_motor_stop(mcpwm_unit_t mcpwm_num0, mcpwm_timer_t timer_num0, mcpwm_unit_t mcpwm_num1, mcpwm_timer_t timer_num1, duty_cycle_set_t duty_cycle)
{
    mcpwm_set_duty(mcpwm_num0, timer_num0, MCPWM_OPR_A, duty_cycle_mcpwm0);
    mcpwm_set_duty(mcpwm_num0, timer_num0, MCPWM_OPR_B, duty_cycle_mcpwm0);

    mcpwm_set_duty(mcpwm_num1, timer_num1, MCPWM_OPR_A, duty_cycle_mcpwm0);
    mcpwm_set_duty(mcpwm_num1, timer_num1, MCPWM_OPR_B, duty_cycle_mcpwm0);
}

void motor_init(void)
{
    // 1. mcpwm gpio 初始化
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1A, GPIO_PWM1A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1B, GPIO_PWM1B_OUT);

    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_PWM0A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, GPIO_PWM0B_OUT);

    // 2. 初始化 mcpwm 配置
    printf("Configuring Initial Parameters of mcpwm...\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 1000; //频率 = 500Hz,
    pwm_config.cmpr_a = 0;       // PWMxA占空比 = 0
    pwm_config.cmpr_b = 0;       // PWMxb占空比 = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_1, &pwm_config);
}
