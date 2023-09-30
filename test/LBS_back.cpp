// // #include <Arduino.h>

// // #include <ESP8266WiFi.h>
// // #include <ESP8266HTTPClient.h>
// // #include <WiFiClient.h>
// // #include <ArduinoJson.h>

// #include "func/LBS.h"

// bool getLocationByWIFI(DynamicJsonDocument &doc)
// {
//     Serial.println("---------- getLocationByWIFI ------------");
//     if (WiFi.status() != WL_CONNECTED)
//     {
//         // 没有连接网络
//         return false;
//     }
//     // 扫描网络
//     Serial.println("Scanning WiFi networks...");
//     int numNetworks = WiFi.scanNetworks();
//     if (numNetworks == 0)
//     {
//         Serial.println("No WiFi networks found");
//         return false;
//     }

//     // 构造请求参数，最多保留15个WIFI信息
//     String wlParam = "";
//     // numNetworks = numNetworks < 15 ? numNetworks : 15;
//     for (int i = 0; i < numNetworks; i++)
//     {
//         if (i > 0)
//         {
//             wlParam += ";";
//         }
//         wlParam += WiFi.BSSIDstr(i) + ",";
//         wlParam += String(WiFi.RSSI(i));
//     }

//     HTTPClient httpClient;
//     WiFiClient client;
//     String response = "";
//     httpClient.begin(client, "http://api.cellocation.com:84/loc/?wl="+wlParam+"&coord=gcj02&output=json");
//     int httpCode = httpClient.GET();
//     if (httpCode == HTTP_CODE_OK)
//     {
//         response = httpClient.getString();
//     }else{
//         Serial.println(httpCode);
//         Serial.println("http://api.cellocation.com:84/loc/?wl="+wlParam+"&coord=gcj02&output=json");
//     }
//     Serial.print("httpClient.headers()  :  ");
//     Serial.println(httpClient.headers());
//     httpClient.end();

//     Serial.print(response);


//     // // 发送HTTP请求
//     // Serial.println("Sending HTTP request...");
//     // WiFiClient client;
//     // if (!client.connect("api.cellocation.com", 84))
//     // {
//     //     Serial.println("Connection failed");
//     //     return false;
//     // }
//     // client.print("GET /loc/?wl=");
//     // client.print(wlParam);
//     // client.print("&coord=gcj02&output=json");
//     // client.println(" HTTP/1.1");
//     // client.println("Host: api.cellocation.com");
//     // client.println("Connection: close");
//     // client.println();

//     // // 读取HTTP响应
//     // // String response = "";
//     // // while (client.connected())
//     // // {
//     // //     String line = client.readStringUntil('\n');
//     // //     if (line == "\r")
//     // //     {
//     // //         break;
//     // //     }
//     // // }

//     // String response = "";
//     // while (client.connected())
//     // {
//     //     String line = client.readStringUntil('\n');
//     //     if (line == "\r")
//     //     {
//     //         Serial.println("headers received");

//     //         break;
//     //     }
//     //     Serial.println("line:");
//     //     Serial.println(line);
//     //     Serial.println();
//     // }

//     // while (client.available())
//     // {
//     //     response += client.readString();
//     // }

//     // Serial.print(response);

//     // 解析JSON数据
//     // DynamicJsonDocument doc(1024);
//     DeserializationError error = deserializeJson(doc, response);
//     if (error)
//     {
//         Serial.print("getLocationByWIFI deserializeJson() failed: ");

//         Serial.println(error.c_str());

//         return false;
//     }
//     int errcode = doc["errcode"];
//     if (errcode != 0)
//         return false;

//     return true;
// }

// // const String url = "/location/ip?coor=gcj02&ak=YOUR_AK";

// bool getLocationByIP(DynamicJsonDocument &doc)
// {
//     Serial.println("---------------getLocationByIP--------------");
//     if (WiFi.status() != WL_CONNECTED)
//     {
//         return false;
//     }

//     WiFiClient client;
//     if (!client.connect("api.map.baidu.com", 80))
//     {
//         Serial.println("connection failed");
//         return false;
//     }

//     client.print("GET /location/ip?coor=gcj02&ak=I54VWZLG73NuKjVjC7gVKsH9FY7CCy6x");
//     client.println(" HTTP/1.1");
//     client.println("Host: api.map.baidu.com");
//     client.println("Connection: keep-alive");
//     client.println();

//     String response = "";
//     bool headersReceived = false;
//     while (client.connected())
//     {
//         String line = client.readStringUntil('\n');
//         if (line == "\r")
//         {
//             Serial.println("headers received");

//             break;
//         }
//         Serial.println("line");
//         Serial.println(line);
//         Serial.println();
//     }

//     if (headersReceived)
//     {
//         while (client.available())
//         {
//             response += client.readStringUntil('\n');
//         }
//     }

//     Serial.print("getLocationByIP response");
//     Serial.println(response);
//     // Serial.println("closing connection");
//     DeserializationError error = deserializeJson(doc, response);
//     if (error)
//     {
//         Serial.print("getLocationByIP deserializeJson() failed: ");
//         Serial.println(error.c_str());
//         return false;
//     }
//     int errcode = doc["status"];
//     if (errcode == 0)
//         return true;
//     return false;

//     // // 从JsonDocument对象中获取所需的数据
//     // String address = jsonDoc["address"];                            // 获取详细地址信息
//     // String city = jsonDoc["content"]["address_detail"]["city"];     // 获取城市信息
//     // float longitude = jsonDoc["content"]["point"]["x"].as<float>(); // 获取经度
//     // float latitude = jsonDoc["content"]["point"]["y"].as<float>();  // 获取纬度

//     // // 输出所获取的数据
//     // Serial.print("Address: ");
//     // Serial.println(address);
//     // Serial.print("City: ");
//     // Serial.println(city);
//     // Serial.print("Longitude: ");
//     // Serial.println(longitude);
//     // Serial.print("Latitude: ");
//     // Serial.println(latitude);
// }
