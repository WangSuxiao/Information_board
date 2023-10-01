#ifndef _TIME_UTIL_H_
#define _TIME_UTIL_H_

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "board/GUI_Paint.h"

extern WiFiUDP udp;
// extern PAINT_TIME sPaint_time;

void updataTime(PAINT_TIME &sPaint_time);


#endif
