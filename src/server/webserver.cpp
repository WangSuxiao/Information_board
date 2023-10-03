// /*
//  * @Author       : WangSuxiao
//  * @Date         : 2023-09-26 09:10:55
//  * @LastEditTime : 2023-10-03 11:47:16
//  * @Description  : 开启网络服务器，用于和终端设备的通信
//  * @Tips         :
//  *      1. 添加、删除、修改单片机中缓存的数据
//  */
// #include "server/webserver.h"
// #include "config.h"

// #include <TimeLib.h>
// #include <vector>
// #include <LittleFS.h>

// std::vector<Todo> todos;

// // void insertTodoToFile(const Todo &todo)
// // {
// //     // 打开文件以追加模式
// //     File file = SPIFFS.open("/todos.txt", "a");
// //     if (file)
// //     {
// //         // 格式化数据并写入文件，用制表符分隔字段
// //         String data = String(todo.id) + "\t" + todo.start + "\t" + todo.end + "\t" + String(todo.level) + "\t" + todo.info;
// //         file.println(data);
// //         file.close();
// //     }
// //     else
// //     {
// //         Serial.println("Failed to open file for writing.");
// //     }
// // }
// // 将 Todo 对象写入文件
// void insertTodoToFile(const Todo &todo)
// {
//     // 打开文件以追加模式
//     File file = LittleFS.open("/todos.txt", "a");
//     if (file)
//     {
//         // 检查是否已存在相同 ID 的记录
//         if (isTodoIdUnique(todo.id))
//         {
//             // 格式化数据并写入文件，用制表符分隔字段
//             String data = String(todo.id) + "\t" + todo.start + "\t" + todo.end + "\t" + String(todo.level) + "\t" + todo.info;
//             file.println(data);
//         }
//         else
//         {
//             Serial.println("Todo with the same ID already exists.");
//         }
//         file.close();
//     }
//     else
//     {
//         Serial.println("Failed to open file for writing.");
//     }
// }

// // 检查是否已存在相同 ID 的记录
// bool isTodoIdUnique(int idToCheck)
// {
//     File file = LittleFS.open("/todos.txt", "r");
//     if (file)
//     {
//         String line;
//         while (file.available())
//         {
//             line = file.readStringUntil('\n');
//             int id = line.substring(0, line.indexOf('\t')).toInt();
//             if (id == idToCheck)
//             {
//                 file.close();
//                 return false; // ID 已存在
//             }
//         }
//         file.close();
//     }
//     else
//     {
//         Serial.println("Failed to open file for reading.");
//     }
//     return true; // ID 是唯一的
// }

// // 根据 ID 删除 Todo 对象
// void deleteTodoFromFile(int idToDelete)
// {
//     // 打开文件以读取和写入模式
//     File file = LittleFS.open("/todos.txt", "r");
//     File tempFile = LittleFS.open("/temp.txt", "w");
//     if (file && tempFile)
//     {
//         String line;
//         while (file.available())
//         {
//             line = file.readStringUntil('\n');
//             int id = line.substring(0, line.indexOf('\t')).toInt();
//             if (id != idToDelete)
//             {
//                 tempFile.println(line);
//             }
//         }
//         file.close();
//         tempFile.close();

//         // 删除原文件并将重命名的临时文件
//         LittleFS.remove("/todos.txt");
//         LittleFS.rename("/temp.txt", "/todos.txt");
//     }
//     else
//     {
//         Serial.println("Failed to open files for reading/writing.");
//     }
// }

// // 根据 ID 更新 Todo 对象
// void updateTodoInFile(const Todo &updatedTodo)
// {
//     // 打开文件以读取和写入模式
//     File file = LittleFS.open("/todos.txt", "r");
//     File tempFile = LittleFS.open("/temp.txt", "w");
//     if (file && tempFile)
//     {
//         String line;
//         while (file.available())
//         {
//             line = file.readStringUntil('\n');
//             int id = line.substring(0, line.indexOf('\t')).toInt();
//             if (id == updatedTodo.id)
//             {
//                 // 更新 Todo 对象
//                 String data = String(updatedTodo.id) + "\t" + updatedTodo.start + "\t" + updatedTodo.end + "\t" + String(updatedTodo.level) + "\t" + updatedTodo.info;
//                 tempFile.println(data);
//             }
//             else
//             {
//                 tempFile.println(line);
//             }
//         }
//         file.close();
//         tempFile.close();

//         // 删除原文件并将重命名的临时文件
//         LittleFS.remove("/todos.txt");
//         LittleFS.rename("/temp.txt", "/todos.txt");
//     }
//     else
//     {
//         Serial.println("Failed to open files for reading/writing.");
//     }
// }

// void handleTODO(ESP8266WebServer &server, DynamicJsonDocument &doc)
// {

//     String json_data = server.arg("plain");
//     DeserializationError error = deserializeJson(doc, json_data);
//     if (error == DeserializationError::Ok)
//     {
//         Serial.println("Received JSON Data:");

//         String method = doc["method"];
//         String key = doc["key"];
//         int id = doc["data"]["id"];
//         String start = doc["data"]["start"];
//         String end = doc["data"]["end"];
//         int level = doc["data"]["level"];
//         String info = doc["data"]["info"];

//         Todo todo = {id, start, end, level, info};
//         File file = SPIFFS.open("/todos.dat", "w");
//         if (!file)
//         {
//             Serial.println("Failed to open file for writing");
//             return;
//         }

//         if (method.equals("insert"))
//         {
//         }
//         else if (method.equals("delete"))
//         {
//         }
//         else if (method.equals("update"))
//         {
//         }

//         Serial.println("Method: " + method);
//         Serial.println("Key: " + key);
//         Serial.println("ID: " + String(id));
//         Serial.println("Start: " + start);
//         Serial.println("End: " + end);
//         Serial.println("Level: " + String(level));
//         Serial.println("Info: " + info);

//         // serializeJsonPretty(doc, Serial);
//         server.send(200, "text/plain", "JSON received and parsed successfully.");
//     }
//     else
//     {
//         Serial.print("Failed to parse JSON: ");
//         Serial.println(error.c_str());
//         server.send(400, "text/plain", "Failed to parse JSON");
//     }

//     // if (server.hasArg("start") && server.hasArg("end") && server.hasArg("level") && server.hasArg("info") && server.hasArg("key"))
//     // {
//     // start : 开始时间
//     // end   : 截止时间
//     // level : 重要性
//     // info  : 描述
//     // key   : 密钥
//     // String key = server.arg("key");
//     // if (key == "123456")
//     // {
//     //     // unsigned long start = server.arg("start").toInt() + TIMEZONE * SECS_PER_HOUR;

//     //     // time_t timestamp = (time_t)start;

//     //     // // 使用strftime函数将time_t格式化为"年-月-日 时:分"的字符串
//     //     // char formattedStartTime[20]; // 足够大的字符数组来容纳格式化后的字符串
//     //     // strftime(formattedStartTime, sizeof(formattedStartTime), "%y-%m-%d %H:%M", localtime(&timestamp));
//     //     // Serial.print(formattedStartTime);

//     //     // tmElements_t startTime;
//     //     // breakTime(start, startTime);

//     //     // int startYear = startTime.Year + 1970; // Time库使用相对于1970年的年份偏移量
//     //     // int startMonth = startTime.Month;
//     //     // int startDay = startTime.Day;
//     //     // int startHour = startTime.Hour;
//     //     // int startMinute = startTime.Minute;
//     //     // int startSecond = startTime.Second;

//     //     // server.send(200, "text/plain", formattedStartTime + String(startYear) + " / " + String(startMonth) + " / " + String(startDay) + "  " + String(startHour) + " : " + String(startMinute));
//     //     server.send(200, "text/plain", "formattedSt");
//     // }
//     // else
//     // {
//     //     server.send(400, "text/plain", "Bad Request");
//     // }
//     // }
// }

// void handleCountdown(ESP8266WebServer &server)
// {
//     if (server.hasArg("data") && server.hasArg("msg") && server.hasArg("key"))
//     {
//         String key = server.arg("key");
//         if (key == "123456")
//         {
//             String data = server.arg("data");
//             String msg = server.arg("msg");
//             for (int a = 0; a < 10; a++)
//             {
//                 Serial.print(data);
//                 Serial.print(msg);
//             }
//             server.send(200, "text/plain", "SUCCESS");
//         }
//         else
//         {
//             server.send(400, "text/plain", "Bad Request");
//         }
//     }
// }
