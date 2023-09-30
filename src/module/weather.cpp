/*
 * @Author       : WangSuxiao
 * @Date         : 2023-09-28 15:59:22
 * @LastEditTime : 2023-09-30 22:38:40
 * @Description  : 天气相关的
 * @Tips         :
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include "module/weather.h"
#include "config.h"




#include "config.h"

String cityCode = "101190405"; // 天气城市代码
WiFiClient wifiClient;


/**
 * @description:  获取城市ID
 * @return {Any}
 */
String getCityCode()
{
    String cityid;
    String URL = "http://wgeo.weather.com.cn/ip/?_=" + String(now());
    // 创建 HTTPClient 对象
    HTTPClient httpClient;
    // 配置请求地址。此处也可以不使用端口号和PATH而单纯的
    httpClient.begin(wifiClient, URL);

    // 设置请求头中的User-Agent
    httpClient.setUserAgent("Mozilla/5.0 (iPhone; CPU iPhone OS 11_0 like Mac OS X) AppleWebKit/604.1.38 (KHTML, like Gecko) Version/11.0 Mobile/15A372 Safari/604.1");
    httpClient.addHeader("Referer", "http://www.weather.com.cn/");
    httpClient.addHeader("Accept-Language","zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6");
    httpClient.addHeader("Accept-Encoding","gzip, deflate");

    // 启动连接并发送HTTP请求
    int httpCode = httpClient.GET();
    Serial.print("Send GET request to URL: ");
    Serial.println(URL);

    // 如果服务器响应OK则从服务器获取响应体信息并通过串口输出
    if (httpCode == HTTP_CODE_OK)
    {
        String str = httpClient.getString();

        int idIndex = str.indexOf("id=");
        if (idIndex > -1)
        {
            cityid = str.substring(idIndex + 4, idIndex + 4 + 9);
            Serial.println(cityid);
        }
        else
        {
            Serial.println("获取城市代码失败");
        }

        // addr="广西,桂林,桂林";
    }
    else
    {
        Serial.println("请求城市代码错误：");
        Serial.println(httpCode);
    }

    // 关闭ESP8266与服务器连接
    httpClient.end();
    return cityid;
}


/**
 * @description: 获取天气信息
 * @param {DynamicJsonDocument} &doc : 用于解析服务器传回来的JSON对象
 * @param {String} path : 请求路径 / 实时天气 / 24小时天气 / 未来7天
 * @param {String} lon : 经度
 * @param {String} lat : 维度
 * @param {String} cityid : 城市ID / 中国天气网和和风天气通用
 * @param {String} key : 授权码 / 服务器身份验证
 * @return {Any}
 */
String requireWeather(String path, City city)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return "";
    }
    String response = "";
    WiFiClient client;
    HTTPClient http;
    http.begin(client, "www.wangsuxiao.com", 80, path);        // set the server address and path
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // set the content type
    String post_data = "lat=" + city.lat + "&lon=" + city.lon + "&cityid=" + city.cityId + "&key=" + AUTHORIZE_KEY;     // set the POST data
    int httpCode = http.POST(post_data);                                 // send the POST request
    if (httpCode > 0)
    {
        response = http.getString();
        // Serial.println(httpCode); Serial.println(response);
    }
    else
    {
        Serial.println("Error on HTTP request");
    }
    http.end(); // close the connection

    return response;
}



/**
 * @description: 获取未来天气，返回一个列表
 * @param {DynamicJsonDocument} &doc :
 * @param {String} path :
 * @param {City} city :
 * @return {Any}
 */
std::vector<HourlyWeather> getWeatherInfo(DynamicJsonDocument &doc, String path, City city)
{

    std::vector<HourlyWeather> hourlyWeatherData;

    String httpResponse = requireWeather(path, city);
    if (!httpResponse.isEmpty())
    {
        DeserializationError resultCode = deserializeJson(doc, httpResponse);
        if (resultCode == DeserializationError::Ok)
        {
            JsonArray hourly = doc["hourly"].as<JsonArray>();
            for (JsonObject hourlyItem : hourly)
            {
                HourlyWeather weather;
                weather.fxTime = hourlyItem["fxTime"].as<String>();
                weather.temp = hourlyItem["temp"].as<String>();
                weather.icon = hourlyItem["icon"].as<int>();
                weather.text = hourlyItem["text"].as<String>();
                weather.pop = hourlyItem["pop"].as<String>();
                weather.humidity = hourlyItem["humidity"].as<String>();
                weather.precip = hourlyItem["precip"].as<String>();
                hourlyWeatherData.push_back(weather);
            }
        }
        else
        {
            Serial.print("获取天气信息失败，返回值解析失败");
        }
    }
    else
    {
        Serial.print("获取天气信息失败，请求结果为空");
    }

    return hourlyWeatherData;
}


/**
 * @description: 绘制实时天气
 * @param {String} weather : 天气情况描述
 * @param {String} temp : 温度
 * @param {String} humidity : 湿度
 * @param {int} icon : 天气图标
 * @return {Any}
 */
void draw_RealtimeWeather(String weather, String temp, String humidity, int icon)
{
    Paint_DrawIcon_From_File(icon, 8, 32, 32, 32, WHITE, BLACK); // 数据集中0为黑色 ; 而板子0定义为白色
    Paint_DrawString_CN_From_File(52, 40, weather.c_str(), &PINGFANG16, BLACK, WHITE);
    int Xstart = 11;
    int Ystart = 70;
    int index = 0;
    for (uint8_t i = 0; i < temp.length(); i++)
    {
        Paint_DrawChar(Xstart + Weather22.Width * i, Ystart, temp[i], &Weather22, BLACK, WHITE,  (temp[i] == '.') ? 14 : temp[i] - '0');
        index = i;
    }
    Paint_DrawChar(Xstart + Weather22.Width * index + 7, Ystart, '°', &Weather22, BLACK, WHITE, 12);
    Paint_DrawChar(Xstart + Weather22.Width * index + 12, Ystart, 'C', &Weather22, BLACK, WHITE, 13);
    for (uint8_t i = 0; i < humidity.length(); i++)
    {
        index = index + i;
        Paint_DrawChar(Xstart + Weather22.Width * (index + 1) + 24, Ystart, humidity[i], &Weather22, BLACK, WHITE, humidity[i] - '0');
    }
    Paint_DrawChar(Xstart + Weather22.Width * (index + 2) + 24, Ystart, '%', &Weather22, BLACK, WHITE, 10);
}


/**
 * @description: 绘制未来天气
 * @param {vector<HourlyWeather>} &hourlyWeatherData :
 * @return {Any}
 */
void draw_FutureWeather(std::vector<HourlyWeather> &hourlyWeatherData)
{
    String time;
    String temp;
    int icon;
    String text;
    String pop;
    String humidity;
    String precip;

    int loopLimit = hourlyWeatherData.size() < 12 ? hourlyWeatherData.size() : 12;

    Paint_ClearWindows(WEATHER_DRAW_X_START,
                       WEATHER_DRAW_Y_START,
                       WEATHER_DRAW_X_START + 6 * WEATHER_DRAW_X_STEP,
                       WEATHER_DRAW_Y_START + 2 * WEATHER_DRAW_Y_STEP,
                       WHITE);
    int item_x = WEATHER_DRAW_X_START;
    int item_y = WEATHER_DRAW_Y_START;
    uint8_t margin = uint8_t((WEATHER_DRAW_Y_STEP - ICON_HEIGHT - WEATHER_DRAW_FONT_SIZE * 2) / 4);
    uint8_t icon_pad = 10;

    for (int i = 0; i < loopLimit; i++)
    {
        HourlyWeather hourlyItem = hourlyWeatherData[i];
        time = hourlyItem.fxTime;
        temp = hourlyItem.temp;
        icon = hourlyItem.icon;
        text = hourlyItem.text;
        pop = hourlyItem.pop;
        humidity = hourlyItem.humidity;
        precip = hourlyItem.precip;

        Serial.println("绘制图标 : ");
        Paint_DrawIcon_From_File(icon, item_x + icon_pad, item_y, ICON_WEIGHT, ICON_HEIGHT, WHITE, BLACK);
        Serial.println("-------降水概率------");
        Paint_DrawWeatherInfo(item_x, item_y + ICON_HEIGHT + margin, temp, humidity, &Time14, BLACK, WHITE);
        Paint_DrawTime(item_x + 8, item_y + ICON_HEIGHT + WEATHER_DRAW_FONT_SIZE + margin, time, &Time14, BLACK, WHITE);

        if ((i + 1) % 6 == 0)
        {
            item_x = WEATHER_DRAW_X_START + 4;
            item_y = WEATHER_DRAW_Y_START + WEATHER_DRAW_Y_STEP;
        }
        else
        {
            item_x = item_x + WEATHER_DRAW_X_STEP + 4;
        }
    }
}
