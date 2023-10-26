/*
 * @Author       : WangSuxiao
 * @Date         : 2023-10-03 15:00:43
 * @LastEditTime : 2023-10-25 14:36:20
 * @Description  : 处理客户端关于TODO的请求，提供CRUD
 *
 *                 改为类的实现方式
 * @Tips         :
 */
#include "module/todoServer.h"

/**
 * @Author: WangSuxiao
 * @description: 处理客户端的上传事件
 * @param {DynamicJsonDocument} &doc : 用以解析和创建JSON字符
 * @param {ESP8266WebServer} &server : Web服务器
 * @param {TodoManager} &todoManager : 用以管理Todo
 * @return {Any}
 */
void handleUpload(DynamicJsonDocument &doc, ESP8266WebServer &server, TodoManager &todoManager)
{
    String json_data = server.arg("plain");
    DeserializationError error = deserializeJson(doc, json_data);
    if (error)
    {
        // 解析失败
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        server.send(400, "text/plain", "Failed to parse JSON");
    }
    else
    {
        // 解析成功
        String method = doc["method"];
        String key = doc["key"];
        if (method == "clean")
        {
            // 清理文件
            todoManager.cleanFile();
        }
        else
        {
            Todo todo;
            String todoMSG;
            todo.id = doc["data"]["id"].as<int>();
            todo.start = doc["data"]["start"].as<time_t>();
            todo.end = doc["data"]["end"].as<time_t>();
            todo.level = doc["data"]["level"].as<int>();
            todoMSG = doc["data"]["info"].as<String>();
            todoMSG.replace("\t", "");

            Serial.println("Method: " + method);
            Serial.println("Key: " + key);
            Serial.println("ID: " + String(todo.id));
            Serial.println("Start: " + String(todo.start));
            Serial.println("End: " + String(todo.end));
            Serial.println("Level: " + String(todo.level));
            Serial.println("Info: " + todoMSG);
            if (method == "insert")
            {
                todoManager.insertTodo(todo, todoMSG);
            }
            else if (method == "delete")
            {
                todoManager.deleteTodo(todo.id);
            }
            else if (method == "update")
            {
                todoManager.updateTodo(todo, todoMSG);
            }
        }
        server.send(200, "text/plain", "Operation completed successfully.");
    }
}


/**
 * @Author: WangSuxiao
 * @description: 处理服务器请求，获取todo列表
 * @param {DynamicJsonDocument} &doc : 用以解析和创建JSON字符
 * @param {ESP8266WebServer} &server : Web服务器
 * @param {TodoManager} &todoManager : 用以管理Todo
 * @return {Any}
 */
void handleReadAll(DynamicJsonDocument &doc, ESP8266WebServer &server, TodoManager &todoManager)
{
    doc.clear();
    JsonArray data = doc.createNestedArray("data");
    File file = LittleFS.open(todoManager.getTodoFilename(), "r");

    for (Todo todo : todoManager.getTodos())
    {
        JsonObject todoObj = data.createNestedObject();
        todoObj["id"] = todo.id;
        todoObj["start"] = todo.start;
        todoObj["end"] = todo.end;
        todoObj["level"] = todo.level;
        todoObj["offset"] = todo.offset;
        todoObj["info"] = todoManager.getTodoInfo(todo.id);
        file.seek(todo.offset);
        todoObj["raw"] = file.readStringUntil('\n');

    }
    doc["code"] = 200;
    doc["counter"] = todoManager.getSize();
    String jsonResponse;
    serializeJson(doc, jsonResponse);
    doc.clear();
    server.send(200, "application/json", jsonResponse);
}
