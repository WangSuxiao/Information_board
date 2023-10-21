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
#include "config.h"
#include "module/todoManager.h"


void handleUpload(DynamicJsonDocument &doc, ESP8266WebServer &server, TodoManager &todoManager);
void handleReadAll(DynamicJsonDocument &doc, ESP8266WebServer &server, TodoManager &todoManager);

#endif
