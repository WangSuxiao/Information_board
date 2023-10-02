#ifndef _LBS_H_
#define _LBS_H_

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "config.h"


String urlDecode(String str);
bool getLocationByWIFI(DynamicJsonDocument& doc);
bool getLocationByIP(DynamicJsonDocument& doc);
void lbs(DynamicJsonDocument &doc, City &city);

#endif
