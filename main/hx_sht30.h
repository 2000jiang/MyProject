#ifndef HX_SHT30_H
#define HX_SHT30_H

#include <stdio.h>
#include "string.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

/*
===========================
全局变量定义
===========================
*/
// SHT30
#define SHT30_WRITE_ADDR 0x44 //地址
#define CMD_FETCH_DATA_H 0x22 //循环采样，参考sht30 datasheet
#define CMD_FETCH_DATA_L 0x36

char str_temp[30];
char str_hum[30];

/*
===========================
函数声明
===========================
*/

void i2c_init(void);
int sht30_init(void);
unsigned char SHT3X_CalcCrc(unsigned char *data, unsigned char nbrOfBytes);
unsigned char SHT3X_CheckCrc(unsigned char *pdata, unsigned char nbrOfBytes, unsigned char checksum);
int sht30_get_value(void);
void sht30_task(void *pvParameters);

#endif