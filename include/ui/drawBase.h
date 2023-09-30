/*****************************************************************************
* | File      	:	Debug.h
* | Author      :   Waveshare team
* | Function    :	debug with printf
* | Info        :
*   Image scanning
*      Please use progressive scanning to generate images or fonts
*----------------
* |	This version:   V1.0
* | Date        :   2018-01-11
* | Info        :   Basic version
*
******************************************************************************/
#ifndef _DRAW_BASE_H_
#define _DRAW_BASE_H_

#include "board/DEV_Config.h"
#include "board/EPD.h"
#include "board/GUI_Paint.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "config.h"


// #define WEATHER_DRAW_X_START 11
// #define WEATHER_DRAW_Y_START 140
// #define WEATHER_DRAW_X_STEP 60
// #define WEATHER_DRAW_Y_STEP 80
// #define WEATHER_DRAW_FONT_SIZE 16

// #define ICON_WEIGHT 32
// #define ICON_HEIGHT 32



UBYTE * init_paint();       // 初始化图像缓冲区

void draw_Frame(UBYTE *BlackImage, String city);
void draw_Time(UBYTE *BlackImage, PAINT_TIME sPaint_time);
// void draw_RealtimeWeather(String weather, String temp, String humidity, int icon);
// void draw_FutureWeather(std::vector<HourlyWeather> &hourlyWeatherData);

#endif

