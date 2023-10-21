// #ifndef _TODO_SERVER_H_
// #define _TODO_SERVER_H_

// #include <ESP8266WebServer.h>
// #include <ArduinoJson.h>
// #include <ESP8266WiFi.h>
// #include <ESP8266WebServer.h>
// #include <ArduinoJson.h>
// #include <LittleFS.h>
// #include <Arduino.h>
// #include <vector>
// #define TIMEZONE 8

// class TodoManager {
// public:
//     TodoManager(const String& filename);

//     bool isTodoIdUnique(uint32_t idToCheck);
//     bool insertTodo(Todo& todo, const String& info);
//     bool deleteTodo(uint32_t idToDelete);
//     uint32_t updateTodo(Todo& updatedTodo, const String& info);
//     bool cleanFile();
//     void printAll();
//     String getTodoFilename();
//     std::vector<Todo> getTodos();
//     int getSize();
//     String getTodoInfo(uint32_t id);
//     Todo getTodo(uint32_t id);
//     // void handleUpload(DynamicJsonDocument& doc, ESP8266WebServer& server);
//     // void readAllTodosFromFile(DynamicJsonDocument& doc, ESP8266WebServer& server);

// private:
//     String todoFileName;
//     int flag;   // 按照什么字段排序
//     int mode;   // 正序还是逆序
//     std::vector<Todo> todos;

//     // bool compareTODO(const Todo &a, const Todo &b, int flag,int mode);
// };

// bool compareTODO(const Todo &a, const Todo &b, int flag,int mode);
// void handleUpload(DynamicJsonDocument &doc, ESP8266WebServer &server, TodoManager &todoManager);

// // void handleReadAll(DynamicJsonDocument &doc,ESP8266WebServer &server);

// #endif
