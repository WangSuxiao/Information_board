#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <Arduino.h>

struct HourlyWeather
{
    String fxTime;
    String temp;
    int icon;
    String text;
    String pop;
    String humidity;
    String precip;
};

struct City
{
    String lon;
    String lat;
    String cityId;
    String cityName;
    String address;
};


#define ICON_WEIGHT 32
#define ICON_HEIGHT 32

#define WEATHER_DRAW_X_START 11
#define WEATHER_DRAW_Y_START 140
#define WEATHER_DRAW_X_STEP 60
#define WEATHER_DRAW_Y_STEP 80
#define WEATHER_DRAW_FONT_SIZE 16

#endif
