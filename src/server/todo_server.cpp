/*
 * @Author       : WangSuxiao
 * @Date         : 2023-10-03 15:00:43
 * @LastEditTime : 2023-10-12 21:14:43
 * @Description  : 处理客户端关于TODO的请求，提供CRUD
 *
 *                 改为类的实现方式
 * @Tips         :
 */

#include "config.h"
#include "server/todo_server.h"

#include <EEPROM.h>

/**
 * @description: 初始化todos;nextPosition
 *               读取文件信息，初始化文件指针位置和记录表
 * @param {String&} filename : 存储TODO的文件名
 * @return {Any}
 */
TodoManager::TodoManager(const String &filename) : todoFileName(filename), nextPosition(0)
{
    Serial.println("=============== TodoManager 初始化 ================");
    File file = LittleFS.open(filename, "r");
    EEPROM.get(EEPROMAddressFlag, flag);
    EEPROM.get(EEPROMAddressMode, mode);
    if (file)
    {
        String line;
        String timetmp;
        auto customCompare = [this](const Todo &a, const Todo &b)
        {
            return compareTODO(a, b, this->flag, this->mode);
        };
        while (file.available())
        {
            line = file.readStringUntil('\n');
            Todo todo;
            String info;
            // ID
            todo.id = line.substring(0, line.indexOf('\t')).toInt();
            line = line.substring(line.indexOf('\t') + 1);
            // 开始时间
            timetmp = line.substring(0, line.indexOf('\t'));
            timetmp.trim();
            todo.start = strtoll(timetmp.c_str(), NULL, 10);
            line = line.substring(line.indexOf('\t') + 1);
            // 结束时间
            timetmp = line.substring(0, line.indexOf('\t'));
            timetmp.trim();
            todo.end = strtoll(timetmp.c_str(), NULL, 10);
            line = line.substring(line.indexOf('\t') + 1);
            // 重要等级
            todo.level = line.substring(0, line.indexOf('\t')).toInt();
            line = line.substring(line.indexOf('\t') + 1);
            info = line.substring(line.indexOf('\t') + 1);
            // 偏置
            todo.offset = nextPosition;
            nextPosition = file.position();
            Serial.println("==== 条目信息 ====");
            Serial.print("ID : ");
            Serial.println(todo.id);
            Serial.print("start : ");
            Serial.println(todo.start);
            Serial.print("info : ");
            Serial.println(info);
            Serial.print("todos number : ");

            auto insertPos = std::lower_bound(todos.begin(), todos.end(), todo, customCompare);
            todos.insert(insertPos, todo);
            // todos.push_back(todo);
            Serial.println(todos.size());
            Serial.println();
        }
        file.close();
    }
    else
    {
        file = LittleFS.open(filename, "w");
        file.close();
    }
    Serial.println("================== 初始化 完成 =====================");
    Serial.println();
    Serial.println();
}

/**
 * @description: 检查Todo的ID是否唯一
 * @param {int} idToCheck : 待检查的ID
 * @return {Any} ID不存在则返回True; 反之，返回false
 */
bool TodoManager::isTodoIdUnique(uint32_t idToCheck)
{
    for (Todo todo : todos)
    {
        if (todo.id == idToCheck)
        {
            return false;
        }
    }
    return true;
}

/**
 * @description: 向文件中追加记录
 * @param {Todo} &todo : TODO摘要
 * @param {String} info : TODO详情
 * @return {Any}
 */
bool TodoManager::insertTodoToFile(Todo &todo, const String &info)
{
    // 打开文件以追加模式
    File file = LittleFS.open(todoFileName, "a");
    if (file)
    {
        // 检查是否已存在相同 ID 的记录
        if (isTodoIdUnique(todo.id))
        {
            Serial.print("当前插入条目的ID唯一，允许插入，插入数据为：");
            // 格式化数据并写入文件，用制表符分隔字段
            String data = String(todo.id) + "\t" + String(todo.start) + "\t" + String(todo.end) + "\t" + String(todo.level) + "\t" + info;
            Serial.println(data);
            if (nextPosition != file.position())
            {
                Serial.println("nextPosition != file.position()");
            }
            // 修改todo记录的偏移量
            todo.offset = nextPosition;
            file.seek(nextPosition, SeekSet);

            Serial.print("存储信息：");
            Serial.print("String(todo.id) : ");
            Serial.println(String(todo.id).length());
            Serial.print("todo.start : ");
            Serial.println(String(todo.start).length());
            Serial.print("todo.end : ");
            Serial.println(String(todo.end).length());
            Serial.print("String(todo.level) : ");
            Serial.println(String(todo.level).length());
            Serial.print("todo.info : ");
            Serial.println(info.length());
            Serial.print("整体长度 ：");
            Serial.println(data.length());

            // 存储与文件中
            file.println(data);
            // 指向文件尾部
            nextPosition = file.position();
            auto customCompare = [this](const Todo &a, const Todo &b)
            {
                return compareTODO(a, b, this->flag, this->mode);
            };
            auto insertPos = std::lower_bound(todos.begin(), todos.end(), todo, customCompare);
            todos.insert(insertPos, todo);
            file.close();
            return true;
        }
        else
        {
            // Serial.println("Todo with the same ID already exists.");
            Serial.print("当前插入条目的ID已经存在，不允许插入");
            file.close();
            return false;
        }
        file.close();
    }
    else
    {
        Serial.println("Failed to open file for writing.");
        return false;
    }
}

/**
 * @description: 删除todos中ID为idToDelete的记录
 * @param {int} idToDelete : 待删除的条目ID
 * @return {Any}
 */
bool TodoManager::deleteTodoFromFile(uint32_t idToDelete)
{
    for (std::vector<Todo>::iterator it = todos.begin(); it != todos.end();)
    {
        if (it->id == idToDelete)
        {
            it = todos.erase(it); // 删除值为10的元素
        }
        else
        {
            ++it;
        }
    }
    return isTodoIdUnique(idToDelete);
}

/**
 * @description: 更新todos中ID为updatedTodo的条目
 *          在文件中插入了新的条目，但是没有删除文件中原有的记录
 * @param {Todo} &updatedTodo : todo对象
 * @param {String} &info : todo描述
 * @return {Any}
 */
uint32_t TodoManager::updateTodo(Todo &updatedTodo, const String &info)
{
    // 打开文件以读取和写入模式
    File file = LittleFS.open(todoFileName, "r");
    if (isTodoIdUnique(updatedTodo.id))
    {
        return insertTodoToFile(updatedTodo, info);
    }
    else
    {
        // 删除记录
        deleteTodoFromFile(updatedTodo.id);
        // 插入新的记录
        return insertTodoToFile(updatedTodo, info);
    }
}

/**
 * @description: 清理文件中的无用记录
 * @return {Any}
 */
bool TodoManager::cleanFile()
{
    // 遍历todos

    // 根据todo记录的偏移位置读取一行

    // 将存入新的文件中

    // 修改todo记录的偏移位置

    File newFile = LittleFS.open("tmpTodoFile.txt", "w");
    if (!newFile)
    {
        Serial.println("Failed to open new file for writing.");
        return false;
    }
    File oldFile = LittleFS.open(todoFileName, "r");
    if (!oldFile)
    {
        Serial.println("Failed to open old file for reading.");
        return false;
    }
    for (Todo &todo : todos)
    {
        // 设置文件指针位置为todo记录的偏移位置
        oldFile.seek(todo.offset, SeekSet);
        // 读取一行数据
        String line = oldFile.readStringUntil('\n');
        // 修改todo记录的偏移位置
        todo.offset = newFile.position();
        // 将读取的行写入新文件
        newFile.println(line);
    }
    // 关闭旧文件
    oldFile.close();
    // 关闭新文件
    newFile.close();
    LittleFS.remove(todoFileName);
    LittleFS.rename("/temp.txt", todoFileName);
    // 返回处理的记录数
    return true;
}

void TodoManager::printAll()
{
    Serial.println("==================== Print All ===================");
    File file = LittleFS.open(todoFileName, "r");
    if (!file)
    {
        Serial.println("Failed to open data file.");
        return;
    }
    Serial.println("===== Vector中的数据 : ");
    // Serial.print("记录条数：");
    // Serial.print(todos.size());
    for (Todo &todo : todos)
    {
        file.seek(todo.offset, SeekSet);
        String line = file.readStringUntil('\n');
        Serial.print("ID : ");
        Serial.println(todo.id);
        Serial.print("Start : ");
        Serial.println(todo.start);
        Serial.println("读取到的数据 :");
        Serial.println(line);
    }
    Serial.println("======== 文件中的数据 : ");
    String line;
    file.seek(0, SeekSet);
    while (file.available())
    {
        line = file.readStringUntil('\n');
        Serial.println(line);
    }
    file.close();
    Serial.println();
    Serial.println("================= Print All Finish================");
    Serial.println();
    Serial.println();
}

/**
 * @description: vector<TODO>的排序
 * @param {Todo} &a :
 * @param {Todo} &b :
 * @param {int} flag :xxxx，其中x的取值如下
 *              1 : level
 *              2 : end
 *              3 : id
 *              4 : start
 * @param {int} mode :xxxx，其中x的取值如下
 *              1 : 升序
 *              0 : 降序
 * @return {Any}
 */
bool compareTODO(const Todo &a, const Todo &b, int flag = 0, int mode = 0)
{
    uint8_t currentFlag = flag % 10; // 取出最后一位
    flag = flag / 10;                // 去除最后一位
    uint8_t currentMode = mode % 10;
    mode = mode / 10;
    while (currentFlag != 0)
    {
        switch (currentFlag)
        {
        case 1:
            if (a.level != b.level)
            {
                if (currentMode == 1)
                {
                    return a.level < b.level;
                }
                return a.level > b.level;
            }
            break;
        case 2:
            if (a.end != b.end)
            {
                if (currentMode == 1)
                {
                    return a.end < b.end;
                }
                return a.end > b.end;
            }
            break;
        case 3:
            if (a.id != b.id)
            {
                if (currentMode == 1)
                {
                    return a.id < b.id;
                }
                return a.id > b.id;
            }
            break;
        case 4:
            if (a.start != b.start)
            {
                if (currentMode == 1)
                {
                    return a.start < b.start;
                }
                return a.start > b.start;
            }
            break;
        }
        currentFlag = flag % 10;
        flag = flag / 10;
        currentMode = mode % 10;
        mode = mode / 10;
    }
    return a.id < b.id;
}

void handleUpload(DynamicJsonDocument &doc, ESP8266WebServer &server, TodoManager &todoManager)
{
    String json_data = server.arg("plain");
    DeserializationError error = deserializeJson(doc, json_data);

    if (error)
    {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        server.send(400, "text/plain", "Failed to parse JSON");
    }
    else
    {
        String method = doc["method"];
        String key = doc["key"];

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
            todoManager.insertTodoToFile(todo, todoMSG);
        }
        else if (method == "delete")
        {
            todoManager.deleteTodoFromFile(todo.id);
        }
        else if (method == "update")
        {
            todoManager.updateTodo(todo, todoMSG);
        }

        server.send(200, "text/plain", "Operation completed successfully.");
    }
}

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
//         // todo.info = line.substring(line.indexOf('\t') + 1);

//         JsonObject todoObj = data.createNestedObject();
//         todoObj["id"] = todo.id;
//         todoObj["start"] = todo.start;
//         todoObj["end"] = todo.end;
//         todoObj["level"] = todo.level;
//         todoObj["info"] = line.substring(line.indexOf('\t') + 1);

//         Serial.println("ID: " + String(todo.id));
//         Serial.println("Start: " + todo.start);
//         Serial.println("End: " + todo.end);
//         Serial.println("Level: " + String(todo.level));
//         Serial.println("Info: " + todoObj["info"] );

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
//     // File file = LittleFS.open(todoFileName, "r");
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
