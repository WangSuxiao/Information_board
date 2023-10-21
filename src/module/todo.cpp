/*
 * @Author       : WangSuxiao
 * @Date         : 2023-09-28 15:59:22
 * @LastEditTime : 2023-10-14 21:58:49
 * @Description  : 绘制TODO
 * @Tips         :
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include "module/todo.h"
#include "config.h"
#include "module/todoManager.h"

/**
 * @description: 以“时间 + 信息”的形式绘制文件中的条目
 * @param {sFONT} *time_font : 用以绘制时间的字体
 * @param {file_char} *cn_font : 用以绘制详细信息的汉字
 * @param {sFONT} *en_font : 用以绘制详细信息的英文
 * @param {int} start : 本次绘制的条目开始的索引
 * @param {int} paddingTop : 两个条目的间隔
 * @return {Any}
 */
UWORD drawTODO_OnePage(sFONT *time_font, file_char *cn_font, sFONT *en_font, int start, TodoManager &todoManager, int paddingTop)
{
    if (paddingTop < 0)
    {
        // 未指定，则下一条距离上条的间距为 高度的一半
        paddingTop = time_font->Height / 2;
    }
    Serial.println("===================== DrawTODO ======================");
    // 1. 读取配置文件
    File file = LittleFS.open(todoManager.getTodoFilename(), "r");

    // for (size_t i = start ; i < todoManager.getSize(); i++)
    // {
    //     Todo &todo = todoManager.getTodos()[i];
    //     std::cout << "Todo " << i << " - ID: " << todo.id << ", Start: " << todo.start << ", End: " << todo.end << ", Level: " << static_cast<int>(todo.level) << ", Offset: " << todo.offset << std::endl;
    // }

    int nextLine = -1;
    if (file)
    {
        String line;
        nextLine = 0;                     // 下一个要绘制的todo条目
        UWORD next_char = 0;              // 已经绘制的字节数
        UWORD use_line = 0;               // 已经绘制的字节数
        UWORD next_y = DRAW_TODO_Y_START; // 指向下一个开始行
        UWORD draw_result;                // 绘制单个item的返回值
        while (file.available())
        {
            line = file.readStringUntil('\n');
            nextLine = nextLine + 1;
            if (nextLine <= start)
            {
                // nextLine指向下一次要绘制的条目索引
                // 如果和下一次和开始的索引相同
                // 那么本次也应该跳过，即不绘制
                continue;
            }
            if (next_y + time_font->Height + paddingTop + cn_font->Height > DRAW_TODO_Y_END)
            {
                // 无法绘制 info 则退出，nextLine回退
                nextLine = nextLine - 1;
                break;
            }

            int ltIndex_1 = -1;
            int ltIndex_2 = -1;
            Todo todo;
            String info;

            ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
            todo.id = line.substring(ltIndex_1 + 1, ltIndex_2).toInt();
            ltIndex_1 = ltIndex_2;

            ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
            // todo.start = line.substring(ltIndex_1 + 1, ltIndex_2);
            line.substring(ltIndex_1 + 1, ltIndex_2);
            ltIndex_1 = ltIndex_2;

            ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
            // todo.end = line.substring(ltIndex_1 + 1, ltIndex_2);
            line.substring(ltIndex_1 + 1, ltIndex_2);
            ltIndex_1 = ltIndex_2;

            ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
            todo.level = line.substring(ltIndex_1 + 1, ltIndex_2).toInt();
            ltIndex_1 = ltIndex_2;

            ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
            // todo.info = line.substring(ltIndex_1 + 1, ltIndex_2);
            line.substring(ltIndex_1 + 1, ltIndex_2);
            ltIndex_1 = ltIndex_2;

            Serial.println("=======================================");
            Serial.println(todo.id * 100 + todo.level);
            Serial.println(todo.start);
            Serial.println(todo.end);
            // Serial.println(todo.info);
            Serial.println("=======================================");

            // String target_str = line;
            // target_str.trim();
            // target_str.replace("\t", " ");
            // draw_result = Paint_DrawString_CN_From_File_V5(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y, EPD_4IN2_HEIGHT, target_str, &PINGFANG12, &Font24, BLACK, WHITE);
            // use_line = (draw_result / 1000);
            // next_y = next_y + use_line * ((PINGFANG12.Height > Font24.Height) ? PINGFANG12.Height : Font24.Height);
            // next_char = draw_result % 1000;

            String time_str = todo.start + " " + todo.end;
            // String info_str = todo.info;
            // info_str.trim();

            Paint_DrawString_EN(DRAW_TODO_X_START, next_y, time_str.c_str(), time_font, BLACK, WHITE);
            // draw_result = Paint_DrawString_CN_From_File_V5(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y + time_font->Height, DRAW_TODO_Y_END, info_str, &PINGFANG12, &Font24, BLACK, WHITE);
            draw_result = Paint_DrawString_CN_From_File_V5(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y + time_font->Height, DRAW_TODO_Y_END, "info_str", &PINGFANG12, &Font24, BLACK, WHITE);
            use_line = (draw_result / 1000);
            next_y = next_y + paddingTop + time_font->Height + use_line * ((PINGFANG12.Height > Font24.Height) ? PINGFANG12.Height : Font24.Height);
            next_char = draw_result % 1000;

            // Serial.print("字符串长度 ： ");
            // Serial.println(target_str.length());
            // Serial.println("已使用行数 : ");
            // Serial.println(use_line);
            // Serial.println("已绘制长度 : ");
            // Serial.println(next_char);
            // Serial.println(draw_result);
            // Serial.print("下次绘制位置 ： ");
            // Serial.println(next_y);

            if (next_y + (cn_font->Height > en_font->Height ? cn_font->Height : en_font->Height) > EPD_4IN2_HEIGHT)
            {
                // 没有剩余空间,结束绘制，返回下一个要绘制的条目索引
                // 如果本条绘制完成，指向下一个索引
                // 如果本条没有绘制完成，仍指向本条目
                // nextLine = (next_char == info_str.length()) ? nextLine : nextLine - 1;
                // nextLine = (next_char == info_str.length()) ? nextLine : nextLine - 1;
                break;
            }
        }
    }
    else
    {
        Serial.println("Failed to open file for reading.");
    }
    file.close();
    return nextLine;
}
