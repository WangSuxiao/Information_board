// /*
//  * @Author       : WangSuxiao
//  * @Date         : 2023-10-01 10:37:33
//  * @LastEditTime : 2023-10-01 14:32:47
//  * @Description  : 使用本地时间更新
//  * @Tips         :
//  */
// #include <ESP8266WiFi.h>
// #include <time.h>
// #include <Arduino.h>

// const char *ssid = "tmp";
// const char *password = "12345678";

// void connectToWiFi()
// {
//     WiFi.begin(ssid, password);
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         delay(1000);
//         Serial.println("Connecting to WiFi...");
//     }
//     Serial.println("Connected to WiFi");
// }

// void setup()
// {
//     Serial.begin(115200);
//     connectToWiFi();
//     configTime(0, 0, "pool.ntp.org", "time.nist.gov");
// }

// void loop()
// {
//     // Get the current time
//     struct tm timeinfo;
//     if (getLocalTime(&timeinfo))
//     {
//         char timeString[50]; // 创建一个足够大的字符数组来存储格式化的日期和时间

//         // 构建格式化的日期和时间字符串
//         sprintf(timeString, "%04d-%02d-%02d %02d:%02d:%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

//         // 打印格式化的日期和时间
//         Serial.println(timeString);
//     }
//     else
//     {
//         Serial.println("Failed to obtain time");
//     }
//     delay(1000);
// }
