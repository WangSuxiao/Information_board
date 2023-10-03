// #include <ESP8266WiFi.h>
// #include <ESP8266WebServer.h>
// #include <ArduinoJson.h>
// #include <LittleFS.h>
// #include <Arduino.h>
// #include <vector>

// const char *ssid = "tmp";          // 你的WiFi名称
// const char *password = "12345678"; // 你的WiFi密码

// ESP8266WebServer server(80);
// std::vector<Todo> todos;
// // 定义 Todo 数据类型
// struct Todo
// {
//     int id;
//     String start;
//     String end;
//     int level;
//     String info;
// };

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

// void handleUpload()
// {
//     String json_data = server.arg("plain");

//     // 输出接收到的JSON数据
//     Serial.println("Received JSON Data:");
//     Serial.println(json_data);

//     // 解析JSON数据
//     DynamicJsonDocument doc(1024); // 适当调整缓冲区大小
//     DeserializationError error = deserializeJson(doc, json_data);

//     if (error)
//     {
//         Serial.print("Failed to parse JSON: ");
//         Serial.println(error.c_str());
//         server.send(400, "text/plain", "Failed to parse JSON");
//     }
//     else
//     {
//         // 提取JSON中的值
//         String method = doc["method"];
//         String key = doc["key"];

//         // 实例化 Todo 对象
//         Todo todo;
//         todo.id = doc["data"]["id"].as<int>();
//         todo.start = doc["data"]["start"].as<String>();
//         todo.start.replace("\t", "");
//         todo.end = doc["data"]["end"].as<String>();
//         todo.end.replace("\t", "");
//         todo.level = doc["data"]["level"].as<int>();
//         todo.info = doc["data"]["info"].as<String>();
//         todo.info.replace("\t", "");

//         Serial.println("Method: " + method);
//         Serial.println("Key: " + key);
//         Serial.println("ID: " + String(todo.id));
//         Serial.println("Start: " + todo.start);
//         Serial.println("End: " + todo.end);
//         Serial.println("Level: " + String(todo.level));
//         Serial.println("Info: " + todo.info);
//         // 根据方法执行操作
//         if (method == "insert")
//         {
//             // 将 Todo 对象写入文件
//             insertTodoToFile(todo);
//         }
//         else if (method == "delete")
//         {
//             // 根据 ID 删除 Todo 对象
//             deleteTodoFromFile(todo.id);
//         }
//         else if (method == "update")
//         {
//             // 根据 ID 更新 Todo 对象
//             updateTodoInFile(todo);
//         }

//         server.send(200, "text/plain", "Operation completed successfully.");
//     }
// }

// void readAllTodosFromFile() {
//   // 打开文件以读取模式
//   File file = LittleFS.open("/todos.txt", "r");
//   if (file) {
//     String line;
//     while (file.available()) {
//       line = file.readStringUntil('\n');
//       Todo todo;
//       todo.id = line.substring(0, line.indexOf('\t')).toInt();
//       line = line.substring(line.indexOf('\t') + 1);
//       todo.start = line.substring(0, line.indexOf('\t'));
//       line = line.substring(line.indexOf('\t') + 1);
//       todo.end = line.substring(0, line.indexOf('\t'));
//       line = line.substring(line.indexOf('\t') + 1);
//       todo.level = line.substring(0, line.indexOf('\t')).toInt();
//       todo.info = line.substring(line.indexOf('\t') + 1);

//       // 将 Todo 对象添加到 vector 中
//       todos.push_back(todo);
//       Serial.println(line);
//     }
//     file.close();
//   } else {
//     Serial.println("Failed to open file for reading.");
//   }
// }



// void handleReadAll() {
//   readAllTodosFromFile();
//   server.send(200, "text/plain", "All records read and displayed.");
// }
// void setup()
// {
//     Serial.begin(115200);

//     // 连接到WiFi网络
//     WiFi.begin(ssid, password);
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         delay(1000);
//         Serial.println("Connecting to WiFi...");
//     }
//     Serial.println("Connected to WiFi");
//     Serial.println(WiFi.localIP().toString().c_str());

//     // 初始化 LittleFS
//     if (LittleFS.begin())
//     {
//         Serial.println("LittleFS initialized.");
//     }
//     else
//     {
//         Serial.println("Failed to initialize LittleFS.");
//     }

//     // 设置路由
//     server.on("/uploadjson", HTTP_ANY, handleUpload);
//     server.on("/readall", HTTP_ANY, handleReadAll);
//     server.begin();
// }

// void loop()
// {
//     server.handleClient();
// }
