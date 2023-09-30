/*
 * @Author       : WangSuxiao
 * @Date         : 2023-09-26 16:35:06
 * @LastEditTime : 2023-09-30 18:10:45
 * @Description  : 初始化图形区域，绘制基本框架
 * @Tips         :
 */

#include "ui/drawBase.h"


/**
 * @description: 初始化图像缓冲区
 * @return {Any}
 */
UBYTE * init_paint()
{
    UWORD imageSize = ((EPD_4IN2_WIDTH % 8 == 0) ? (EPD_4IN2_WIDTH / 8) : (EPD_4IN2_WIDTH / 8 + 1)) * EPD_4IN2_HEIGHT;
    UBYTE *BlackImage;
    if ((BlackImage = (UBYTE *)malloc(imageSize)) == NULL)
    {
        while (1)
        {
            Serial.println("Failed to apply for black memory...\r\n");
            delay(5000);
        }
    }
    Paint_NewImage(BlackImage, EPD_4IN2_WIDTH, EPD_4IN2_HEIGHT, 0, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    return BlackImage;
}

/**
 * @description: 绘制基础框架（地点、IP）
 * @param {UBYTE} *BlackImage :
 * @param {String} city :
 */
void draw_Frame(UBYTE *BlackImage, String city)
{
    EPD_4IN2_Init_Fast();
    Paint_Clear(WHITE);
    DEV_Delay_ms(500);
    Paint_DrawLine(0, 25, 400, 25, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(0, 90, 400, 90, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(0, 128, 400, 128, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Serial.println();
    Serial.println(city.length());
    Serial.println(400 - city.length() / 3 * 16 - 10 - 22);
    Serial.println();
    if (!city.isEmpty())
    {
        Paint_DrawString_CN_From_File(400 - city.length() / 3 * 16 - 10, 2, city.c_str(), &PINGFANG12, BLACK, WHITE);
    }
    else
    {
        Serial.print("city isEmpty");
        delay(1000);
    }

    Paint_DrawString_EN(32, 12, WiFi.localIP().toString().c_str(), &Font12, BLACK, WHITE);
    EPD_4IN2_Display(BlackImage);
    DEV_Delay_ms(500);
    delay(500);
}


/**
 * @description: 绘制实时时间
 * @param {UBYTE} *BlackImage :
 * @param {PAINT_TIME} sPaint_time :
 * @return {Any}
 */
void draw_Time(UBYTE *BlackImage, PAINT_TIME sPaint_time)
{
    int time_x = 52;
    EPD_4IN2_Init_Partial();
    EPD_4IN2_PartialDisplay_step1(120, time_x, 120 + FXLED36.Width * 4.5, time_x + FXLED36.Height, BlackImage);
    Paint_ClearWindows(120, time_x, 120 + FXLED36.Width * 4.5, time_x + FXLED36.Height, WHITE);
    Paint_DrawTime_From_File(120, time_x, &sPaint_time, &FXLED36, WHITE, BLACK);
    EPD_4IN2_PartialDisplay_step2(120, time_x, 120 + FXLED36.Width * 4.5, time_x + FXLED36.Height, BlackImage);
    EPD_4IN2_Init_Fast();
    EPD_4IN2_Display(BlackImage);
    EPD_4IN2_Sleep();
}

// /**
//  * @description: 绘制实时天气
//  * @param {String} weather : 天气情况描述
//  * @param {String} temp : 温度
//  * @param {String} humidity : 湿度
//  * @param {int} icon : 天气图标
//  * @return {Any}
//  */
// void draw_RealtimeWeather(String weather, String temp, String humidity, int icon)
// {
//     Paint_DrawIcon_From_File(icon, 8, 32, 32, 32, WHITE, BLACK); // 数据集中0为黑色 ; 而板子0定义为白色
//     Paint_DrawString_CN_From_File(52, 40, weather.c_str(), &PINGFANG16, BLACK, WHITE);
//     int Xstart = 11;
//     int Ystart = 70;
//     int index = 0;
//     for (uint8_t i = 0; i < temp.length(); i++)
//     {
//         Paint_DrawChar(Xstart + Weather22.Width * i, Ystart, temp[i], &Weather22, BLACK, WHITE, temp[i] - '0');
//         index = i;
//     }
//     Paint_DrawChar(Xstart + Weather22.Width * index + 7, Ystart, '°', &Weather22, BLACK, WHITE, 12);
//     Paint_DrawChar(Xstart + Weather22.Width * index + 12, Ystart, 'C', &Weather22, BLACK, WHITE, 13);
//     for (uint8_t i = 0; i < humidity.length(); i++)
//     {
//         index = index + i;
//         Paint_DrawChar(Xstart + Weather22.Width * (index + 1) + 24, Ystart, humidity[i], &Weather22, BLACK, WHITE, humidity[i] - '0');
//     }
//     Paint_DrawChar(Xstart + Weather22.Width * (index + 2) + 24, Ystart, '%', &Weather22, BLACK, WHITE, 10);
// }


// /**
//  * @description: 绘制未来天气
//  * @param {vector<HourlyWeather>} &hourlyWeatherData :
//  * @return {Any}
//  */
// void draw_FutureWeather(std::vector<HourlyWeather> &hourlyWeatherData)
// {
//     String time;
//     String temp;
//     int icon;
//     String text;
//     String pop;
//     String humidity;
//     String precip;

//     int loopLimit = hourlyWeatherData.size() < 12 ? hourlyWeatherData.size() : 12;

//     Paint_ClearWindows(WEATHER_DRAW_X_START,
//                        WEATHER_DRAW_Y_START,
//                        WEATHER_DRAW_X_START + 6 * WEATHER_DRAW_X_STEP,
//                        WEATHER_DRAW_Y_START + 2 * WEATHER_DRAW_Y_STEP,
//                        WHITE);
//     int item_x = WEATHER_DRAW_X_START;
//     int item_y = WEATHER_DRAW_Y_START;
//     uint8_t margin = uint8_t((WEATHER_DRAW_Y_STEP - ICON_HEIGHT - WEATHER_DRAW_FONT_SIZE * 2) / 4);
//     uint8_t icon_pad = 10;

//     for (int i = 0; i < loopLimit; i++)
//     {
//         HourlyWeather hourlyItem = hourlyWeatherData[i];

//         // time = hourlyItem["fxTime"].as<String>();
//         // temp = hourlyItem["temp"].as<String>();
//         // icon = hourlyItem["icon"].as<int>();
//         // text = hourlyItem["text"].as<String>();
//         // pop = hourlyItem["pop"].as<String>();
//         // humidity = hourlyItem["humidity"].as<String>();
//         // precip = hourlyItem["precip"].as<String>();
//         time = hourlyItem.fxTime;
//         temp = hourlyItem.temp;
//         icon = hourlyItem.icon;
//         text = hourlyItem.text;
//         pop = hourlyItem.pop;
//         humidity = hourlyItem.humidity;
//         precip = hourlyItem.precip;

//         Serial.println("绘制图标 : ");
//         Paint_DrawIcon_From_File(icon, item_x + icon_pad, item_y, ICON_WEIGHT, ICON_HEIGHT, WHITE, BLACK);
//         Serial.println("-------降水概率------");
//         Paint_DrawWeatherInfo(item_x, item_y + ICON_HEIGHT + margin, temp, humidity, &Time14, BLACK, WHITE);
//         Paint_DrawTime(item_x + 8, item_y + ICON_HEIGHT + WEATHER_DRAW_FONT_SIZE + margin, time, &Time14, BLACK, WHITE);

//         if ((i + 1) % 6 == 0)
//         {
//             item_x = WEATHER_DRAW_X_START + 4;
//             item_y = WEATHER_DRAW_Y_START + WEATHER_DRAW_Y_STEP;
//         }
//         else
//         {
//             item_x = item_x + WEATHER_DRAW_X_STEP + 4;
//         }
//     }
// }
