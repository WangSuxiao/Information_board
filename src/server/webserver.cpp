/*
 * @Author       : WangSuxiao
 * @Date         : 2023-09-26 09:10:55
 * @LastEditTime : 2023-10-02 21:55:10
 * @Description  : 开启网络服务器，用于和终端设备的通信
 * @Tips         :
 *      1. 添加、删除、修改单片机中缓存的数据
 */
#include "server/webserver.h"
#include "config.h"

#include <TimeLib.h>
#include <vector>


std::vector<Todo> todos;


void handleTODO(ESP8266WebServer &server, DynamicJsonDocument &doc)
{

    String json_data = server.arg("plain");
    DeserializationError error = deserializeJson(doc, json_data);
    if (error == DeserializationError::Ok)
    {
        Serial.println("Received JSON Data:");

        String method = doc["method"];
        String key = doc["key"];
        int id = doc["data"]["id"];
        String start = doc["data"]["start"];
        String end = doc["data"]["end"];
        int level = doc["data"]["level"];
        String info = doc["data"]["info"];

        if(method.equals("insert")){
        }else if (method.equals("delete"))
        {
        }else if (method.equals("update"))
        {
        }
        Serial.println("Method: " + method);
        Serial.println("Key: " + key);
        Serial.println("ID: " + String(id));
        Serial.println("Start: " + start);
        Serial.println("End: " + end);
        Serial.println("Level: " + String(level));
        Serial.println("Info: " + info);

        // serializeJsonPretty(doc, Serial);
        server.send(200, "text/plain", "JSON received and parsed successfully.");
    }
    else
    {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        server.send(400, "text/plain", "Failed to parse JSON");
    }

    // if (server.hasArg("start") && server.hasArg("end") && server.hasArg("level") && server.hasArg("info") && server.hasArg("key"))
    // {
    // start : 开始时间
    // end   : 截止时间
    // level : 重要性
    // info  : 描述
    // key   : 密钥
    // String key = server.arg("key");
    // if (key == "123456")
    // {
    //     // unsigned long start = server.arg("start").toInt() + TIMEZONE * SECS_PER_HOUR;

    //     // time_t timestamp = (time_t)start;

    //     // // 使用strftime函数将time_t格式化为"年-月-日 时:分"的字符串
    //     // char formattedStartTime[20]; // 足够大的字符数组来容纳格式化后的字符串
    //     // strftime(formattedStartTime, sizeof(formattedStartTime), "%y-%m-%d %H:%M", localtime(&timestamp));
    //     // Serial.print(formattedStartTime);

    //     // tmElements_t startTime;
    //     // breakTime(start, startTime);

    //     // int startYear = startTime.Year + 1970; // Time库使用相对于1970年的年份偏移量
    //     // int startMonth = startTime.Month;
    //     // int startDay = startTime.Day;
    //     // int startHour = startTime.Hour;
    //     // int startMinute = startTime.Minute;
    //     // int startSecond = startTime.Second;

    //     // server.send(200, "text/plain", formattedStartTime + String(startYear) + " / " + String(startMonth) + " / " + String(startDay) + "  " + String(startHour) + " : " + String(startMinute));
    //     server.send(200, "text/plain", "formattedSt");
    // }
    // else
    // {
    //     server.send(400, "text/plain", "Bad Request");
    // }
    // }
}

void handleCountdown(ESP8266WebServer &server)
{
    if (server.hasArg("data") && server.hasArg("msg") && server.hasArg("key"))
    {
        String key = server.arg("key");
        if (key == "123456")
        {
            String data = server.arg("data");
            String msg = server.arg("msg");
            for (int a = 0; a < 10; a++)
            {
                Serial.print(data);
                Serial.print(msg);
            }
            server.send(200, "text/plain", "SUCCESS");
        }
        else
        {
            server.send(400, "text/plain", "Bad Request");
        }
    }
}
