#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

void handleRoot(ESP8266WebServer &server)
{
    server.send(200, "text/plain", "Hello from ESP8266!");
}

// 新线程函数，负责运行服务器
void serverTask(void *parameter)
{
    ESP8266WebServer* server = (ESP8266WebServer*)parameter;
    server->on("/", [&server]()
                  { handleRoot(*server); });
    server->begin();

    while (1)
    {
        server->handleClient(); // 处理客户端请求
        yield();               // 允许任务切换，避免阻塞其他任务
    }
}
