#include "main.h"

/**
 * @brief iC2配置
 *
 */
void i2c_init(void)
{
    // i2c配置结构体
    conf.mode = I2C_MODE_MASTER;             // I2C模式
    conf.sda_io_num = I2C_SDA_IO;            // SDA IO映射
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE; // SDA IO模式
    conf.scl_io_num = I2C_SCL_IO;            // SCL IO映射
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE; // SCL IO模式
    conf.master.clk_speed = 100000;          // I2C CLK频率
    i2c_param_config(I2C_MASTER_NUM, &conf); //设置I2C
    //注册I2C服务即使能
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

/*
* 任务：建立TCP连接并从TCP接收数据
* @param[in]   void  		       :无
* @retval      void                :无
* @note        修改日志
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n
*/
static void tcp_connect(void *pvParameters)
{
    while (1)
    {
        g_rxtx_need_restart = false;
        //等待WIFI连接信号量，死等
        xEventGroupWaitBits(tcp_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
        ESP_LOGI(TAG, "start tcp connected");

        TaskHandle_t tx_rx_task = NULL;
#if TCP_SERVER_CLIENT_OPTION
        //延时3S准备建立server
        vTaskDelay(3000 / portTICK_RATE_MS);
        ESP_LOGI(TAG, "create tcp server");
        //建立server
        int socket_ret = create_tcp_server(true);
#else
        //延时3S准备建立clien
        vTaskDelay(3000 / portTICK_RATE_MS);
        ESP_LOGI(TAG, "create tcp Client");
        //建立client
        int socket_ret = create_tcp_client();
#endif
        if (socket_ret == ESP_FAIL)
        {
            //建立失败
            ESP_LOGI(TAG, "create tcp socket error,stop...");
            continue;
        }
        else
        {
            //建立成功
            ESP_LOGI(TAG, "create tcp socket succeed...");

            //建立tcp接收数据任务
            if (pdPASS != xTaskCreate(&recv_data, "recv_data", 4096, NULL, 1, &tx_rx_task))
            {
                //建立失败
                ESP_LOGI(TAG, "Recv task create fail!");
            }
            else
            {
                //建立成功
                ESP_LOGI(TAG, "Recv task create succeed!");  
                
            }
            //  //建立tcp发送数据任务
            // if (pdPASS != xTaskCreate(&rend_data, "rend_data", 4096, NULL, 1, &tx_rx_task))
            // {
            //     //建立失败
            //     ESP_LOGI(TAG, "Rend task create fail!");
            // }
            // else
            // {
            //     //建立成功
            //     ESP_LOGI(TAG, "Rend task create succeed!");  
            // }
        }

        while (1)
        {

            vTaskDelay(3000 / portTICK_RATE_MS);

#if TCP_SERVER_CLIENT_OPTION
            //重新建立server，流程和上面一样
            if (g_rxtx_need_restart)
            {
                ESP_LOGI(TAG, "tcp server error,some client leave,restart...");
                    //建立server
                if (ESP_FAIL != create_tcp_server(false))
                {
                    if (pdPASS != xTaskCreate(&recv_data, "recv_data", 4096, NULL, 1, &tx_rx_task))
                    {
                        ESP_LOGE(TAG, "tcp server Recv task create fail!");
                    }
                    else
                    {
                        ESP_LOGI(TAG, "tcp server Recv task create succeed!");
                        //重新建立完成，清除标记
                        g_rxtx_need_restart = false;
                    }
                }
            }
#else
            //重新建立client，流程和上面一样
            if (g_rxtx_need_restart)
            {
                vTaskDelay(3000 / portTICK_RATE_MS);
                ESP_LOGI(TAG, "reStart create tcp client...");
                //建立client
                int socket_ret = create_tcp_client();

                if (socket_ret == ESP_FAIL)
                {
                    ESP_LOGE(TAG, "reStart create tcp socket error,stop...");
                    continue;
                }
                else
                {
                    ESP_LOGI(TAG, "reStart create tcp socket succeed...");
                    //重新建立完成，清除标记
                    g_rxtx_need_restart = false;
                    //建立tcp接收数据任务
                    if (pdPASS != xTaskCreate(&recv_data, "recv_data", 4096, NULL, 4, &tx_rx_task))
                    {
                        ESP_LOGE(TAG, "reStart Recv task create fail!");
                    }
                    else
                    {
                        ESP_LOGI(TAG, "reStart Recv task create succeed!");
                    }
                }
            }
#endif
        }
    }

    vTaskDelete(NULL);
}

void app_main(void)
{

    i2c_init(); // I2C初始化
    oled_init();
    sht30_init();                       // sht30初始化
    motor_init();                       //初始化电机
    vTaskDelay(100 / portTICK_RATE_MS); //延时100ms
    xTaskCreate(sht30_task, "Sht30_task", 4 * 1024, NULL, 0, NULL);
    xTaskCreate(distance_task, "Distance_task", 4 * 1024, NULL, 0, NULL);
    xTaskCreate(ir_rx_task, "ir_rx_task", 2048, NULL, 0, NULL); //红外接收

    //初始化flash
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

#if TCP_SERVER_CLIENT_OPTION
    // server，建立ap
    wifi_init_softap();
#else
    // client，建立sta
    wifi_init_sta();
#endif
    //新建一个tcp连接任务
    xTaskCreate(tcp_connect, "tcp_connect", 4096, NULL, 0, NULL);
}