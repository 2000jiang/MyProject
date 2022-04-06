#include "cs.h"
#include "oled.h"
#include "motor.h"
int64_t star = 0;
int64_t endTime = 0;
esp_timer_handle_t ceju = 0; //定时器句柄
void distance_task(void *pvParameters)
{
  
    double x = 50.0;
    esp_timer_create_args_t once_test = {//定时器结构体初始化
                                         .callback = &timer_once,
                                         .arg = NULL,
                                         .name = "HC_SR04Timer"};
    //设置Trig引脚为输出模式 Echo引脚为输入模式//
    gpio_pad_select_gpio(Trig);
    gpio_pad_select_gpio(Echo);
    gpio_set_direction(Trig, GPIO_MODE_OUTPUT);
    gpio_set_direction(Echo, GPIO_MODE_INPUT);
    led_once();
    while (1)
    {
        esp_timer_init();                    //初始化
        esp_timer_create(&once_test, &ceju); //创建定时器

        //开始时Trig引脚先拉低20us，保证不被干扰
        gpio_set_level(Trig, 0);
        ets_delay_us(10);
        //然后拉高Trig至少10us以上
        gpio_set_level(Trig, 1);
        ets_delay_us(10);
        //再拉低Trig，完成一次声波发出信号
        gpio_set_level(Trig, 0);

        //检测Echo引脚，一直为低电平就一直在等待
        while (gpio_get_level(Echo) == 0)
        {
        }
        // Echo低电平结束后，定时器开始一个单个周期工作，到达1740us就回调函数
        esp_timer_start_once(ceju, 1740);
        ets_delay_us(1); //延时1微秒

        //获取当前时间    记录当前时间star为Echo引脚变高的开始时间
        star = esp_timer_get_time();
        while (gpio_get_level(Echo) == 1)
        {
            //  vTaskDelay(10 / portTICK_PERIOD_MS); //注意，一定要把结束时间的代码放到循环里面，如果放到循环外面可能不能正常测距
        }
        endTime = esp_timer_get_time(); //记录Echo引脚变为低电平的结束时间
        esp_timer_delete(ceju);         //删除定时器，等下次循环再开启
        //计算Echo引脚高电平持续的时间
        endTime = endTime - star;
        //计算距离，时间单位为us
        double cm = (((double)endTime * 0.034) / 2);
        printf("The distance is %lf cm\n\n", cm); //输出数据
        if (cm <= x)
        {
            gpio_set_level(GPIO_OUTPUT_IO_LED, 1);
            brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_UNIT_1, MCPWM_TIMER_1, 0.0);
        }
        else
        
            gpio_set_level(GPIO_OUTPUT_IO_LED, 0);
           // brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_UNIT_1, MCPWM_TIMER_1, 0.0);
        

        sprintf(distance, "distance %4.2fcm ", cm);
        oled_show_str(0, 36, distance, &Font_7x10, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
//定时器回调函数，作为超时处理
void timer_once(void *arg)
{
    int64_t tick = esp_timer_get_time();
    printf("定时器开始时间已有=%lld \n", tick);
    esp_timer_delete(ceju);
}

void led_once(void) // led和蜂鸣器共用GPIO2来控制
{
    //定义一个gpio_config类型的结构体，下面的都算对其进行的配置
    gpio_config_t io_conf = {
        //禁用中断
        .intr_type = GPIO_PIN_INTR_DISABLE,
        //设置为输出模式
        .mode = GPIO_MODE_OUTPUT,
        //你想设置的引脚
        .pin_bit_mask = (1 << GPIO_OUTPUT_IO_LED),
        //关闭下拉模式
        .pull_down_en = 0,
        //禁用牵引模式
        .pull_up_en = 0,
    };
    //配置GPIO与给定的设置。
    gpio_config(&io_conf);
}