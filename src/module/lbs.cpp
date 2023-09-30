/*
 * @Author       : WangSuxiao
 * @Date         : 2023-09-26 09:10:55
 * @LastEditTime : 2023-09-30 15:57:49
 * @Description  : 定位服务
 * @Tips         :
 */
#include "module/lbs.h"


/**
 * @description: 用于将unicode编码转为UTF8编码
 * @param {String} str : unicode编码
 */
String urlDecode(String str)
{
    String decoded = "";
    char c;
    int len = str.length();
    for (int i = 0; i < len; i++)
    {
        if (str[i] == '+')
        {
            decoded += ' ';
        }
        else if (str[i] == '%')
        {
            if (isxdigit(str[i + 1]) && isxdigit(str[i + 2]))
            {
                c = strtoul(str.substring(i + 1, i + 3).c_str(), NULL, 16);
                decoded += c;
                i += 2;
            }
            else
            {
                decoded += '%';
            }
        }
        else
        {
            decoded += str[i];
        }
    }
    return decoded;
}

bool getLocationByWIFI(DynamicJsonDocument &doc)
{
    // Serial.println("---------- getLocationByWIFI ------------");
    if (WiFi.status() != WL_CONNECTED)
    {
        return false;
    }
    Serial.println("Scanning WiFi networks...");
    int numNetworks = WiFi.scanNetworks();
    if (numNetworks == 0)
    {
        Serial.println("No WiFi networks found");
        return false;
    }

    String wlParam = "";
    for (int i = 0; i < numNetworks; i++)
    {
        if (i > 0)
        {
            wlParam += ";";
        }
        wlParam += WiFi.BSSIDstr(i) + ",";
        wlParam += String(WiFi.RSSI(i));
    }

    HTTPClient httpClient;
    WiFiClient client;
    String response = "";
    httpClient.begin(client, "http://api.cellocation.com:84/loc/?wl=" + wlParam + "&coord=gcj02&output=json");
    int httpCode = httpClient.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        response = httpClient.getString();
    }
    else
    {
        Serial.println(httpCode);
        Serial.println("http://api.cellocation.com:84/loc/?wl=" + wlParam + "&coord=gcj02&output=json");
    }
    httpClient.end();

    // Serial.print(response);

    DeserializationError error = deserializeJson(doc, response);
    if (error)
    {
        Serial.print("getLocationByWIFI deserializeJson() failed: ");

        Serial.println(error.c_str());

        return false;
    }
    int errcode = doc["errcode"];
    if (errcode != 0)
        return false;

    return true;
}


bool getLocationByIP(DynamicJsonDocument &doc)
{
    Serial.println("---------------getLocationByIP--------------");
    if (WiFi.status() != WL_CONNECTED)
    {
        return false;
    }

    HTTPClient httpClient;
    WiFiClient client;
    String response = "";
    httpClient.begin(client, "http://api.map.baidu.com/location/ip?coor=gcj02&ak=I54VWZLG73NuKjVjC7gVKsH9FY7CCy6x");
    int httpCode = httpClient.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        response = httpClient.getString();
    }
    else
    {
        Serial.println(httpCode);
        Serial.println("https://api.map.baidu.com/location/ip?coor=gcj02&ak=I54VWZLG73NuKjVjC7gVKsH9FY7CCy6x");
    }
    httpClient.end();

    // Serial.println(response);
    DeserializationError error = deserializeJson(doc, response);
    if (error)
    {
        Serial.print("getLocationByIP deserializeJson() failed: ");
        Serial.println(error.c_str());
        return false;
    }
    int errcode = doc["status"];
    if (errcode == 0)
        return true;
    return false;
}

