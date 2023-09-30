/*
 * @Author       : WangSuxiao
 * @Date         : 2023-09-26 09:10:55
 * @LastEditTime : 2023-09-30 16:03:16
 * @Description  : 请求 一言
 * @Tips         :
 */

#include "module/motto.h"


// WiFiClient wifiClient;

String getHito(String type,String key)
{
    Serial.println("--------------- getHito --------------");

    String response = "";

    if (WiFi.status() != WL_CONNECTED)
    {
        return response;
    }

    WiFiClient client;
    HTTPClient http;
    http.begin(client, "www.wangsuxiao.com", 80, "/hitokoto");        // set the server address and path
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // set the content type
    String post_data = "type_=" + type + "&key=" + key;     // set the POST data
    int httpCode = http.POST(post_data);                                 // send the POST request
    if (httpCode > 0)
    {
        response = http.getString();
        Serial.println(httpCode);
        Serial.println(response);
    }
    else
    {
        Serial.println("Error on HTTP request");
    }
    http.end(); // close the connection

    // Serial.print("getLocationByIP response");
    // Serial.println(response);

    return response;
}







// bool getMaxim(String type)
// {

//     WiFiClientSecure client;
//     client.setInsecure();
//     if (!client.connect("v1.hitokoto.cn", 443))
//     {
//         Serial.println("Connection failed");
//         return false;
//     }

//     String url = "/?c=d&encode=text";
//     client.print(String("GET ") + url + " HTTP/1.1\r\n" +
//                  "Host: " + "v1.hitokoto.cn" + "\r\n" +
//                  "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36 Edg/113.0.1774.42\r\n" +
//                  "Connection: close\r\n\r\n");
//     Serial.println("Request sent");

//     while (client.connected())
//     {
//         String line = client.readStringUntil('\n');
//         if (line == "\r")
//         {
//             Serial.println("Headers received");
//             break;
//         }
//     }

//     while (client.available())
//     {
//         String response = client.readStringUntil('\n');
//         Serial.println(response);
//     }

//     Serial.println("Request complete");
//     return true;
// }
