#include "board/DEV_Config.h"
#include "board/EPD.h"
#include "board/GUI_Paint.h"
#include <stdlib.h>

#include <TaskScheduler.h>

#include "ui/drawBase.h"
#include "module/lbs.h"
#include "module/weather.h"
#include "module/timeUtil.h"
#include <ArduinoJson.h>
#include "module/motto.h"
#include "resource/icon.h"
#include "module/wificonfig.h"
#include "server/webserver.h"

UBYTE *BlackImage;

PAINT_TIME sPaint_time;

bool WIFILOCATION = false;
bool IPLOCATION = false;

String lon = "";
String lat = "";
String cityId = "101190405";
String cityName = "";
String address = "";
City city ={
        lon,
        lat,
        cityId,
        cityName,
        address};
// City city = {
//     "longitude_value",
//     "latitude_value",
//     "cityid_value",
//     "city_name_value"
// };
DynamicJsonDocument doc(4096);

// /**
//  * @description: 画布/缓冲区初始化
//  * @return {Any}
//  */
// void init_paint()
// {
//     UWORD imageSize = ((EPD_4IN2_WIDTH % 8 == 0) ? (EPD_4IN2_WIDTH / 8) : (EPD_4IN2_WIDTH / 8 + 1)) * EPD_4IN2_HEIGHT;
//     if ((BlackImage = (UBYTE *)malloc(imageSize)) == NULL)
//     {
//         while (1)
//         {
//             Serial.println("Failed to apply for black memory...\r\n");
//             delay(5000);
//         }
//     }
//     Paint_NewImage(BlackImage, EPD_4IN2_WIDTH, EPD_4IN2_HEIGHT, 0, WHITE);
//     Paint_SelectImage(BlackImage);
//     Paint_Clear(WHITE);
// }

// /**
//  * @description: 绘制框架 ： 线宽、IP、地址
//  * @return {Any}
//  */
// void draw_fram()
// {
//     EPD_4IN2_Init_Fast();
//     Paint_Clear(WHITE);
//     DEV_Delay_ms(500);
//     Paint_DrawLine(0, 25, 400, 25, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//     Paint_DrawLine(0, 90, 400, 90, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//     Paint_DrawLine(0, 128, 400, 128, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
//     Serial.println();
//     Serial.println(city.length());
//     Serial.println(400 - city.length() / 3 * 16 - 10 - 22);
//     Serial.println();
//     if (!city.isEmpty())
//     {
//         Paint_DrawString_CN_From_File(400 - city.length() / 3 * 16 - 10, 2, city.c_str(), &PINGFANG12, BLACK, WHITE);
//     }
//     else
//     {
//         Serial.print("city isEmpty");
//         delay(1000);
//     }

//     Paint_DrawString_EN(32, 12, WiFi.localIP().toString().c_str(), &Font12, BLACK, WHITE);

//     City city;
//     city.cityid = cityid;
//     city.lon = lon;
//     city.lat = lat;
//     String res = requireWeather("/weather/today", city);
//     DeserializationError resultCode = deserializeJson(doc, res);
//     if (resultCode == DeserializationError::Ok)
//     {
//         String temp = doc["temp"].as<String>();
//         String city = doc["city"].as<String>();
//         String wind_d = doc["wind_d"].as<String>();
//         String wind_s = doc["wind_s"].as<String>();
//         String humidity = doc["humidity"].as<String>();
//         String air_press = doc["air_press"].as<String>();
//         String weather = doc["weather"].as<String>();
//         String source = doc["source"].as<String>();
//         int icon = doc["icon"].as<int>();
//         draw_RealtimeWeather(weather, temp, humidity, icon);
//     }

//     EPD_4IN2_Display(BlackImage);
//     DEV_Delay_ms(500);
//     delay(500);
// }

// void update_time(void* parameter) {
//   while (true) {
//     // Your task 1 code here
//     // ...

//     // Delay to avoid continuous processing
//     delay(1000);
//   }
// }

Scheduler scheduler;
ESP8266WebServer webserver(80);

// 时间
UBYTE old_min = 0;
void update_time_task()
{

    int time_x = 52;
    updataTime();
    if (sPaint_time.Min != old_min)
    {
        EPD_4IN2_Init_Partial();
        EPD_4IN2_PartialDisplay_step1(120, time_x, 120 + FXLED36.Width * 4.5, time_x + FXLED36.Height, BlackImage);
        Paint_ClearWindows(120, time_x, 120 + FXLED36.Width * 4.5, time_x + FXLED36.Height, WHITE);
        Paint_DrawTime_From_File(120, time_x, &sPaint_time, &FXLED36, WHITE, BLACK);
        EPD_4IN2_PartialDisplay_step2(120, time_x, 120 + FXLED36.Width * 4.5, time_x + FXLED36.Height, BlackImage);
        old_min = sPaint_time.Min;
        // TODO :
        // draw_12h_weather(doc, lon, lat, cityid);

        EPD_4IN2_Init_Fast();
        EPD_4IN2_Display(BlackImage);
        EPD_4IN2_Sleep();
    }
    Serial.println("update_time_task");
}

// 实时天气
void update_realweather_task()
{
    Serial.println("===============================================================");
    Serial.println("update_realweather_task");
    City city;
    city.cityId = cityId;
    city.lon = lon;
    city.lat = lat;
    String res = requireWeather("/weather/now", city);
    Serial.println(res);
    DeserializationError resultCode = deserializeJson(doc, res);
    if (resultCode == DeserializationError::Ok)
    {
        String temp = doc["temp"].as<String>();
        String city = doc["city"].as<String>();
        String wind_d = doc["wind_d"].as<String>();
        String wind_s = doc["wind_s"].as<String>();
        String humidity = doc["humidity"].as<String>();
        String air_press = doc["air_press"].as<String>();
        String weather = doc["weather"].as<String>();
        String source = doc["source"].as<String>();
        int icon = doc["icon"].as<int>();
        Serial.println(weather);
        Serial.println(temp);
        Serial.println(humidity);
        Serial.println(icon);
        draw_RealtimeWeather(weather, temp, humidity, icon);
    }
    EPD_4IN2_Init_Fast();
    EPD_4IN2_Display(BlackImage);
    EPD_4IN2_Sleep();
    Serial.println("===============================================================");
}

// 未来天气
void update_futureweather_task()
{
    Serial.println("update_futureweather_task");
    EPD_4IN2_Init_Partial();
    std::vector<HourlyWeather> hourlyWeatherData = getWeatherInfo(doc,"/weather/today", city);
    draw_FutureWeather(hourlyWeatherData);
    EPD_4IN2_Init_Fast();
    EPD_4IN2_Display(BlackImage);
    EPD_4IN2_Sleep();
}

Task update_time(1000 * 5, TASK_FOREVER, &update_time_task);
Task update_realweather(1000 * 10 , TASK_FOREVER, &update_realweather_task);
Task update_futureweather(1000 * 20, TASK_FOREVER, &update_futureweather_task);

void setup()
{
    DEV_Module_Init(); // 文件系统、串口、SPI、GPIO初始化

    BlackImage = init_paint(); // 创建图像缓冲区

    // Serial.print("setup : GetHito Module Start");
    // // 一言
    // for(int i=0; i<10; i++ ){
    //     getHito("d","SkqZeGILxdGHyoflbhcgddcWykFgbZlh");
    //     delay(1000);
    // }
    // delay(2000);

    // 墨水屏模块初始化并清屏
    EPD_4IN2_Init_Fast();
    EPD_4IN2_Clear(); // 清屏
    DEV_Delay_ms(500);

    // wifiConnect();
    wifiConfig(BlackImage);

    // LBS : IP定位
    Serial.println("setup : Get Location By IP");
    IPLOCATION = getLocationByIP(doc);
    if (IPLOCATION)
    {
        lon = doc["content"]["point"]["x"].as<String>();
        lat = doc["content"]["point"]["y"].as<String>();
        cityName = doc["content"]["address_detail"]["city"].as<String>();
        if (cityName.isEmpty())
        {
            cityName = doc["content"]["address"].as<String>();
        }
        Serial.println(lon);
        Serial.println(lat);
        Serial.println(cityName);
        cityName.replace("\\u", "%u");
        cityName = urlDecode(cityName);
    }
    delay(2000);

    // LBS : WIFI定位
    Serial.println("setup : Get Location By WIFI");
    WIFILOCATION = getLocationByWIFI(doc);
    if (WIFILOCATION)
    {
        lat = doc["lat"].as<String>();
        lon = doc["lon"].as<String>();
        address = doc["address"].as<String>();
    }
    Serial.println("---------------Location Info--------------");
    Serial.println(lon);
    Serial.println(lat);
    Serial.println(cityName);
    Serial.println(address);
    Serial.println("------------------------------------------");

    draw_Frame(BlackImage, cityName);
    // TODO :
    // draw_real_weather(doc, lon, lat, cityid);

    printf("Goto Sleep...\r\n");
    EPD_4IN2_Sleep();
    udp.begin(1127);
    delay(500);

    Serial.println("scheduler init ");
    scheduler.init();
    scheduler.addTask(update_time);
    scheduler.addTask(update_realweather);
    scheduler.addTask(update_futureweather);
    update_time.enable();
    update_realweather.enable();
    update_futureweather.enable();

    webserver.on("/todo", HTTP_ANY, []()
                 { handleTODO(webserver); });
    webserver.begin();
}

void loop()
{
    scheduler.execute();
    webserver.handleClient();
}
