/*
 * @Author       : WangSuxiao
 * @Date         : 2023-10-03 15:00:43
 * @LastEditTime : 2023-10-14 21:47:41
 * @Description  : 处理客户端关于TODO的请求，提供CRUD
 *
 *                 改为类的实现方式
 * @Tips         :
 */

#include "module/todoManager.h"

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

/**
 * @description: 初始化todos;nextPosition
 *               读取文件信息，初始化文件指针位置和记录表
 * @param {String&} filename : 存储TODO的文件名
 * @return {Any}
 */
TodoManager::TodoManager(const String &filename) : todoFileName(filename)
{
    Serial.println("=============== TodoManager 初始化 ================");
    File file = LittleFS.open(filename, "r");
    EEPROM.get(EEPROMAddressFlag, flag);
    EEPROM.get(EEPROMAddressMode, mode);
    if (file)
    {
        String line;
        String timetmp;
        String info;
        int ltindex;
        // 调用外部的比较函数
        auto customCompare = [this](const Todo &a, const Todo &b)
        {
            return compareTODO(a, b, this->flag, this->mode);
        };
        while (file.available())
        {
            Todo todo;
            todo.offset = file.position();
            line = file.readStringUntil('\n');
            Serial.println("==== 文件记录 ====");
            Serial.println(line);
            // 解析todo
            ltindex = line.indexOf('\t');
            todo.id = line.substring(0, ltindex).toInt();
            line = line.substring(ltindex + 1); // 截取后半段

            ltindex = line.indexOf('\t');
            timetmp = line.substring(0, ltindex);
            timetmp.trim(); // 去除首尾空格
            todo.start = strtoll(timetmp.c_str(), NULL, 10);
            line = line.substring(ltindex + 1);

            ltindex = line.indexOf('\t');
            timetmp = line.substring(0, ltindex);
            timetmp.trim();
            todo.end = strtoll(timetmp.c_str(), NULL, 10);
            line = line.substring(ltindex + 1);

            ltindex = line.indexOf('\t');
            todo.level = line.substring(0, ltindex).toInt();

            info = line.substring(ltindex + 1); // 截取后半段
            info.trim();
            // 偏置
            Serial.println("==== 条目信息 ====");
            Serial.print("ID : ");
            Serial.println(todo.id);
            Serial.print("Start : ");
            Serial.println(todo.start);
            Serial.print("End : ");
            Serial.println(todo.end);
            Serial.print("Level : ");
            Serial.println(todo.level);
            Serial.print("Info : ");
            Serial.println(info);
            Serial.print("Todos Size : ");
            // 获取插入位置
            auto insertPos = std::lower_bound(todos.begin(), todos.end(), todo, customCompare);
            todos.insert(insertPos, todo);
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
 * @description: 检查Todo的ID在todos中是否唯一
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
 * @description: 向文件和todos中追加记录
 * @param {Todo} &todo : TODO摘要
 * @param {String} info : TODO详情
 * @return {Any}
 */
bool TodoManager::insertTodo(Todo &todo, const String &info)
{
    // 打开文件以追加模式
    File file = LittleFS.open(todoFileName, "a");
    Serial.print("================== 插入文件记录  ===================");
    if (file)
    {
        // 检查是否已存在相同 ID 的记录
        if (isTodoIdUnique(todo.id))
        {
            Serial.println("ID唯一，允许插入，插入数据为：");
            // 格式化数据并写入文件，用制表符分隔字段
            String data = String(todo.id) + "\t" + String(todo.start) + "\t" + String(todo.end) + "\t" + String(todo.level) + "\t" + info;
            Serial.println(data);
            // 修改todo记录的偏移量
            todo.offset = file.position();
            file.seek(todo.offset, SeekSet);

            Serial.println("存储信息：");
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
            Serial.println("ID已经存在，不允许插入");
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
    Serial.println("===================================================");
    Serial.println();
    Serial.println();
}

/**
 * @description: 删除todos中ID为idToDelete的记录
 * @param {int} idToDelete : 待删除的条目ID
 * @return {Any}
 */
bool TodoManager::deleteTodo(uint32_t idToDelete)
{
    for (std::vector<Todo>::iterator it = todos.begin(); it != todos.end();)
    {
        if (it->id == idToDelete)
        {
            it = todos.erase(it);
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
        return insertTodo(updatedTodo, info);
    }
    else
    {
        // 删除记录
        deleteTodo(updatedTodo.id);
        // 插入新的记录
        return insertTodo(updatedTodo, info);
    }
}

/**
 * @description: 清理文件中的无用记录
 * @return {Any}
 */
bool TodoManager::cleanFile()
{
    String tmpfile = "/tmpTodoFile.txt";
    File newFile = LittleFS.open(tmpfile, "w");
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
    // 遍历todos
    for (Todo &todo : todos)
    {
        // 根据todo记录的偏移位置读取一行
        oldFile.seek(todo.offset, SeekSet);
        String line = oldFile.readStringUntil('\n');
        // 修改todo记录的偏移位置
        todo.offset = newFile.position();
        // 将存入新的文件中
        newFile.println(line);
    }
    // 关闭文件
    oldFile.close();
    newFile.close();
    LittleFS.remove(todoFileName);
    LittleFS.rename(tmpfile, todoFileName);
    return true;
}

/**
 * @Author: WangSuxiao
 * @description: 获取存储文件名
 * @return {String}
 */
String TodoManager::getTodoFilename()
{
    return todoFileName;
}

/**
 * @Author: WangSuxiao
 * @description: 获取todos
 * @return {vector<Todo>}
 */
std::vector<Todo> TodoManager::getTodos()
{
    return todos;
}

/**
 * @Author: WangSuxiao
 * @description: 获取todos的尺寸
 * @return {int}
 */
int TodoManager::getSize()
{
    return todos.size();
}

/**
 * @Author: WangSuxiao
 * @description: 根据ID获取todo的info
 * @param {uint32_t} id : todo的ID
 * @return {String}
 */
String TodoManager::getTodoInfo(uint32_t id)
{
    Todo todo = getTodobyID(id);
    if (todo.id > 0)
    {
        File file = LittleFS.open(todoFileName, "r");
        if (file)
        {
            file.seek(todo.offset, SeekSet);
            return "";
        }
    }
    return "";
}

/**
 * @Author: WangSuxiao
 * @description: 根据ID获取todo
 * @param {uint32_t} id :
 * @return {Any}
 */
Todo TodoManager::getTodobyID(uint32_t id)
{
    for (const Todo &todo : todos)
    {
        if (todo.id == id)
        {
            return todo;
        }
    }

    Todo emptyTodo = {0, 0, 0, 0, 0};
    return emptyTodo;
}

/**
 * @Author: WangSuxiao
 * @description: 根据ID获取todo
 * @param {uint32_t} id :
 * @return {Any}
 */
Todo TodoManager::getTodobyIndex(uint32_t index)
{
    if (index < 0 || index >= todos.size()){
        return todos[index];
    }
    Todo emptyTodo = {0, 0, 0, 0, 0};
    return emptyTodo;
}

/**
 * @Author: WangSuxiao
 * @description: 打印板子中存储的全部数据
 * @return {Any}
 */
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