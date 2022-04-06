#include "string.h"
#include "stdlib.h"
#include <freertos/FreeRTOS.h>  //操作系统
#include "freertos/task.h"
#include "hx_sht30.h"          //温湿度监测
#include "fonts.h"             //字符显示
#include "oled.h"              //oled显示屏
#include "cs.h"                //超声波测距
#include "ir_protocols.h"      //红外遥控
#include "motor.h"
#include "tcp_bsp.h"

// I2C
#define I2C_SCL_IO 33              // SCL->IO33
#define I2C_SDA_IO 32              // SDA->IO32
#define I2C_MASTER_NUM I2C_NUM_1   // I2C_1
#define WRITE_BIT I2C_MASTER_WRITE //写:0
#define READ_BIT I2C_MASTER_READ   //读:1
#define ACK_CHECK_EN 0x1           //主机检查从机的ACK
#define ACK_CHECK_DIS 0x0          //主机不检查从机的ACK
#define ACK_VAL 0x0                //应答
#define NACK_VAL 0x1               //不应答

i2c_config_t conf;