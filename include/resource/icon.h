/*
 * @Author       : WangSuxiao
 * @Date         : 2023-07-18 20:10:08
 * @LastEditTime : 2023-07-29 10:43:30
 * @Description  : 定义具体的汉字，函数
 * @Tips         :
 */
#ifndef __ICON_H
#define __ICON_H

#include <stdint.h>
#include<stdint.h>
#include "LittleFS.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


#define ICON_INDEX_FILE "/icon/weather/index"
#define ICON_DATA_FILE "/icon/weather/data"
#define ICON_WEIGHT 32
#define ICON_HEIGHT 32


uint8_t get_dataspace_length();

void store_icon_index(int index);
int get_icon_index(int icon_code);
void store_icon_data(uint8_t *dataspace);
bool get_icon_data(int icon_code, uint8_t *dataspace);

bool require_icon_data(int icon_code, uint8_t *dataspace);


#endif
