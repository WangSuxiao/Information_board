#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <Arduino.h>

#define TODO_FILE "/todos.txt"

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

struct Todo
{
    uint32_t id;
    time_t start;
    time_t end;
    uint8_t level;
    //   String info;
    uint32_t offset;
};

#define ICON_WEIGHT 32
#define ICON_HEIGHT 32

#define WEATHER_DRAW_X_START 11
#define WEATHER_DRAW_Y_START 140
#define WEATHER_DRAW_X_STEP 60
#define WEATHER_DRAW_Y_STEP 80
#define WEATHER_DRAW_FONT_SIZE 16

#define EEPROMAddressFlag 0
#define EEPROMAddressMode sizeof(int)


#endif
