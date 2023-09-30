// #include "func/LBS.h"

// bool getLocationByWIFI(DynamicJsonDocument &doc)
// {
//     Serial.println("---------- getLocationByWIFI ------------");
//     if (WiFi.status() != WL_CONNECTED)
//     {
//         return false;
//     }
//     Serial.println("Scanning WiFi networks...");
//     int numNetworks = WiFi.scanNetworks();
//     if (numNetworks == 0)
//     {
//         Serial.println("No WiFi networks found");
//         return false;
//     }

//     String wlParam = "";
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
//     httpClient.begin(client, "http://api.cellocation.com:84/loc/?wl=" + wlParam + "&coord=gcj02&output=json");
//     int httpCode = httpClient.GET();
//     if (httpCode == HTTP_CODE_OK)
//     {
//         response = httpClient.getString();
//     }
//     else
//     {
//         Serial.println(httpCode);
//         Serial.println("http://api.cellocation.com:84/loc/?wl=" + wlParam + "&coord=gcj02&output=json");
//     }
//     httpClient.end();

//     Serial.print(response);

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


// bool getLocationByIP(DynamicJsonDocument &doc)
// {
//     Serial.println("---------------getLocationByIP--------------");
//     if (WiFi.status() != WL_CONNECTED)
//     {
//         return false;
//     }

//     HTTPClient httpClient;
//     WiFiClient client;
//     String response = "";
//     httpClient.begin(client, "http://api.map.baidu.com/location/ip?coor=gcj02&ak=I54VWZLG73NuKjVjC7gVKsH9FY7CCy6x");
//     int httpCode = httpClient.GET();
//     if (httpCode == HTTP_CODE_OK)
//     {
//         response = httpClient.getString();
//     }
//     else
//     {
//         Serial.println(httpCode);
//         Serial.println("https://api.map.baidu.com/location/ip?coor=gcj02&ak=I54VWZLG73NuKjVjC7gVKsH9FY7CCy6x");
//     }
//     httpClient.end();

//     // WiFiClient client;
//     // if (!client.connect("api.map.baidu.com", 80))
//     // {
//     //     Serial.println("connection failed");
//     //     return false;
//     // }

//     // client.print("GET /location/ip?coor=gcj02&ak=I54VWZLG73NuKjVjC7gVKsH9FY7CCy6x");
//     // client.println(" HTTP/1.1");
//     // client.println("Host: api.map.baidu.com");
//     // client.println("Connection: keep-alive");
//     // client.println();

//     // String response = "";
//     // while (client.connected())
//     // {
//     //     String line = client.readStringUntil('\n');
//     //     if (line == "\r")
//     //     {
//     //         Serial.println("headers received");
//     //         break;
//     //     }
//     // }
//     // while (client.available())
//     // {
//     //     response += client.readStringUntil('\n');
//     // }

//     Serial.println(response);
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
// }
