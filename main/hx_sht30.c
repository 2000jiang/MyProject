#include "hx_sht30.h"
#include "oled.h"
#include "string.h"
#include "stdlib.h"
#include "fonts.h"

unsigned char sht30_buf[6] = {0};
float g_temp = 0.0, g_rh = 0.0;

/*
* sht30初始化
* @param[in]   void  		        :无
* @retval      int                  :0成功，其他失败
* @note        修改日志
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n
*/
int sht30_init(void)
{
    int ret;
    //配置SHT30的寄存器
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();                                //新建操作I2C句柄
    i2c_master_start(cmd);                                                       //启动I2C
    i2c_master_write_byte(cmd, SHT30_WRITE_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN); //发地址+写+检查ack
    i2c_master_write_byte(cmd, CMD_FETCH_DATA_H, ACK_CHECK_EN);                  //发数据高8位+检查ack
    i2c_master_write_byte(cmd, CMD_FETCH_DATA_L, ACK_CHECK_EN);                  //发数据低8位+检查ack
    i2c_master_stop(cmd);                                                        //停止I2C
    ret = i2c_master_cmd_begin(I2C_NUM_1, cmd, 100 / portTICK_RATE_MS);          // I2C发送
    i2c_cmd_link_delete(cmd);                                                    //删除I2C句柄
    return ret;
}
/*
* sht30校验算法
* @param[in]   pdata  		        :需要校验的数据
* @param[in]   nbrOfBytes  		    :需要校验的数据长度
* @retval      int                  :校验值
* @note        修改日志
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n
*/
unsigned char SHT3X_CalcCrc(unsigned char *data, unsigned char nbrOfBytes)
{
    unsigned char bit;               // bit mask
    unsigned char crc = 0xFF;        // calculated checksum
    unsigned char byteCtr;           // byte counter
    unsigned int POLYNOMIAL = 0x131; // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

    // calculates 8-Bit checksum with given polynomial
    for (byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++)
    {
        crc ^= (data[byteCtr]);
        for (bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ POLYNOMIAL;
            }
            else
            {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}
/*
* sht30数据校验
* @param[in]   pdata  		        :需要校验的数据
* @param[in]   nbrOfBytes  		    :需要校验的数据长度
* @param[in]   checksum  		    :校验的结果
* @retval      int                  :0成功，其他失败
* @note        修改日志
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n
*/
unsigned char SHT3X_CheckCrc(unsigned char *pdata, unsigned char nbrOfBytes, unsigned char checksum)
{
    unsigned char crc;
    crc = SHT3X_CalcCrc(pdata, nbrOfBytes); // calculates 8-Bit checksum
    if (crc != checksum)
    {
        return 1;
    }
    return 0;
}
/*
* 获取sht30温湿度
* @param[in]   void  		       :无
* @retval      void                :无
* @note        修改日志
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n
*/
int sht30_get_value(void)
{
    int ret;
    //配置SHT30的寄存器
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();                               //新建操作I2C句柄
    i2c_master_start(cmd);                                                      //启动I2C
    i2c_master_write_byte(cmd, SHT30_WRITE_ADDR << 1 | READ_BIT, ACK_CHECK_EN); //发地址+读+检查ack
    i2c_master_read_byte(cmd, &sht30_buf[0], ACK_VAL);                          //读取数据+回复ack
    i2c_master_read_byte(cmd, &sht30_buf[1], ACK_VAL);                          //读取数据+回复ack
    i2c_master_read_byte(cmd, &sht30_buf[2], ACK_VAL);                          //读取数据+回复ack
    i2c_master_read_byte(cmd, &sht30_buf[3], ACK_VAL);                          //读取数据+回复ack
    i2c_master_read_byte(cmd, &sht30_buf[4], ACK_VAL);                          //读取数据+回复ack
    i2c_master_read_byte(cmd, &sht30_buf[5], NACK_VAL);                         //读取数据+不回复ack
    i2c_master_stop(cmd);                                                       //停止I2C
    ret = i2c_master_cmd_begin(I2C_NUM_1, cmd, 100 / portTICK_RATE_MS);         // I2C发送
    if (ret != ESP_OK)
    {
        return ret;
    }
    i2c_cmd_link_delete(cmd); //删除I2C句柄
    //校验读出来的数据，算法参考sht30 datasheet
    if ((!SHT3X_CheckCrc(sht30_buf, 2, sht30_buf[2])) && (!SHT3X_CheckCrc(sht30_buf + 3, 2, sht30_buf[5])))
    {
        ret = ESP_OK; //成功
    }
    else
    {
        ret = 1;
    }
    return ret;
}

void sht30_task(void *pvParameters) //任务函数
{
    while (1)
    {
        if (sht30_get_value() == ESP_OK) //获取温湿度
        {
            //算法参考sht30 datasheet
            g_temp = ((((sht30_buf[0] * 256) + sht30_buf[1]) * 175) / 65535.0 - 45);
            g_rh = (((sht30_buf[3] * 256) + (sht30_buf[4])) * 100 / 65535.0);
            ESP_LOGI("SHT30", "temp:%4.2f C \r\n", g_temp); //℃打印出来是乱码,所以用C
            ESP_LOGI("SHT30", "hum:%4.2f%% RH \r\n", g_rh);
            oled_show_str(27, 0, " Jiang Lei", &Font_7x10, 1);
            sprintf(str_temp, "temp     %4.2f C", g_temp);
            sprintf(str_hum, "hum      %4.2f%% RH", g_rh);
            oled_show_str(0, 12, str_temp, &Font_7x10, 1);
            oled_show_str(0, 24, str_hum, &Font_7x10, 1);
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
