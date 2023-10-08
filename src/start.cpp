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
// #include "server/webserver.h"
#include "server/todo_server.h"
#include "module/todo.h"

UBYTE *BlackImage;

PAINT_TIME sPaint_time;

bool WIFILOCATION = false;
bool IPLOCATION = false;

// String lon = "";
// String lat = "";
String cityId = "101190405";
// String cityName = "";
// String address = "";
// City city = {
//     "lon",
//     lat,
//     cityId,
//     cityName,
//     address};
City city;
DynamicJsonDocument doc(4096);

Scheduler scheduler;
ESP8266WebServer webserver(80);

// 时间
UBYTE old_min = 0;
void update_time_task()
{
    struct tm timeinfo;
    int time_x = 52;
    if (getLocalTime(&timeinfo))
    {

        sPaint_time.Year = timeinfo.tm_year + 1900;
        sPaint_time.Month = timeinfo.tm_mon;
        sPaint_time.Day = timeinfo.tm_mday;
        sPaint_time.Hour = timeinfo.tm_hour + 8;
        sPaint_time.Min = timeinfo.tm_min;
        sPaint_time.Sec = timeinfo.tm_sec;
        sPaint_time.Week = timeinfo.tm_wday;
        if (sPaint_time.Min != old_min)
        {
            EPD_4IN2_Init_Partial();
            EPD_4IN2_PartialDisplay_step1(120, time_x, 120 + FXLED36.Width * 4.5, time_x + FXLED36.Height, BlackImage);
            Paint_ClearWindows(120, time_x, 120 + FXLED36.Width * 4.5, time_x + FXLED36.Height, WHITE);
            Paint_DrawTime_From_File(120, time_x, &sPaint_time, &FXLED36, WHITE, BLACK);
            EPD_4IN2_PartialDisplay_step2(120, time_x, 120 + FXLED36.Width * 4.5, time_x + FXLED36.Height, BlackImage);
            old_min = sPaint_time.Min;
            // EPD_4IN2_Init_Fast();
            // EPD_4IN2_Display(BlackImage);
            EPD_4IN2_Sleep();
        }
    }
    else
    {
        Serial.println("Failed to obtain time");
    }

    // updataTime(sPaint_time);
    // if (sPaint_time.Min != old_min)
    // {
    //     EPD_4IN2_Init_Partial();
    //     EPD_4IN2_PartialDisplay_step1(120, time_x, 120 + FXLED36.Width * 4.5, time_x + FXLED36.Height, BlackImage);
    //     Paint_ClearWindows(120, time_x, 120 + FXLED36.Width * 4.5, time_x + FXLED36.Height, WHITE);
    //     Paint_DrawTime_From_File(120, time_x, &sPaint_time, &FXLED36, WHITE, BLACK);
    //     EPD_4IN2_PartialDisplay_step2(120, time_x, 120 + FXLED36.Width * 4.5, time_x + FXLED36.Height, BlackImage);
    //     old_min = sPaint_time.Min;
    //     EPD_4IN2_Init_Fast();
    //     EPD_4IN2_Display(BlackImage);
    //     EPD_4IN2_Sleep();
    // }
    // Serial.println("update_time_task");
}

// 实时天气
void update_realweather_task()
{
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.println("===============================================================");
    Serial.println("update_realweather_task");
    String res = requireWeather("/weather/now", city);
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
}

// 未来天气
void update_futureweather_task()
{
    Serial.println("update_futureweather_task");
    EPD_4IN2_Init_Partial();
    std::vector<HourlyWeather> hourlyWeatherData = getWeatherInfo(doc, "/weather/today", city);
    draw_FutureWeather(hourlyWeatherData);
    EPD_4IN2_Init_Fast();
    EPD_4IN2_Display(BlackImage);
    EPD_4IN2_Sleep();
}

UWORD todo_index = 0;
void drawTODO()
{

    int sum = getTODOQuantity();
    Serial.print("条目总和:");
    Serial.println(sum);
    if (todo_index == sum)
    {
        todo_index = 0;
    }
    if (todo_index < sum)
    {
        EPD_4IN2_Init_Fast();
        // Paint_Clear(WHITE);
        Paint_ClearWindows(DRAW_TODO_X_START,DRAW_TODO_Y_START,DRAW_TODO_X_END,DRAW_TODO_Y_END,WHITE);
        Serial.print("开始的索引 :");
        Serial.println(todo_index);
        todo_index = drawTODO_OnePage(&PINGFANG12, &Font24, todo_index);
        EPD_4IN2_Display(BlackImage);
        Serial.print("下一次的索引 :");
        Serial.println(todo_index);
        EPD_4IN2_Sleep();
        // delay(10 * 1000);
    }
    // UWORD index = 0;
    // while (index < sum)
    // {
    //     EPD_4IN2_Init_Fast();
    //     Serial.println("开始的索引 :");
    //     Paint_Clear(WHITE);
    //     Serial.println(index);
    //     index = drawTODO_OnePage(&PINGFANG12, &Font24, index);
    //     EPD_4IN2_Display(BlackImage);
    //     Serial.println("下一次的索引 :");
    //     Serial.println(index);
    //     EPD_4IN2_Sleep();
    //     delay(10 * 1000);
    // }
}

Task update_time(1000 * 1, TASK_FOREVER, &update_time_task);
Task update_realweather(1000 * 60 * 10, TASK_FOREVER, &update_realweather_task);
Task update_futureweather(1000 * 60 * 15, TASK_FOREVER, &update_futureweather_task);
Task update_todo(1000 * 20, TASK_FOREVER, &drawTODO);

void setup()
{
    DEV_Module_Init();         // 文件系统、串口、SPI、GPIO初始化
    BlackImage = init_paint(); // 创建图像缓冲区
    EPD_4IN2_Init_Fast();
    EPD_4IN2_Clear(); // 清屏
    DEV_Delay_ms(500);

    // String line = "\t12\t3456\t7\t89";
    // // Serial.println(String(line.indexOf("\t")));
    // // Serial.println(String(line.indexOf("\t", 5)));
    // // Serial.println(String(line.indexOf("\t", 1)));
    // // Serial.println(String(line.indexOf("\t", 0)));


    // 网络连接
    wifiConnect();
    // wifiConfig(BlackImage);

    // 初始化系统时间
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    // 定位服务
    lbs(doc, city);

    // 绘制基本框架
    draw_Frame(BlackImage, city.cityName);
    EPD_4IN2_Sleep();
    udp.begin(1127);
    delay(500);

    // 开启子任务
    scheduler.init();
    scheduler.addTask(update_time);
    scheduler.addTask(update_realweather);
    scheduler.addTask(update_futureweather);
    scheduler.addTask(update_todo);

    update_time.enable();
    update_realweather.enable();
    update_futureweather.enable();
    update_todo.enable();

    // 开启网络服务器
    webserver.on("/upload", HTTP_ANY, []()
                 { handleUpload(doc, webserver); });
    webserver.begin();
}

void loop()
{
    scheduler.execute();
    webserver.handleClient();
}
