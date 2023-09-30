/*
 * @Author       : WangSuxiao
 * @Date         : 2023-09-26 09:10:55
 * @LastEditTime : 2023-09-30 15:01:25
 * @Description  :
 * @Tips         :
 */

#include "resource/icon.h"

/**
 * @description: 获取指定编号的图标，在文件中的索引
 * @param {int} icon_code : 图标编号
 * @return {Any}
 */
void store_icon_index(int icon_code)
{
    Serial.println("存储索引数据");
    File file = LittleFS.open(ICON_INDEX_FILE, "a");
    if (!file)
    {
        Serial.println("存储图标索引 ： 索引文件打开失败");
        return;
    }
    file.println(icon_code);
    file.close();
    get_icon_index(icon_code);

}

/**
 * @description: 获取编号为icon_code的图标的索引
 * @param {int} icon_code : 图标编号
 * @return {Any}
 */
int get_icon_index(int icon_code)
{
    Serial.print("定位图标:");
    Serial.println(icon_code);
    File file = LittleFS.open(ICON_INDEX_FILE, "r");
    if (!file)
    {
        Serial.println("定位图标索引 ： 索引文件打开失败");
        return -1;
    }

    int index = 0;
    while (file.available())
    {
        int value = file.parseInt();
        if (value == icon_code)
        {
            // Serial.print("Found at : ");
            // Serial.println(index);
            file.close();
            return index;
        }
        index++;
    }
    Serial.println("定位图标索引 ： 图标不存在");
    file.close();
    return -1;
}

/**
 * @description: 存储图标数据
 * @param {uint8_t} *dataspace : 数据
 * @return {Any}
 */
void store_icon_data(uint8_t *dataspace)
{
    Serial.println("存储图标数据");
    File data_file = LittleFS.open(ICON_DATA_FILE, "a");
    if (!data_file)
    {
        Serial.println("Failed to open data_file for writing");
        return;
    }
    else
    {
        Serial.println("图标文件打开成功");
    }
    data_file.write(dataspace,  get_dataspace_length());
    data_file.close();
}


/**
 * @description: 获取单个图标占用多少个字节
 * @return {Any}
 */
uint8_t get_dataspace_length(){
    uint16_t w_byte = (ICON_WEIGHT % 8) ? (ICON_WEIGHT / 8) + 1 : ICON_WEIGHT / 8;
    return w_byte * ICON_HEIGHT;
}


/**
 * @description: 从文件中读取天气图标数据
 * @param {int} icon_code : 图标编号
 * @param {uint8_t} *dataspace : 缓冲区
 * @return {Any}
 */
bool get_icon_data(int icon_code, uint8_t *dataspace)
{
    uint8_t length = get_dataspace_length();

    int index = get_icon_index(icon_code);
    if (index < 0)
    {
        Serial.println("获取图标数据 :  无法获取图标索引");
        Serial.println(icon_code);
        if (require_icon_data(icon_code, dataspace))
        {
            Serial.println("图标数据请求成功");
            for (size_t i = 0; i < length; i++)
            {
                if (dataspace[i] < 0x10)
                {
                    Serial.print("0");
                }
                Serial.print(dataspace[i], HEX);
                Serial.print(" ");
            }
            Serial.println();

            // 存储索引
            store_icon_index(icon_code);

            // 存储数据
            store_icon_data(dataspace);

            return true;
        }
        return false;
    }

    // 读取文件
    File dataFile = LittleFS.open(ICON_DATA_FILE, "r");
    if (!dataFile)
    {
        Serial.println("获取图标数据 :  图标文件打开失败");
        return false;
    }
    if (dataFile.seek(index * length, SeekSet) != true)
    {
        Serial.println("获取图标数据 :  图标数据定位失败");
        dataFile.close();
        return false;
    }
    if (dataFile.available() < length)
    {
        Serial.println("获取图标数据 :  无法获得足量图标数据");
        dataFile.close();
        return false;
    }
    if (dataFile.read(dataspace, length) != length)
    {
        Serial.println("获取图标数据 :  图标数据读取失败");
        dataFile.close();
        return false;
    }
    return true;
}

/**
 * @description: 向服务器请求数据
 * @param {int} icon_code : 请求的图像编号
 * @param {uint8_t} *dataspace : 缓冲区
 * @return {Any}
 */
bool require_icon_data(int icon_code, uint8_t *dataspace)
{
    Serial.print("请求图标数据，图标编号：");
    Serial.println(icon_code);

    HTTPClient http;
    WiFiClient client;
    http.begin(client, "www.wangsuxiao.com", 80, "/require/icon");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String post_data = "icon_code=" + String(icon_code);
    // String post_data = "icon_code=101";

    int httpCode = http.POST(post_data);

    if (httpCode == HTTP_CODE_OK)
    {

        // Read the response as a stream of bytes and store it in the dataspace array
        WiFiClient *stream = http.getStreamPtr();
        size_t len = stream->available();
        // uint8_t dataspace[len];
        stream->readBytes(dataspace, len);

        // Now you have the response data in the dataspace array
        // You can process it as needed
        for (size_t i = 0; i < len; i++)
        {
            if (dataspace[i] < 0x10)
            {
                Serial.print("0");
            }
            Serial.print(dataspace[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
    else
    {
        Serial.print("Response code: ");
        Serial.println(httpCode);
    }

    http.end();
    return true;
}

// /**
//  * @description: 向服务器请求数据
//  * @param {int} icon_code : 请求的图像编号
//  * @param {uint8_t} *dataspace : 缓冲区
//  * @return {Any}
//  */
// bool require_icon_data(int icon_code, uint8_t *dataspace)
// {
//     // 1. 请求数据
//     WiFiClient client;
//     HTTPClient http;
//     String response = "";

//     http.begin(client, "www.wangsuxiao.com", 80, "/require/icon");
//     http.addHeader("Content-Type", "application/x-www-form-urlencoded");
//     String post_data = "icon_code=" + icon_code ;
//     int httpCode = http.POST(post_data);

//     if (httpCode == HTTP_CODE_OK)
//     {
//         response = http.getString();
//         Serial.print("icon_code : ");
//         Serial.println(icon_code);
//         Serial.print("response : ");
//         Serial.print(response);
//         Serial.print("response length : ");
//         Serial.print(response.length());

//         // 2. 解析数据
//         int ch_size = (response.length() + 1) / 5;
//         unsigned char dataArray[ch_size];
//         int dataIndex = 0;
//         int startIndex = 0;
//         int commaIndex = response.indexOf(',');
//         while (commaIndex != -1 && dataIndex < ch_size)
//         {
//             String hexValue = response.substring(startIndex, commaIndex);
//             unsigned char value = (unsigned char)strtol(hexValue.c_str(), NULL, 16);
//             dataArray[dataIndex] = value;
//             dataIndex++;
//             startIndex = commaIndex + 1;
//             commaIndex = response.indexOf(',', startIndex);
//         }

//         String hexValue = response.substring(startIndex);
//         unsigned char lastvalue = (unsigned char)strtol(hexValue.c_str(), NULL, 16);
//         dataArray[dataIndex] = lastvalue;
//         for (int i = 0; i < ch_size; i++)
//         {
//             Serial.print("dataArray[");
//             Serial.print(i);
//             Serial.print("] = 0x");
//             Serial.print(dataArray[i], HEX);
//             Serial.print(" ");
//         }

//     }
//     else
//     {
//         Serial.println("HTTP GET request failed.");
//     }
//     http.end();
//     return true;
// }
