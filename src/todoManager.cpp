// /*
//  * @Author       : WangSuxiao
//  * @Date         : 2023-10-09 10:53:01
//  * @LastEditTime : 2023-10-12 21:39:41
//  * @Description  : 测试TodoManager 和 EEPROM
//  * @Tips         :
//  */
// #include "board/DEV_Config.h"
// #include "board/EPD.h"
// #include "board/GUI_Paint.h"
// #include <stdlib.h>

// #include <TaskScheduler.h>
// #include <EEPROM.h>

// #include "ui/drawBase.h"
// #include "module/lbs.h"
// #include "module/weather.h"
// #include "module/timeUtil.h"
// #include <ArduinoJson.h>
// #include "module/motto.h"
// #include "resource/icon.h"
// #include "module/wificonfig.h"
// #include "server/todo_server.h"
// #include "module/todo.h"


// void setup()
// {
//     DEV_Module_Init();         // 文件系统、串口、SPI、GPIO初始化

//     wifiConnect();

//     EEPROM.begin(512);
//     EEPROM.put(EEPROMAddressFlag, 31);
//     EEPROM.put(EEPROMAddressMode, 11);
//     EEPROM.commit();

//     TodoManager todoManager = TodoManager("todos.txt");


//     Todo todo ;
//     todo.id = 1004;
//     todo.start = 1697000000;
//     todo.end = 1697000000 ;
//     todo.level = 2;
//     todoManager.insertTodoToFile(todo,"测试用例test Example测试用例test Example测试用例test Example测试用例test Example");
//     todo.id = 1005;
//     todo.start = 1679000000;
//     todo.end = 1679000000 ;
//     todo.level = 1;
//     todoManager.insertTodoToFile(todo,"测试用例test Example测试用例test Example测试用例test Example测试用例test Example");
//     // todo.id = 1002;
//     // todo.start =1696979424;
//     // todo.end = 1696999424 ;
//     // todoManager.insertTodoToFile(todo,"test Example测试用例test Example测试用例test Example测试用例test Example测试用例");
//     todoManager.printAll();
//     udp.begin(1127);
// }

// void loop()
// {
//     time_t t = getNtpTime();
//     Serial.println(t);
//     delay(1500);
// }
