// /*
//  * @Author       : WangSuxiao
//  * @Date         : 2023-10-09 10:53:01
//  * @LastEditTime : 2023-10-09 12:52:48
//  * @Description  : 测试时间戳转String
//  * @Tips         :
//  */
// #include "board/DEV_Config.h"
// #include "board/EPD.h"
// #include "board/GUI_Paint.h"
// #include <stdlib.h>

// #include <TaskScheduler.h>

// #include "ui/drawBase.h"
// #include "module/lbs.h"
// #include "module/weather.h"
// #include "module/timeUtil.h"
// #include <ArduinoJson.h>
// #include "module/motto.h"
// #include "resource/icon.h"
// #include "module/wificonfig.h"
// // #include "server/webserver.h"
// #include "server/todo_server.h"
// #include "module/todo.h"


// void setup()
// {
//     DEV_Module_Init();         // 文件系统、串口、SPI、GPIO初始化

//     wifiConnect();
//     udp.begin(1127);
// }

// void loop()
// {
//     time_t t = getNtpTime();
//     // 查看时间戳的格式
//     Serial.println(t);
//     delay(1500);
// }
