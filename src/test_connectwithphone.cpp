/*
 * @Author       : WangSuxiao
 * @Date         : 2023-10-02 17:17:34
 * @LastEditTime : 2023-10-03 17:05:06
 * @Description  : 测试与手机的通信
 * @Tips         :
 */
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
#include "server/todo_server.h"
// #include "server/webserver.h"

UBYTE *BlackImage;

PAINT_TIME sPaint_time;

bool WIFILOCATION = false;
bool IPLOCATION = false;

String cityId = "101190405";

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

            EPD_4IN2_Sleep();
        }
    }
    else
    {
        Serial.println("Failed to obtain time");
    }
}

Task update_time(1000 * 1, TASK_FOREVER, &update_time_task);

void setup()
{
    DEV_Module_Init();         // 文件系统、串口、SPI、GPIO初始化
    BlackImage = init_paint(); // 创建图像缓冲区
    EPD_4IN2_Init_Fast();
    EPD_4IN2_Clear(); // 清屏
    DEV_Delay_ms(500);
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
    update_time.enable();


    webserver.on("/uploadjson", HTTP_ANY, []()
                 { handleUpload(doc,webserver); });
    webserver.on("/readall", HTTP_ANY, []()
                 { handleReadAll(doc,webserver); });
    webserver.begin();
}

void loop()
{
    scheduler.execute();
    webserver.handleClient();
}
