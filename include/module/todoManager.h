#ifndef _TODO_MANAGER_H_
#define _TODO_MANAGER_H_

#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <vector>
#include "config.h"

class TodoManager {
public:
    TodoManager(const String& filename);

    bool isTodoIdUnique(uint32_t idToCheck);
    bool insertTodo(Todo& todo, const String& info);
    bool deleteTodo(uint32_t idToDelete);
    uint32_t updateTodo(Todo& updatedTodo, const String& info);
    bool cleanFile();
    void printAll();
    String getTodoFilename();
    std::vector<Todo> getTodos();
    int getSize();
    String getTodoInfo(uint32_t id);
    Todo getTodobyID(uint32_t id);
    Todo getTodobyIndex(uint32_t index);

private:
    String todoFileName;
    int flag;   // 按照什么字段排序
    int mode;   // 正序还是逆序
    std::vector<Todo> todos;
};

#endif
