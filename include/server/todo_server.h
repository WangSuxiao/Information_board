#ifndef _TODO_SERVER_H_
#define _TODO_SERVER_H_

#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <Arduino.h>
#include <vector>
#define TIMEZONE 8


void handleUpload(DynamicJsonDocument &doc, ESP8266WebServer &server);

void handleReadAll(DynamicJsonDocument &doc,ESP8266WebServer &server);

#endif
