/*
 * @Author       : WangSuxiao
 * @Date         : 2023-04-25 19:03:03
 * @LastEditTime : 2023-10-03 17:02:59
 * @Description  :  开机后网络连接
 *
 *                  1. 修改代码结构
 *
 *                  2. 修改逻辑：开机后首先尝试联网，失败再创建AP
 *
 *                  3. 2023/08/07 修改代码结构
 *                      ———— 读取配置
 *                      ———— 尝试连接 ： 热点信息
 *                      ———— 开启服务器，提醒
 * @Tips         :
 */

#include "module/wificonfig.h"
#include "board/GUI_Paint.h"
#include "board/EPD.h"

#define WIFI_CONFIG_FILE "/config.json"

void onWiFiDisconnect()
{
    Serial.println("WIFI Disconnected");
    // 在这里执行你想要的特定函数
}

/**
 * @description: 尝试连接网络
 * @param {DynamicJsonDocument} &doc :
 * @param {uint8_t} *BlackImage :
 * @return {Any}
 */
int tryConnectFromFile(DynamicJsonDocument &doc, uint8_t *BlackImage)
{
    int xstart = 32;
    ESP8266WiFiMulti wifiMulti;
    Serial.println("Connecting ...");
    Paint_Clear(WHITE);
    int line = 1;
    int height = PINGFANG12.Height;

    if (doc.size() <= 0)
    {
        Paint_DrawString_CN_From_File(xstart, height * line, "配置文件为空…", &PINGFANG12, BLACK, WHITE);
        EPD_4IN2_Display(BlackImage);
        line = (line + 1) % 10;
        return line + 1;
    }
    Paint_DrawString_CN_From_File(xstart, height * line, "尝试连接以下热点…", &PINGFANG12, BLACK, WHITE);

    WiFi.disconnect(true); // 禁用自动重连
    WiFi.onStationModeDisconnected([&doc, &BlackImage](const WiFiEventStationModeDisconnected &event)
                                   {
                                       Serial.print("============准备重连=============");
                                       // tryConnectFromFile(doc,BlackImage);
                                   });

    line = (line + 2) % 10;
    for (size_t i = 0; i < doc.size(); i++)
    {
        Paint_DrawString_EN(xstart, height * line, doc[i]["ssid"], &Font12, BLACK, WHITE);
        line = (line + 1) % 10;
        wifiMulti.addAP(doc[i]["ssid"], doc[i]["password"]);
    }
    EPD_4IN2_Display(BlackImage);
    // 通过串口监视器输出信息告知用户NodeMCU正在尝试连接WiFi

    uint8_t attempt = 0;
    while (wifiMulti.run() != WL_CONNECTED)
    {
        if (attempt > 10)
        {
            Serial.printf("Failed to connect ");
            line = (line + 1) % 10;
            Paint_DrawString_CN_From_File(xstart, height * line, "连接失败…", &PINGFANG12, BLACK, WHITE);
            EPD_4IN2_Display(BlackImage);
            return line + 1;
        }
        delay(500);
        Serial.println("=====================");
        Serial.println(xstart);
        Serial.println(PINGFANG12.Height * attempt);
        Serial.println("=====================");
        Paint_DrawString_CN_From_File(xstart + PINGFANG12.Height * attempt, height * line, "…", &PINGFANG12, BLACK, WHITE);

        EPD_4IN2_Display(BlackImage);
        attempt++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        // 连接成功，输出一下
        Serial.printf("\nConnected to %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    }
    return line + 1;
}

/**
 * @description: 访问根目录
 * @param {DynamicJsonDocument} &doc :
 * @param {ESP8266WebServer} &server :
 * @return {Any}
 */
void handleRoot(DynamicJsonDocument &doc, ESP8266WebServer &server)
{
    String html = "<html><head><meta charset=\"UTF-8\"><title>Wi-Fi账户密码列表</title>";
    html += "<style>body{background-color:#f2f2f2; font-size:16px; font-family:'Microsoft Yahei', sans-serif; text-align:center;}"
            "table{border-collapse:collapse; margin:20px auto;}"
            "th{background-color:#e6e6e6; padding:10px;}"
            "td{padding:5px;}"
            "form{margin:20px auto; max-width: 400px;}"
            "label{display:block; margin-bottom:5px; text-align:left;}"
            "input[type=text], input[type=password]{width:100%; padding:10px; border-radius:5px; border:1px solid #ccc; margin-bottom:10px;}"
            "input[type=submit]{background-color:#4CAF50; color:#fff; border:none; border-radius:5px; padding:10px 20px; cursor:pointer;}"
            "button{background-color:#F40; color:#fff; border:none; border-radius:5px; padding:10px 20px; cursor:pointer;}"
            "</style>";
    html += "</head><body><h1>已保存的Wi-Fi账户</h1><table>";
    html += "<tr><th>WIFI</th><th>Password</th><th>操作</th></tr>";
    for (size_t i = 0; i < doc.size(); i++)
    {
        const char *ssid = doc[i]["ssid"];
        const char *password = doc[i]["password"];
        html += "<tr><td>" + String(ssid) + "</td><td>" + String(password) + "</td><td>";
        // html += "<a href='/delete?index=" + String(i) + "'>删除</a></td></tr>";
        html += "<a style='text-decoration:none;' href='/delete?index=" + String(i) + "'>删除</a></td></tr>";
    }
    html += "</table>";
    html += "<form method='post' action='/add'>";
    html += "<h1>添加新的Wi-Fi</h1>";
    html += "<label>WIFI:</label><input type='text' name='ssid'/>";
    html += "<label>Password:</label><input type='text' name='password'/>";
    html += "<input type='submit' value='添加'/></form>";
    html += "<button onclick=\"window.location.href='/finish'\" type=\"button\" id=\"add\">完成</button>";
    html += "</body></html>";

    server.send(200, "text/html", html);
}

/**
 * @description: 请求添加
 * @param {DynamicJsonDocument} &doc :
 * @param {ESP8266WebServer} &server :
 * @return {Any}
 */
void handleAdd(DynamicJsonDocument &doc, ESP8266WebServer &server)
{
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    if (ssid == "" || password == "")
    {
        server.send(400, "text/plain", "无效的参数");
        return;
    }
    JsonObject obj = doc.createNestedObject();
    obj["ssid"] = ssid;
    obj["password"] = password;
    File configFile = LittleFS.open("/config.json", "w");
    if (!configFile)
    {
        server.send(500, "text/plain", "无法打开配置文件");
        return;
    }
    serializeJson(doc, configFile);
    configFile.close();
    server.sendHeader("Location", String("/"), true);
    server.send(302, "text/plain", "");
}

/**
 * @description:请求删除
 * @param {DynamicJsonDocument} &doc :
 * @param {ESP8266WebServer} &server :
 * @return {Any}
 */
void handleDelete(DynamicJsonDocument &doc, ESP8266WebServer &server)
{
    size_t index = server.arg("index").toInt();
    if (index < 0 || index >= doc.size())
    {
        server.send(400, "text/plain", "无效的索引");
        return;
    }
    doc.remove(index);
    File configFile = LittleFS.open("/config.json", "w");
    if (!configFile)
    {
        server.send(500, "text/plain", "无法打开配置文件");
        return;
    }
    serializeJson(doc, configFile);
    configFile.close();
    server.sendHeader("Location", String("/"), true);
    server.send(302, "text/plain", "");
}

/**
 * @description: 结束
 * @param {DynamicJsonDocument} &doc :
 * @param {ESP8266WebServer} &server :
 * @param {uint8_t} *BlackImage :
 * @return {Any}
 */
void handleFinish(DynamicJsonDocument &doc, ESP8266WebServer &server, uint8_t *BlackImage)
{
    if (doc.size() <= 0)
    {
        server.send(500, "text/plain", "无法打开配置文件");
        return;
    }
    // 需要给出提示： 服务器暂时停止
    String html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>WIFI配置</title><script type=\"text/javascript\">function closePage() {alert(\"配置完成，设备将根据配置内容连接网络，接下来请根据屏幕提示操作。请手动关闭网页\");window.opener = null;window.open('about:blank', '_self').top.close();}</script></head><body onload=\"closePage()\"></body></html>";
    server.send(200, "text/html", html);
    tryConnectFromFile(doc, BlackImage);
}

/**
 * @description: 获取配置文件
 * @param {DynamicJsonDocument} &doc :
 * @param {uint8_t} *BlackImage :
 * @return {Any}
 *      true : 文件读取不为空
 *      false: 文件不存在 / 文件内容为空
 */
bool initConfig(DynamicJsonDocument &doc, uint8_t *BlackImage)
{
    int xstart = 32;
    int line = 1;
    int height = PINGFANG12.Height;
    Paint_Clear(WHITE);

    // 1. 打开文件系统
    if (!LittleFS.begin())
    {
        Serial.println("文件系统初始化失败");
        Paint_DrawString_CN_From_File(xstart, height * line++, "文件系统初始化失败", &PINGFANG12, BLACK, WHITE);
        EPD_4IN2_Display(BlackImage);
        return false;
    }
    else
    {
        Serial.println("文件系统初始化成功");
        Paint_DrawString_CN_From_File(xstart, height * line++, "文件系统初始化成功", &PINGFANG12, BLACK, WHITE);
        EPD_4IN2_Display(BlackImage);
    }

    // 2. 打开配置文件
    File configFile = LittleFS.open(WIFI_CONFIG_FILE, "r");
    if (!configFile)
    {
        // 打开失败，尝试创建
        Serial.println("网络配置文件不存在，即将创建");
        Paint_DrawString_CN_From_File(xstart, height * line++, "网络配置文件不存在，即将创建", &PINGFANG12, BLACK, WHITE);
        EPD_4IN2_Display(BlackImage);
        File newConfigFile = LittleFS.open(WIFI_CONFIG_FILE, "w");
        if (!newConfigFile)
        {
            // 创建失败，退出
            Serial.println("配置文件创建失败，退出配置");
            Paint_DrawString_CN_From_File(xstart, height * line++, "配置文件创建失败，退出配置", &PINGFANG12, BLACK, WHITE);
            EPD_4IN2_Display(BlackImage);
            return false;
        }
        Serial.println("配置文件创建成功");
        Paint_DrawString_CN_From_File(xstart, height * line++, "配置文件创建成功", &PINGFANG12, BLACK, WHITE);
        EPD_4IN2_Display(BlackImage);
        newConfigFile.close();
        configFile = LittleFS.open(WIFI_CONFIG_FILE, "r");
    }

    // 3. 初始化doc对象
    deserializeJson(doc, configFile);
    configFile.close();

    // 4. 输出配置
    if (doc.size() <= 0)
    {
        Paint_DrawString_CN_From_File(xstart, height * line, "配置文件为空", &PINGFANG12, BLACK, WHITE);
        EPD_4IN2_Display(BlackImage);
        line = (line + 1) % 10;
        return false;
    }

    Paint_DrawString_CN_From_File(xstart, height * line, "保存的热点信息为：", &PINGFANG12, BLACK, WHITE);
    line = (line + 2) % 10;
    for (size_t i = 0; i < doc.size(); i++)
    {
        Paint_DrawString_EN(xstart, height * line, doc[i]["ssid"], &Font12, BLACK, WHITE);
        line = (line + 1) % 10;
    }
    EPD_4IN2_Display(BlackImage);
    return true;
}

void configFromWeb(DynamicJsonDocument &doc, uint8_t *BlackImage)
{
    int xstart = 32;
    int line = 1;
    int height = PINGFANG12.Height;
    Paint_Clear(WHITE);

    Paint_DrawString_CN_From_File(xstart, height * line++, "请连接热点进行配网", &PINGFANG12, BLACK, WHITE);
    line = line + 1;
    Paint_DrawString_EN(xstart, height * line++, "NAME: EPAPERWiFiAP", &Font16, BLACK, WHITE);
    Paint_DrawString_EN(xstart, height * line++, "PWD : 123456789", &Font16, BLACK, WHITE);
    line = line + 1;
    Paint_DrawString_CN_From_File(xstart, height * line, "再使用浏览器访问：", &PINGFANG12, BLACK, WHITE);
    Paint_DrawString_EN(xstart + PINGFANG12.Width * 9, height - Font16.Height + height * line++, "192.168.4.1", &Font16, BLACK, WHITE);
    //
    EPD_4IN2_Display(BlackImage);
    // 通过配置文件连接网络失败，开启配置
    ESP8266WebServer server(80);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    server.on("/", [&doc, &server]()
              { handleRoot(doc, server); });
    server.on("/add", HTTP_POST, [&doc, &server]()
              { handleAdd(doc, server); });
    server.on("/delete", [&doc, &server]()
              { handleDelete(doc, server); });
    server.on("/finish", [&doc, &server, BlackImage]()
              { handleFinish(doc, server, BlackImage); });

    server.begin();
    Serial.println("Web server started");
    while (WiFi.status() != WL_CONNECTED)
    {
        // finishHandle将会连接网络
        server.handleClient();
        delay(300);
    }
    // 需要改进 ： 给出提示连接成功

    server.stop();
    WiFi.softAPdisconnect(true);
}
void wifiConfig(uint8_t *BlackImage)
{

    EPD_4IN2_Init_Fast();
    EPD_4IN2_Clear();

    int xstart = 32;
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    EPD_4IN2_Display(BlackImage);
    DynamicJsonDocument doc(512);

    // 1. 读取配置文件
    initConfig(doc, BlackImage);

    // 2. 尝试连接
    tryConnectFromFile(doc, BlackImage);

    // 3. 连接失败，开启网络服务器，配网
    if (WiFi.status() != WL_CONNECTED)
    {
        configFromWeb(doc, BlackImage);
    }
    Paint_Clear(WHITE);
    Paint_DrawString_CN_From_File(xstart, 20, "网络连接成功", &PINGFANG12, BLACK, WHITE);
    Paint_DrawString_EN(xstart, 42, (String("WIFI: ") + WiFi.SSID()).c_str(), &Font20, BLACK, WHITE);

    Paint_DrawString_EN(xstart, 62, (String("I P : ") + WiFi.localIP().toString()).c_str(), &Font20, BLACK, WHITE);

    EPD_4IN2_Display(BlackImage);
}

/**
 * @description: 连接WIFI: tmp
 * @return {Any}
 */
void wifiConnect()
{
    IPAddress dns(8, 8, 8, 8);
    WiFi.config(IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0), dns);
    WiFi.begin("tmp", "12345678");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WIFI Connect Success");
    Serial.println(WiFi.SSID());
}

// int tryConnectFromFile(DynamicJsonDocument &doc, uint8_t *BlackImage)
// {
//     ESP8266WiFiMulti wifiMulti;
//     WiFi.disconnect(true); // 禁用自动重连
//     WiFi.onStationModeDisconnected([&doc,&BlackImage](const WiFiEventStationModeDisconnected &event)
//                                    {
//                                     Serial.print("============准备重连=============");
//                                     tryConnectFromFile(doc,BlackImage);
//                                     });
//     while (wifiMulti.run() != WL_CONNECTED)
//     {
//             Serial.printf(". ");
//             delay(500);
//     }
// }

// int tryConnectFromFile(DynamicJsonDocument &doc, uint8_t *BlackImage)
// {
//     ESP8266WiFiMulti wifiMulti;
//     WiFi.disconnect(true); // 禁用自动重连
//     WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event)
//                                    {
//                                     Serial.print("============准备重连=============");
//                                     });
//     while (wifiMulti.run() != WL_CONNECTED)
//     {
//             Serial.printf(". ");
//             delay(500);
//     }
// }
