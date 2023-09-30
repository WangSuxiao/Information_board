#ifndef _WIFICONFIG_H_
#define _WIFICONFIG_H_
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#define AP_SSID "EPAPERWiFiAP"
#define AP_PASSWORD "123456789"


// AP 名称和密码
// extern const char *ssid = "WiFiAP";
// extern const char *password = "123456789";

void wifiConfig(uint8_t *BlackImage);
void wifiConnect();

#endif
