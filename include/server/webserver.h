#ifndef _WEB_SERVER_H_
#define _WEB_SERVER_H_

#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#define TIMEZONE 8
void handleTODO(ESP8266WebServer &server,DynamicJsonDocument &doc);




#endif
