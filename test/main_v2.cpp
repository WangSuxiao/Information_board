// /*
//  * @Author       : WangSuxiao
//  * @Date         : 2023-05-01 11:00:03
//  * @LastEditTime : 2023-05-01 15:42:46
//  * @Description  : 本部分代码实现了： 定位的测试
//  *                  使用ESP8266通过http协议向网站：http://api.cellocation.com:84发起请求，返回的结果是一段JSON数据。
//  *                  下面是几种可能的不同的返回：
//                         {"errcode": 0, "lat": "25.283480551425473", "lon": "110.33620787709138", "radius": "100", "address": "广西壮族自治区路口西42米", "type": "wifi"}
//                         {"errcode": 10001, "lat": "0.0", "lon": "0.0", "radius": "0", "address": "", "type": ""}
//                         {"errcode": 10000, "lat": "0.0", "lon": "0.0", "radius": "0", "address": "", "type": ""}
//  * @Tips         :
//  */


// #include <ESP8266WiFi.h>
// // #include <ESP8266HTTPClient.h>
// #include <WiFiClient.h>
// #include <ArduinoJson.h>

// const char *ssid = "tmp";
// const char *password = "12345678";
// const char *server = "api.cellocation.com";
// const int httpPort = 84;

// void setup() {
//   Serial.begin(115200);
//   delay(10);

//   // 连接WiFi网络
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.println("WiFi connected");

//   // 扫描周围的WIFI信息
//   Serial.println("Scanning WiFi networks...");
//   int numNetworks = WiFi.scanNetworks();
//   if (numNetworks == 0) {
//     Serial.println("No WiFi networks found");
//     return;
//   }
//   String wlParam = "";
//   for (int i = 0; i < numNetworks; i++) {
//     if (i > 0) {
//       wlParam += ";";
//     }
//     wlParam += WiFi.BSSIDstr(i) + ",";
//     wlParam += String(WiFi.RSSI(i));
//   }

//   // 发送HTTP请求
//   Serial.println("Sending HTTP request...");
//   WiFiClient client;
//   if (!client.connect(server, httpPort)) {
//     Serial.println("Connection failed");
//     return;
//   }
//   client.print("GET /loc/?wl=");
//   client.print(wlParam);
//   client.print("&coord=gcj02&output=json");
//   client.println(" HTTP/1.1");
//   client.println("Host: api.cellocation.com");
//   client.println("Connection: close");
//   client.println();

//   // 读取HTTP响应
//   String response = "";
//   while (client.connected()) {
//     String line = client.readStringUntil('\n');
//     if (line == "\r") {
//       break;
//     }
//   }
//   while (client.available()) {
//     response += client.readString();
//   }
//   Serial.println(response);

//   // 解析JSON数据
//   DynamicJsonDocument doc(1024);
//   DeserializationError error = deserializeJson(doc, response);
//   if (error) {
//     Serial.print("deserializeJson() failed: ");
//     Serial.println(error.c_str());
//     return;
//   }
//   int errcode = doc["errcode"];
//   if (errcode == 0) {
//     String lat = doc["lat"];
//     String lon = doc["lon"];
//     String radius = doc["radius"];
//     String address = doc["address"];
//     String type = doc["type"];
//     Serial.println("Location:");
//     Serial.print("Lat: ");
//     Serial.println(lat);
//     Serial.print("Lon: ");
//     Serial.println(lon);
//     Serial.print("Radius: ");
//     Serial.println(radius);
//     Serial.print("Address: ");
//     Serial.println(address);
//     Serial.print("Type: ");
//     Serial.println(type);
//   } else {
//     Serial.print("Error code: ");
//     Serial.println(errcode);
//   }
// }

// void loop() {
//   // do nothing
// }
