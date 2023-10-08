/*
 * @Author       : WangSuxiao
 * @Date         : 2023-09-28 15:59:22
 * @LastEditTime : 2023-10-07 21:27:32
 * @Description  : 绘制TODO
 * @Tips         :
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include "module/todo.h"
#include "config.h"

/**
 * @description:【测试; 模板】 Paint_DrawString_CN_From_File_V4的测试函数
 * @param {int} y_start :
 * @param {int} item_sum : 条目的总数
 * @param {int} current_item : 当下要绘制的索引【从0开始】
 * @param {file_char} *cn_font :
 * @param {sFONT} *en_font :
 * @return {UWORD}  下一次要绘制的索引
 */
UWORD draw_TODO_onepage(int y_start, int item_sum, int current_item, file_char *cn_font, sFONT *en_font)
{
    UWORD next_str = 0;            // 已经绘制的字节数
    UWORD use_line = 0;            // 已经绘制的字节数
    UWORD next_y = y_start;        // 指向下一个开始行
    UWORD draw_result;             // 绘制单个item的返回值
    UWORD return_index = item_sum; // 绘制完成的条目数
    for (int index = current_item; index < item_sum; index++)
    {
        String target_str = "一二三四五01234六七八九十56789一二三四五01234六七八九十567890123456789";
        draw_result = Paint_DrawString_CN_From_File_V5(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y, EPD_4IN2_HEIGHT, target_str, &PINGFANG12, &Font24, BLACK, WHITE);
        use_line = (draw_result / 1000);
        next_y = next_y + use_line * ((PINGFANG12.Height > Font24.Height) ? PINGFANG12.Height : Font24.Height);
        next_str = draw_result % 1000;

        Serial.print("字符串长度 ： ");
        Serial.println(target_str.length());
        Serial.println("已使用行数 : ");
        Serial.println(use_line);
        Serial.println("已绘制长度 : ");
        Serial.println(next_str);
        Serial.println(draw_result);
        Serial.print("下次绘制位置 ： ");
        Serial.println(next_y);

        // 102 = 9
        // if (next_str == target_str.length())
        // {
        //     // 如果本条绘制完成，指向下一个索引
        //     // 如果本条没有绘制完成，仍指向本条目
        //     return_index = (next_str == target_str.length()) ? index + 1 : index;
        // }

        if (next_y + (cn_font->Height > en_font->Height ? cn_font->Height : en_font->Height) > EPD_4IN2_HEIGHT)
        {
            // 没有剩余空间,结束绘制，返回下一个要绘制的条目索引
            // 如果本条绘制完成，指向下一个索引
            // 如果本条没有绘制完成，仍指向本条目
            return_index = (next_str == target_str.length()) ? index + 1 : index;
            break;
        }
    }
    return return_index;
}

int getTODOQuantity()
{
    int lineCount = 0;
    File file = LittleFS.open(TODO_FILE, "r");
    while (file.available())
    {
        file.readStringUntil('\n');
        lineCount++; // 每次读取一行，增加行数计数
    }
    file.close();
    return lineCount;
}
/**
 * @description:
 * @param {int} start : 本次绘制的起始index
 * @return {UWORD}
 *      下一个要绘制的todo的index
 *      -1 ：文件处理失败
 *      < start : 条目数量不对
 */
UWORD drawTODO_OnePage(file_char *cn_font, sFONT *en_font, int start)
{
    Serial.println("drawTODO_OnePage : ");
    // 1. 读取配置文件
    File file = LittleFS.open(TODO_FILE, "r");
    int nextLine = -1;
    // int ltIndex = -1;              // line中的\t的index
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
            // 分隔
            // ltIndex = -1;       // 可以省略，
            // while (true)
            // {
            //     ltIndex = line.indexOf("\t", ltIndex + 1);
            //     if (ltIndex == -1)
            //     {
            //         break;
            //     }
            //     Serial.println(String(ltIndex));
            // }

            int ltIndex_1 = -1;
            int ltIndex_2 = -1;
            Todo todo;

            ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
            // Serial.println(line.substring(ltIndex_1 + 1, ltIndex_2));
            todo.id = line.substring(ltIndex_1 + 1, ltIndex_2).toInt();
            ltIndex_1 = ltIndex_2;

            ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
            // Serial.println(line.substring(ltIndex_1 + 1, ltIndex_2));
            todo.start = line.substring(ltIndex_1 + 1, ltIndex_2);
            ltIndex_1 = ltIndex_2;

            ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
            // Serial.println(line.substring(ltIndex_1 + 1, ltIndex_2));
            todo.end = line.substring(ltIndex_1 + 1, ltIndex_2);
            ltIndex_1 = ltIndex_2;

            ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
            todo.level = line.substring(ltIndex_1 + 1, ltIndex_2).toInt();
            ltIndex_1 = ltIndex_2;

            ltIndex_2 = line.indexOf('\t', ltIndex_1 + 1);
            // Serial.println(line.substring(ltIndex_1 + 1, ltIndex_2));
            todo.info = line.substring(ltIndex_1 + 1, ltIndex_2);
            ltIndex_1 = ltIndex_2;

            Serial.println("=======================================");
            Serial.println(todo.id * 100 + todo.level);
            Serial.println(todo.start);
            Serial.println(todo.end);
            Serial.println(todo.info);
            Serial.println("=======================================");
            // Todo todo;
            // todo.id = line.substring(0, line.indexOf('\t',ltIndex)).toInt();
            // line = line.substring(line.indexOf('\t') + 1);
            // todo.start = line.substring(0, line.indexOf('\t'));
            // line = line.substring(line.indexOf('\t') + 1);
            // todo.end = line.substring(0, line.indexOf('\t'));
            // line = line.substring(line.indexOf('\t') + 1);
            // todo.level = line.substring(0, line.indexOf('\t')).toInt();
            // todo.info = line.substring(line.indexOf('\t') + 1);

            // Serial.print("本条读取的数据 : ");
            // Serial.println(line);

            String target_str = line;
            target_str.trim();
            target_str.replace("\t", " ");
            draw_result = Paint_DrawString_CN_From_File_V5(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y, EPD_4IN2_HEIGHT, target_str, &PINGFANG12, &Font24, BLACK, WHITE);
            use_line = (draw_result / 1000);
            next_y = next_y + use_line * ((PINGFANG12.Height > Font24.Height) ? PINGFANG12.Height : Font24.Height);
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
                nextLine = (next_char == target_str.length()) ? nextLine : nextLine - 1;
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
    // 2. 绘制

    // int item_sum;
    // int current_item;
    // UWORD next_str = 0;               // 已经绘制的字节数
    // UWORD use_line = 0;               // 已经绘制的字节数
    // UWORD next_y = DRAW_TODO_Y_START; // 指向下一个开始行
    // UWORD draw_result;                // 绘制单个item的返回值
    // UWORD return_index = item_sum;    // 绘制完成的条目数
    // for (int index = current_item; index < item_sum; index++)
    // {
    //     String target_str = "一二三四五01234六七八九十56789一二三四五01234六七八九十567890123456789";
    //     draw_result = Paint_DrawString_CN_From_File_V5(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y, EPD_4IN2_HEIGHT, target_str, &PINGFANG12, &Font24, BLACK, WHITE);
    //     use_line = (draw_result / 1000);
    //     next_y = next_y + use_line * ((PINGFANG12.Height > Font24.Height) ? PINGFANG12.Height : Font24.Height);
    //     next_str = draw_result % 1000;

    //     Serial.print("字符串长度 ： ");
    //     Serial.println(target_str.length());
    //     Serial.println("已使用行数 : ");
    //     Serial.println(use_line);
    //     Serial.println("已绘制长度 : ");
    //     Serial.println(next_str);
    //     Serial.println(draw_result);
    //     Serial.print("下次绘制位置 ： ");
    //     Serial.println(next_y);

    //     // 102 = 9
    //     // if (next_str == target_str.length())
    //     // {
    //     //     // 如果本条绘制完成，指向下一个索引
    //     //     // 如果本条没有绘制完成，仍指向本条目
    //     //     return_index = (next_str == target_str.length()) ? index + 1 : index;
    //     // }

    //     if (next_y + (cn_font->Height > en_font->Height ? cn_font->Height : en_font->Height) > EPD_4IN2_HEIGHT)
    //     {
    //         // 没有剩余空间,结束绘制，返回下一个要绘制的条目索引
    //         // 如果本条绘制完成，指向下一个索引
    //         // 如果本条没有绘制完成，仍指向本条目
    //         return_index = (next_str == target_str.length()) ? index + 1 : index;
    //         break;
    //     }
    // }
    // return return_index;
}

// int sum = getTODOQuantity();
//     Serial.println("条目总和:");
//     Serial.println(sum);
//     UWORD index = 0;
//     while (index < sum)
//     {
//         EPD_4IN2_Init_Fast();
//         Serial.println("开始的索引 :");
//         Paint_Clear(WHITE);
//         Serial.println(index);
//         index = drawTODO_OnePage(&PINGFANG12, &Font24, index);
//         EPD_4IN2_Display(BlackImage);
//         Serial.println("下一次的索引 :");
//         Serial.println(index);
//         EPD_4IN2_Sleep();
//         delay(10 * 1000);
//     }
