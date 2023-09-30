#ifndef _MODULE_WEATHER_H_
#define _MODULE_WEATHER_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "board/DEV_Config.h"
#include "board/EPD.h"
#include "board/GUI_Paint.h"
#include "config.h"

#define AUTHORIZE_KEY  "SkqZeGILxdGHyoflbhcgddcWykFgbZlh"

String getCityCode();
String requireWeather(String path, City city);

std::vector<HourlyWeather> getWeatherInfo(DynamicJsonDocument &doc, String path, City city);
void draw_RealtimeWeather(String weather, String temp, String humidity, int icon);
void draw_FutureWeather(std::vector<HourlyWeather> &hourlyWeatherData);
#endif
