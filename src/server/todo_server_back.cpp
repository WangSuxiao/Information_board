/*
 * @Author       : WangSuxiao
 * @Date         : 2023-10-03 15:00:43
 * @LastEditTime : 2023-10-09 16:14:08
 * @Description  : 处理客户端关于TODO的请求，提供CRUD
 *                 备份：
 *                 基于文件的操作
 * @Tips         :
 */

// #include "config.h"
// #include "server/todo_server.h"

// std::vector<Todo> todos;
// // 定义 Todo 数据类型

// int sorted = 0;
// /**
//  * @description: 检查Todo的ID是否唯一
//  * @param {int} idToCheck :
//  * @return {Any}
//  */
// bool isTodoIdUnique(u32_t idToCheck)
// {
//     File file = LittleFS.open(TODO_FILE, "r");
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
//                 return false;
//             }
//         }
//         file.close();
//     }
//     else
//     {
//         Serial.println("Failed to open file for reading.");
//     }
//     return true;
// }

// /**
//  * @description: 向文件中追加记录
//  * @param {Todo} &todo :
//  */
// u32_t insertTodoToFile(const Todo &todo,  String info)
// {
//     // 打开文件以追加模式
//     File file = LittleFS.open(TODO_FILE, "a");
//     u32_t nextpos = todo.offset;
//     if (file)
//     {
//         // 检查是否已存在相同 ID 的记录
//         if (isTodoIdUnique(todo.id))
//         {
//             // 格式化数据并写入文件，用制表符分隔字段
//             String data = String(todo.id) + "\t" + String(todo.start) + "\t" + String(todo.end) + "\t" + String(todo.level) + "\t" + info;

//             file.seek(nextpos,SeekSet);

//             Serial.print("存储信息：");
//             Serial.print("String(todo.id) : ");
//             Serial.println(String(todo.id).length());
//             Serial.print("todo.start : ");
//             Serial.println(String(todo.start).length());
//             Serial.print("todo.end : ");
//             Serial.println(String(todo.end).length());
//             Serial.print("String(todo.level) : ");
//             Serial.println(String(todo.level).length());
//             Serial.print("todo.info : ");
//             Serial.println(info.length());
//             Serial.print("整体长度 ：");
//             Serial.println(data.length());

//             file.println(data);
//             nextpos = file.position();

//             // file.close();
//             // return true;
//         }
//         else
//         {
//             Serial.println("Todo with the same ID already exists.");
//             // file.close();
//             // return false;
//         }
//         file.close();
//     }
//     else
//     {
//         Serial.println("Failed to open file for writing.");
//         // return false;
//     }
//     return nextpos;
// }

// // 根据 ID 删除 Todo 对象
// void deleteTodoFromFile(int idToDelete)
// {
//     // 打开文件以读取和写入模式
//     File file = LittleFS.open(TODO_FILE, "r");
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
//         LittleFS.remove(TODO_FILE);
//         LittleFS.rename("/temp.txt", TODO_FILE);
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
//     File file = LittleFS.open(TODO_FILE, "r");
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
//         LittleFS.remove(TODO_FILE);
//         LittleFS.rename("/temp.txt", TODO_FILE);
//     }
//     else
//     {
//         Serial.println("Failed to open files for reading/writing.");
//     }
// }

// void handleUpload(DynamicJsonDocument &doc, ESP8266WebServer &server)
// {
//     String json_data = server.arg("plain");
//     DeserializationError error = deserializeJson(doc, json_data);

//     if (error)
//     {
//         Serial.print("Failed to parse JSON: ");
//         Serial.println(error.c_str());
//         server.send(400, "text/plain", "Failed to parse JSON");
//     }
//     else
//     {
//         String method = doc["method"];
//         String key = doc["key"];

//         Todo todo;
//         todo.id = doc["data"]["id"].as<int>();
//         todo.start = doc["data"]["start"].as<time_t>();
//         todo.end = doc["data"]["end"].as<time_t>();
//         todo.level = doc["data"]["level"].as<int>();
//         todo.info = doc["data"]["info"].as<String>();
//         todo.info.replace("\t", "");

//         Serial.println("Method: " + method);
//         Serial.println("Key: " + key);
//         Serial.println("ID: " + String(todo.id));
//         Serial.println("Start: " + String(todo.start));
//         Serial.println("End: " + String(todo.end));
//         Serial.println("Level: " + String(todo.level));
//         Serial.println("Info: " + todo.info);
//         if (method == "insert")
//         {
//             insertTodoToFile(todo);
//         }
//         else if (method == "delete")
//         {
//             deleteTodoFromFile(todo.id);
//         }
//         else if (method == "update")
//         {
//             updateTodoInFile(todo);
//         }

//         server.send(200, "text/plain", "Operation completed successfully.");
//     }
// }

// void readAllTodosFromFile()
// {
//     // 打开文件以读取模式
//     File file = LittleFS.open("/todos.txt", "r");
//     if (file)
//     {
//         String line;
//         String timetmp;
//         while (file.available())
//         {
//             line = file.readStringUntil('\n');
//             Todo todo;
//             todo.id = line.substring(0, line.indexOf('\t')).toInt();
//             line = line.substring(line.indexOf('\t') + 1);

//             timetmp = line.substring(0, line.indexOf('\t'));
//             timetmp.trim();
//             todo.start = strtoll(timetmp.c_str(), NULL, 10);
//             line = line.substring(line.indexOf('\t') + 1);
//             timetmp = line.substring(0, line.indexOf('\t'));
//             timetmp.trim();
//             todo.end = strtoll(timetmp.c_str(), NULL, 10);
//             line = line.substring(line.indexOf('\t') + 1);
//             todo.level = line.substring(0, line.indexOf('\t')).toInt();
//             todo.info = line.substring(line.indexOf('\t') + 1);

//             // 将 Todo 对象添加到 vector 中
//             todos.push_back(todo);
//             Serial.println(line);
//         }
//         file.close();
//     }
//     else
//     {
//         Serial.println("Failed to open file for reading.");
//     }
// }

// void handleReadAll(DynamicJsonDocument &doc, ESP8266WebServer &server)
// {
//     doc.clear();
//     JsonArray data = doc.createNestedArray("data");
//     String line;
//     File file = LittleFS.open("/todos.txt", "r");
//     int counter = 0;
//     String timetmp;
//     while (file.available())
//     {
//         line = file.readStringUntil('\n');
//         Todo todo;
//         todo.id = line.substring(0, line.indexOf('\t')).toInt();
//         line = line.substring(line.indexOf('\t') + 1);
//         timetmp = line.substring(0, line.indexOf('\t'));
//         timetmp.trim();
//         todo.start = strtoll(timetmp.c_str(), NULL, 10);
//         line = line.substring(line.indexOf('\t') + 1);
//         timetmp = line.substring(0, line.indexOf('\t'));
//         timetmp.trim();
//         todo.end = strtoll(timetmp.c_str(), NULL, 10);
//         line = line.substring(line.indexOf('\t') + 1);
//         todo.level = line.substring(0, line.indexOf('\t')).toInt();
//         todo.info = line.substring(line.indexOf('\t') + 1);

//         JsonObject todoObj = data.createNestedObject();
//         todoObj["id"] = todo.id;
//         todoObj["start"] = todo.start;
//         todoObj["end"] = todo.end;
//         todoObj["level"] = todo.level;
//         todoObj["info"] = todo.info;

//         Serial.println("ID: " + String(todo.id));
//         Serial.println("Start: " + todo.start);
//         Serial.println("End: " + todo.end);
//         Serial.println("Level: " + String(todo.level));
//         Serial.println("Info: " + todo.info);

//         counter++;
//     }
//     file.close();

//     doc["code"] = 200;
//     doc["counter"] = todos.size();

//     String jsonResponse;
//     serializeJson(doc, jsonResponse);
//     doc.clear();
//     server.send(200, "application/json", jsonResponse);

//     // // 打开文件以读取模式
//     // File file = LittleFS.open(TODO_FILE, "r");
//     // if (file)
//     // {
//     //     String line;
//     //     while (file.available())
//     //     {
//     //         line = file.readStringUntil('\n');
//     //         Todo todo;
//     //         todo.id = line.substring(0, line.indexOf('\t')).toInt();
//     //         line = line.substring(line.indexOf('\t') + 1);
//     //         todo.start = line.substring(0, line.indexOf('\t'));
//     //         line = line.substring(line.indexOf('\t') + 1);
//     //         todo.end = line.substring(0, line.indexOf('\t'));
//     //         line = line.substring(line.indexOf('\t') + 1);
//     //         todo.level = line.substring(0, line.indexOf('\t')).toInt();
//     //         todo.info = line.substring(line.indexOf('\t') + 1);

//     //         // 将 Todo 对象添加到 vector 中
//     //         todos.push_back(todo);
//     //         Serial.println(line);
//     //     }
//     //     file.close();
//     // }
//     // else
//     // {
//     //     Serial.println("Failed to open file for reading.");
//     // }
//     // server.send(200, "text/plain", "All records read and displayed.");
// }
