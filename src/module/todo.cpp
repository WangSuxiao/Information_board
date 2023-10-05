/*
 * @Author       : WangSuxiao
 * @Date         : 2023-09-28 15:59:22
 * @LastEditTime : 2023-10-05 20:20:17
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
    UWORD next_str = 0;               // 已经绘制的字节数
    UWORD use_line = 0;               // 已经绘制的字节数
    UWORD next_y = y_start;           // 指向下一个开始行
    UWORD draw_result;                // 绘制单个item的返回值
    UWORD return_index = item_sum;    // 绘制完成的条目数
    for (int index = current_item; index < item_sum; index++)
    {
        String target_str = "一二三四五01234六七八九十56789一二三四五01234六七八九十567890123456789";
        draw_result = Paint_DrawString_CN_From_File_V4(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y, EPD_4IN2_HEIGHT, target_str, &PINGFANG12, &Font24, BLACK, WHITE);
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



UWORD draw_TODO_onepage_(int y_start, int item_sum, int current_item, file_char *cn_font, sFONT *en_font)
{
    UWORD next_str = 0;               // 已经绘制的字节数
    UWORD next_y = DRAW_TODO_Y_START;           // 指向下一个开始行
    UWORD draw_result;                // 绘制单个item的返回值
    UWORD return_index = item_sum;    // 绘制完成的条目数
    for (int index = current_item; index < item_sum; index++)
    {
        String target_str = String(index) + "三：是测试abc文字这是测试cfsdg文字这是测试文字这是测试文字这是测试文字";
        // current_y = Paint_DrawString_CN_From_File_V3(DRAW_TODO_X_START, current_y, String(index) + "三：是测试abc文字这是测试cfsdg文字这是测试文字这是测试文字这是测试文字", &PINGFANG12, &Font24, BLACK, WHITE);
        draw_result = Paint_DrawString_CN_From_File_V4(DRAW_TODO_X_START, DRAW_TODO_X_END, next_y, EPD_4IN2_HEIGHT, target_str, &PINGFANG12, &Font24, BLACK, WHITE);
        next_y = next_y + (draw_result / 1000) * ((PINGFANG12.Height > Font24.Height) ? PINGFANG12.Height : Font24.Height);
        next_str = draw_result % 1000;
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
