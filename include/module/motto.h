#ifndef _HITOKOTO_H_
#define _HITOKOTO_H_

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
// #include <WiFiClientSecure.h>
#include <ArduinoJson.h>


String getHito(String type, String key);

// bool getLocationByWIFI(DynamicJsonDocument& doc);
// bool getLocationByIP(DynamicJsonDocument& doc);

#endif
