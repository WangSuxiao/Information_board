/*
 * @Author       : WangSuxiao
 * @Date         : 2023-09-28 15:59:22
 * @LastEditTime : 2023-10-29 16:33:25
 * @Description  : 绘制TODO
 * @Tips         :
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include "module/todo.h"
#include "config.h"
#include "module/todoManager.h"

/**
 * @description: 以“时间 + 信息”的形式绘制todos中的条目
 * @param {sFONT} *time_font : 用以绘制时间的字体
 * @param {file_char} *cn_font : 用以绘制详细信息的汉字
 * @param {sFONT} *en_font : 用以绘制详细信息的英文
 * @param {int} start : 本次绘制的条目开始的索引
 * @param {int} paddingTop : 两个条目的间隔
 * @return {Any}
 *              文件打开失败，返回-1；否则返回下一个应当绘制的item的下标
 */
UWORD drawTODO_OnePage(file_char_acsll *time_font, file_char *cn_font, sFONT *en_font, int start, TodoManager &todoManager, int paddingTop)
{
    Serial.println("=============== DrawTODO start");
    if (paddingTop < 0)
    {
        // 未指定，则下一条距离上条的间距为 高度的一半
        paddingTop = time_font->Height / 2;
    }
    // 1. 读取配置文件
    File file = LittleFS.open(todoManager.getTodoFilename(), "r");
    // int nextLine = -1;  // 本次绘制过程中，用于记录即将绘制条目
    if (file)
    {
        String line;
        String info;
        String time_str;
        char buffer[17];                  // 用以解析时间戳
        int index = start;                // 待绘制的item的下标
        UWORD draw_result;                // 绘制长字符串的返回值：
        UWORD next_char = 0;              // 绘制长字符串的返回值：已经绘制的字节数
        UWORD use_line = 0;               // 绘制长字符串的返回值：已经绘制的行数
        UWORD next_y = DRAW_TODO_Y_START; // 指向下一个开始行

        UWORD Max_Height = (PINGFANG12.Height > Font24.Height) ? PINGFANG12.Height : Font24.Height;

        std::vector<Todo> todos = todoManager.getTodos();
        for (auto it = todos.begin() + start; it != todos.end(); ++it)
        {

            // 获取迭代器的偏移量
            index = std::distance(todos.begin(), it);
            Serial.print("尝试绘制:");
            Serial.println(index);
            // 判断当前是否有足够空间绘制
            if (next_y + paddingTop + time_font->Height + cn_font->Height > DRAW_TODO_Y_END)
            {
                // 要求至少能够绘制 ：间隔 + 时间信息 + 一行描述
                // 无法绘制 info 则退出
                index = index - 1;
                break;
                // return index;
            }
            Serial.print("偏移量:");
            Serial.println(it->offset);

            // 可以绘制，则读取todo的详细信息
            file.seek(it->offset);
            line = file.readStringUntil('\n');
            Serial.print("读取的数据:");
            Serial.println(line);
            // 解析todo_info
            info = line.substring(line.indexOf("#", 0) + 1);
            Serial.print("解析的数据:");
            Serial.println(info);
            // info = todoManager.getTodoInfo(it->id);

            strftime(buffer, sizeof(buffer), "> %y/%m/%d %H:%M", localtime(&it->start));
            time_str = String(buffer);

            // 绘制信息
            // Paint_DrawString_EN(DRAW_TODO_X_START, next_y, time_str.c_str(), time_font, BLACK, WHITE);
            // Paint_DrawString_EN_From_File(DRAW_TODO_X_START, next_y, time_str.c_str(), time_font, WHITE, BLACK);
            PaintDrawASCIIStringFromFile_V2(DRAW_TODO_X_START, next_y, time_str.c_str(), time_font, WHITE, BLACK);

            // draw_result = Paint_DrawString_CN_From_File_V5(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y + time_font->Height, DRAW_TODO_Y_END, info_str, &PINGFANG12, &Font24, BLACK, WHITE);
            draw_result = Paint_DrawString_CN_From_File_V5(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y + time_font->Height, DRAW_TODO_Y_END, info, &PINGFANG12, &Font24, BLACK, WHITE);
            use_line = (draw_result / 1000);
            next_char = draw_result % 1000;
            next_y = next_y + paddingTop + time_font->Height + use_line * Max_Height;

            // Serial.print("字符串长度 ： ");
            // Serial.println(target_str.length());
            // Serial.println("已使用行数 : ");
            // Serial.println(use_line);
            // Serial.println("已绘制长度 : ");
            // Serial.println(next_char);
            // Serial.println(draw_result);
            // Serial.print("下次绘制位置 ： ");
            // Serial.println(next_y);

            // 检查本条是否绘制完全
            if (next_char < info.length())
            {
                // 如果本条没有绘制完成，仍指向本条目
                index = index - 1;
                break;
                // return index;
            }
        }
        file.close();
        Serial.println("=============== DrawTODO end");
        return index + 1;
    }
    else
    {
        Serial.println("记录文件打开失败");
    }
    Serial.println("=============== DrawTODO end");
    return -1;
}



/**
 * @description:    V2:使用文件中的字模数据
 *                  V1:以“时间 + 信息”的形式绘制todos中的条目
 * @param {sFONT} *time_font : 用以绘制时间的字体
 * @param {file_char} *cn_font : 用以绘制详细信息的汉字
 * @param {sFONT} *en_font : 用以绘制详细信息的英文
 * @param {int} start : 本次绘制的条目开始的索引
 * @param {int} paddingTop : 两个条目的间隔
 * @return {Any}
 *              文件打开失败，返回-1；否则返回下一个应当绘制的item的下标
 */
UWORD drawTODO_OnePage_V2(file_char_acsll *time_font, file_char *cn_font, file_char_acsll  *en_font, int start, TodoManager &todoManager, int paddingTop)
{
    Serial.println("=============== DrawTODO start");
    if (paddingTop < 0)
    {
        // 未指定，则下一条距离上条的间距为 高度的一半
        paddingTop = time_font->Height / 2;
    }
    // 1. 读取配置文件
    File file = LittleFS.open(todoManager.getTodoFilename(), "r");
    // int nextLine = -1;  // 本次绘制过程中，用于记录即将绘制条目
    if (file)
    {
        String line;
        String info;
        String time_str;
        char buffer[17];                  // 用以解析时间戳
        int index = start;                // 待绘制的item的下标
        UWORD draw_result;                // 绘制长字符串的返回值：
        UWORD next_char = 0;              // 绘制长字符串的返回值：已经绘制的字节数
        UWORD use_line = 0;               // 绘制长字符串的返回值：已经绘制的行数
        UWORD next_y = DRAW_TODO_Y_START; // 指向下一个开始行

        UWORD Max_Height = (PINGFANG12.Height > Font24.Height) ? PINGFANG12.Height : Font24.Height;

        std::vector<Todo> todos = todoManager.getTodos();
        for (auto it = todos.begin() + start; it != todos.end(); ++it)
        {

            // 获取迭代器的偏移量
            index = std::distance(todos.begin(), it);
            // Serial.print("尝试绘制:");
            // Serial.println(index);
            // 判断当前是否有足够空间绘制
            if (next_y + paddingTop + time_font->Height + cn_font->Height > DRAW_TODO_Y_END)
            {
                // 要求至少能够绘制 ：间隔 + 时间信息 + 一行描述
                // 无法绘制 info 则退出
                index = index - 1;
                break;
                // return index;
            }
            // Serial.print("偏移量:");
            // Serial.println(it->offset);

            // 可以绘制，则读取todo的详细信息
            file.seek(it->offset);
            line = file.readStringUntil('\n');
            line.trim();
            // Serial.print("读取的数据:");
            // Serial.println(line);
            // 解析todo_info
            info = line.substring(line.indexOf("#", 0) + 1);
            // Serial.print("解析的数据:");
            // Serial.println(info);
            // info = todoManager.getTodoInfo(it->id);
            strftime(buffer, sizeof(buffer), "> %y/%m/%d %H:%M", localtime(&it->start));
            time_str = String(buffer);
            // 绘制信息
            PaintDrawASCIIStringFromFile_V2(DRAW_TODO_X_START, next_y, time_str.c_str(), time_font, WHITE, BLACK);
            draw_result = Paint_DrawString_CN_From_File_V6(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y + time_font->Height, DRAW_TODO_Y_END,  info, &PINGFANG12, &ASCII_Adobe_L14, BLACK, WHITE);

            use_line = (draw_result / 1000);
            next_char = draw_result % 1000;
            next_y = next_y + paddingTop + time_font->Height + use_line * Max_Height;

            // Serial.print("字符串长度 ： ");
            // Serial.println(target_str.length());
            // Serial.println("已使用行数 : ");
            // Serial.println(use_line);
            // Serial.println("已绘制长度 : ");
            // Serial.println(next_char);
            // Serial.println(draw_result);
            // Serial.print("下次绘制位置 ： ");
            // Serial.println(next_y);

            // 检查本条是否绘制完全
            if (next_char < info.length())
            {
                // 如果本条没有绘制完成，仍指向本条目
                index = index - 1;
                break;
                // return index;
            }
        }
        file.close();
        Serial.println("=============== DrawTODO end");
        return index + 1;
    }
    else
    {
        Serial.println("记录文件打开失败");
    }
    Serial.println("=============== DrawTODO end");
    return -1;
}

// int findLastDotIndex(String input, int len, String target)
// {
//     int lastIndex = -1;
//     if ((target[0] & 0xE0) == 0xE0)
//     {
//         for (int i = 0; i < len; i = i + 3)
//         {
//             if (input[i] == target[0] && input[i + 1] == target[1] && input[i + 2] == target[2])
//             {
//                 lastIndex = i;
//             }
//         }
//     }
//     else
//     {
//         for (int i = 0; i < len; i++)
//         {
//             if (input[i] == target[0])
//             {
//                 lastIndex = i;
//             }
//         }
//     }
//     return lastIndex;
// }

int findLastDotIndex(String input, int len, String target)
{
    int lastIndex = -1;
    if ((target[0] & 0xE0) == 0xE0)
    {
        Serial.println("汉字符号");
        for (int i = 0; i < len;)
        {
            if ((input[i] & 0xE0) == 0xE0)
            {
                if (input[i] == target[0] && input[i + 1] == target[1] && input[i + 2] == target[2])
                {
                    lastIndex = i;
                }
                i = i + 3;
            }else{
                i = i + 1;
            }

        }
    }
    else
    {
        Serial.println("英文符号");
        for (int i = 0; i < len; i++)
        {
            if (input[i] == target[0])
            {
                lastIndex = i;
            }
        }
    }
    return lastIndex;
}


/**
 * @Author: WangSuxiao
 * @description:    V3:修正：单条数据超过显示范围的情况
 *                  V2:使用文件中的字模数据
 *                  V1:以“时间 + 信息”的形式绘制todos中的条目
 * @param {file_char_acsll} *time_font :    用以绘制时间的字体
 * @param {file_char} *cn_font :            用以绘制信息的汉字
 * @param {file_char_acsll } *en_font :     用以绘制信息的英文
 * @param {int} itemStart :                 本次绘制的条目开始的索引
 * @param {int} charStart :                 条目开始的字符偏置
 * @param {TodoManager} &两个条目的间隔 :    TODO管理
 * @param {int} paddingTop :                两个条目的间隔
 * @return {Any}
 */
u32_t drawTODO_OnePage_V3(file_char_acsll *time_font, file_char *cn_font, file_char_acsll  *en_font, int itemStart, int charStart, TodoManager &todoManager, int paddingTop)
{
    Serial.println("=============== DrawTODO start");
    if (paddingTop < 0)
    {
        // 未指定，则下一条距离上条的间距为 高度的一半
        paddingTop = time_font->Height / 2;
    }
    // 1. 读取配置文件
    File file = LittleFS.open(todoManager.getTodoFilename(), "r");
    int result = -1;
    if (file)
    {
        String line;
        String info;
        String time_str;
        char buffer[17];                  // 用以解析时间戳
        int index = itemStart;            // 待绘制的item的下标
        UWORD draw_result;                // 绘制长字符串的返回值：
        UWORD next_char = 0;              // 绘制长字符串的返回值：已经绘制的字节数
        UWORD use_line = 0;               // 绘制长字符串的返回值：已经绘制的行数
        UWORD next_y = DRAW_TODO_Y_START; // 指向下一个开始行

        UWORD Max_Height = (PINGFANG12.Height > Font24.Height) ? PINGFANG12.Height : Font24.Height;

        std::vector<Todo> todos = todoManager.getTodos();
        int info_len = 0;
        int index_1 = 0;
        int index_2 = 0;
        int index_3 = 0;
        for (auto it = todos.begin() + itemStart; it != todos.end(); ++it)
        {
            // 获取迭代器的偏移量
            index = std::distance(todos.begin(), it);
            result = index + 1;     // 正常的返回结果，指向下一个条目
            // Serial.print("尝试绘制:");
            // Serial.println(index);
            // 判断当前是否有足够空间绘制
            if (next_y + paddingTop + time_font->Height + cn_font->Height > DRAW_TODO_Y_END)
            {
                // 要求至少能够绘制 ：间隔 + 时间信息 + 一行描述
                // 无法绘制 info 则退出
                // index = index - 1;
                Serial.print("无法绘制本次, index : ");
                result = index;
                Serial.println(result);
                break;
                // return index;
            }
            // Serial.print("偏移量:");
            // Serial.println(it->offset);

            // 可以绘制，则读取todo的详细信息
            file.seek(it->offset);
            line = file.readStringUntil('\n');
            line.trim();
            // Serial.print("读取的数据:");
            // Serial.println(line);
            // 解析todo_info
            Serial.print("开始绘制条目,ID:");
            Serial.println(it->id);

            Serial.println("条目信息 : ");
            info = line.substring(line.indexOf("#", 0) + 1);
            Serial.println(info);
            Serial.print("条目长度 : ");
            info_len = info.length();
            Serial.println(info_len);

            index_1 = findLastDotIndex(info, charStart, ".") + 1;
            index_2 = findLastDotIndex(info, charStart, "。") + 1;
            index_3 = findLastDotIndex(info, charStart, " ")+1;

            Serial.print("原始起始位置: ");
            Serial.println(charStart);
            Serial.print("最近的英文句号: ");
            Serial.println(index_1);
            Serial.print("最近的中文句号: ");
            Serial.println(index_2);
            Serial.print("最近的空格: ");
            Serial.println(index_3);
            index_1 = max(0,index_1);
            index_1 = max(index_2,index_1);
            // index_1 = max(index_3,index_1);
            if(index_1 > max(charStart-150,0)){
                charStart = min(index_1,charStart);
            }
            Serial.print("优化后起始位置: ");
            Serial.println(charStart);
            info = info.substring(charStart);
            // if(charStart > 0){
            //     info = info.substring((info.length() - charStart));
            // }
            Serial.println("draw line : ");
            Serial.println(info);
            // Serial.print("解析的数据:");
            // Serial.println(info);
            // info = todoManager.getTodoInfo(it->id);
            strftime(buffer, sizeof(buffer), "> %y/%m/%d %H:%M", localtime(&it->start));
            time_str = String(buffer);
            // 绘制信息
            PaintDrawASCIIStringFromFile_V2(DRAW_TODO_X_START, next_y, time_str.c_str(), time_font, WHITE, BLACK);
            draw_result = Paint_DrawString_CN_From_File_V6(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y + time_font->Height, DRAW_TODO_Y_END,  info, &PINGFANG12, &ASCII_Adobe_L14, BLACK, WHITE);
            Serial.println("字符串绘制结果 ： ");

            use_line = (draw_result / 1000);
            next_char = draw_result % 1000;
            next_y = next_y + paddingTop + time_font->Height + use_line * Max_Height;
            Serial.print("使用的行数 ： ");
            Serial.println(use_line);
            Serial.print("下一次绘制的字符位置 ： ");
            Serial.println(next_char);
            Serial.println("实际绘制的字符串 : ");
            Serial.println(info.substring(0, next_char));
            Serial.println("还需绘制的字符串 : ");
            Serial.println(info.substring(next_char));

            // Serial.print("字符串长度 ： ");
            // Serial.println(target_str.length());
            // Serial.println("已使用行数 : ");
            // Serial.println(use_line);
            // Serial.println("已绘制长度 : ");
            // Serial.println(next_char);
            // Serial.println(draw_result);
            // Serial.print("下次绘制位置 ： ");
            // Serial.println(next_y);

            // 检查本条是否绘制完全
            if (next_char < info.length())
            {
                // 如果本条没有绘制完成，仍指向本条目
                // index = index - 1;
                result = index  +( next_char + charStart) * 100;
                Serial.println("本条没有绘制完成");
                Serial.print("ID : ");
                Serial.println(index);
                Serial.print("本次的偏置 : ");
                Serial.println( charStart);
                Serial.print("本次的绘制 : ");
                Serial.println(next_char);
                Serial.print("下次的偏置 : ");
                Serial.println( next_char + charStart);
                break;
                // return index;
            }else{
                Serial.println("本条绘制完成");
            }
            charStart = 0;
        }
        file.close();
        Serial.println("=============== DrawTODO end");
        return result;
    }
    else
    {
        Serial.println("记录文件打开失败");
    }
    Serial.println("=============== DrawTODO end");
    return -1;
}

// /**
//  * @description: 以“时间 + 信息”的形式绘制文件中的条目
//  *              【历史版本，以文件形式存储数据】
//  * @param {sFONT} *time_font : 用以绘制时间的字体
//  * @param {file_char} *cn_font : 用以绘制详细信息的汉字
//  * @param {sFONT} *en_font : 用以绘制详细信息的英文
//  * @param {int} start : 本次绘制的条目开始的索引
//  * @param {int} paddingTop : 两个条目的间隔
//  * @return {Any}
//  */
// UWORD drawTODO_OnePage(sFONT *time_font, file_char *cn_font, sFONT *en_font, int start, TodoManager &todoManager, int paddingTop)
// {
//     if (paddingTop < 0)
//     {
//         // 未指定，则下一条距离上条的间距为 高度的一半
//         paddingTop = time_font->Height / 2;
//     }
//     Serial.println("=============== DrawTODO start");
//     // 1. 读取配置文件
//     File file = LittleFS.open(todoManager.getTodoFilename(), "r");
//     int nextLine = -1;  // 本次绘制过程中，用于记录即将绘制条目
//     if (file)
//     {
//         String line;
//         nextLine = 0;                     // 下一个要绘制的todo条目
//         UWORD next_char = 0;              // 已经绘制的字节数
//         UWORD use_line = 0;               // 已经绘制的字节数
//         UWORD next_y = DRAW_TODO_Y_START; // 指向下一个开始行
//         UWORD draw_result;                // 绘制单个item的返回值
//         while (file.available())
//         {
//             line = file.readStringUntil('\n');
//             nextLine = nextLine + 1;
//             if (nextLine <= start)
//             {
//                 // nextLine指向下一次要绘制的条目索引
//                 // 如果和下一次和开始的索引相同
//                 // 那么本次也应该跳过，即不绘制
//                 continue;
//             }
//             if (next_y + time_font->Height + paddingTop + cn_font->Height > DRAW_TODO_Y_END)
//             {
//                 // 无法绘制 info 则退出，nextLine回退
//                 nextLine = nextLine - 1;
//                 break;
//             }

//             int ltIndex_1 = -1;
//             int ltIndex_2 = -1;
//             Todo todo;
//             String info;

//             ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
//             todo.id = line.substring(ltIndex_1 + 1, ltIndex_2).toInt();
//             ltIndex_1 = ltIndex_2;

//             ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
//             // todo.start = line.substring(ltIndex_1 + 1, ltIndex_2);
//             line.substring(ltIndex_1 + 1, ltIndex_2);
//             ltIndex_1 = ltIndex_2;

//             ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
//             // todo.end = line.substring(ltIndex_1 + 1, ltIndex_2);
//             line.substring(ltIndex_1 + 1, ltIndex_2);
//             ltIndex_1 = ltIndex_2;

//             ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
//             todo.level = line.substring(ltIndex_1 + 1, ltIndex_2).toInt();
//             ltIndex_1 = ltIndex_2;

//             ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
//             // todo.info = line.substring(ltIndex_1 + 1, ltIndex_2);
//             line.substring(ltIndex_1 + 1, ltIndex_2);
//             ltIndex_1 = ltIndex_2;

//             Serial.println("=======================================");
//             Serial.println(todo.id * 100 + todo.level);
//             Serial.println(todo.start);
//             Serial.println(todo.end);
//             // Serial.println(todo.info);
//             Serial.println("=======================================");

//             // String target_str = line;
//             // target_str.trim();
//             // target_str.replace("\t", " ");
//             // draw_result = Paint_DrawString_CN_From_File_V5(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y, EPD_4IN2_HEIGHT, target_str, &PINGFANG12, &Font24, BLACK, WHITE);
//             // use_line = (draw_result / 1000);
//             // next_y = next_y + use_line * ((PINGFANG12.Height > Font24.Height) ? PINGFANG12.Height : Font24.Height);
//             // next_char = draw_result % 1000;

//             String time_str = todo.start + " " + todo.end;
//             // String info_str = todo.info;
//             // info_str.trim();

//             Paint_DrawString_EN(DRAW_TODO_X_START, next_y, time_str.c_str(), time_font, BLACK, WHITE);
//             // draw_result = Paint_DrawString_CN_From_File_V5(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y + time_font->Height, DRAW_TODO_Y_END, info_str, &PINGFANG12, &Font24, BLACK, WHITE);
//             draw_result = Paint_DrawString_CN_From_File_V5(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y + time_font->Height, DRAW_TODO_Y_END, "info_str", &PINGFANG12, &Font24, BLACK, WHITE);
//             use_line = (draw_result / 1000);
//             next_y = next_y + paddingTop + time_font->Height + use_line * ((PINGFANG12.Height > Font24.Height) ? PINGFANG12.Height : Font24.Height);
//             next_char = draw_result % 1000;

//             // Serial.print("字符串长度 ： ");
//             // Serial.println(target_str.length());
//             // Serial.println("已使用行数 : ");
//             // Serial.println(use_line);
//             // Serial.println("已绘制长度 : ");
//             // Serial.println(next_char);
//             // Serial.println(draw_result);
//             // Serial.print("下次绘制位置 ： ");
//             // Serial.println(next_y);

//             if (next_y + (cn_font->Height > en_font->Height ? cn_font->Height : en_font->Height) > EPD_4IN2_HEIGHT)
//             {
//                 // 没有剩余空间,结束绘制，返回下一个要绘制的条目索引
//                 // 如果本条绘制完成，指向下一个索引
//                 // 如果本条没有绘制完成，仍指向本条目
//                 // nextLine = (next_char == info_str.length()) ? nextLine : nextLine - 1;
//                 // nextLine = (next_char == info_str.length()) ? nextLine : nextLine - 1;
//                 break;
//             }
//         }
//     }
//     else
//     {
//         Serial.println("Failed to open file for reading.");
//     }
//     file.close();
//     Serial.println("=============== DrawTODO end");
//     return nextLine;
// }
