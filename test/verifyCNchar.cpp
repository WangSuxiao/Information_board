// /*
//  * @Author       : WangSuxiao
//  * @Date         : 2023-05-01 08:15:49
//  * @LastEditTime : 2023-05-01 13:44:55
//  * @Description  : 本文件用于验证汉字字库显示的完整性
//  *                      将字库中的每一个字显示到屏幕中
//  * @Tips         : 
//  */

// #include "edf/DEV_Config.h"
// #include "edf/EPD.h"
// #include "paint/GUI_Paint.h"
// #include "weather.h"
// #include <stdlib.h>
// #include "func/wifiConfig.h"

// void setup()
// {

//     DEV_Module_Init(); // 文件系统、串口、SPI、GPIO初始化
//     EPD_4IN2_Init_Fast();
//     EPD_4IN2_Clear(); // 清屏
//     DEV_Delay_ms(500);

//     // 创建图像缓冲区
//     UBYTE *BlackImage;
//     UWORD imageSize = ((EPD_4IN2_WIDTH % 8 == 0) ? (EPD_4IN2_WIDTH / 8) : (EPD_4IN2_WIDTH / 8 + 1)) * EPD_4IN2_HEIGHT;
//     if ((BlackImage = (UBYTE *)malloc(imageSize)) == NULL)
//     {
//         while (1)
//         {
//             Serial.println("Failed to apply for black memory...\r\n");
//             delay(5000);
//         }
//     }
//     Paint_NewImage(BlackImage, EPD_4IN2_WIDTH, EPD_4IN2_HEIGHT, 0, WHITE);
//     Paint_SelectImage(BlackImage);
//     Paint_Clear(WHITE);

//     // 输出各个汉字
//     uint8_t _Color_Foreground = BLACK;
//     uint8_t _Color_Background = WHITE;
//     int x = 0;
//     int y = 0;
//     int numberofrows = 25;
//     int i, j, m, n;
//     size_t len = numberofrows * 2 * 22; // 16 个汉字  *  每个汉字字模的大小
//     uint8_t dataspace[len];             // 用于读取的缓冲区

//     int rows = 3753 / numberofrows; // 整个字库需要绘制的总行数
//     int hasReadBytes = 0;           // 实际读取到的字节数
//     const unsigned char *ptr = dataspace;

//     size_t freeHeap = ESP.getFreeHeap();
//     Serial.print("Free heap: ");
//     Serial.println(freeHeap);
//     for (n = 0; n < rows; n++)
//     {
//         // 遍历整个字库，每行绘制16个汉字，处理各行
//         hasReadBytes = read(PINGFANG12.data_file, dataspace, len, n * len);
//         if (hasReadBytes % 44 != 0)
//         {
//             // 读取数据不完整
//             Serial.println("The data read is incomplete");
//             return;
//         }
//         Serial.printf("The number of Chinese characters read : ");
//         Serial.println(hasReadBytes / 44);
//         ptr = dataspace;
//         for (m = 0; m < hasReadBytes / 44; m++)
//         {
//             // 处理每行的汉字
//             // Serial.printf("X : ");
//             // Serial.println(x);
//             // Serial.printf("Y : ");
//             // Serial.println(y);
//             // freeHeap = ESP.getFreeHeap();
//             // Serial.print("Free heap: ");
//             // Serial.println(freeHeap);
//             for (j = 0; j < PINGFANG12.Height; j++)
//             {
//                 // Serial.printf("start : ");
//                 // Serial.printf("%d ", j);
//                 for (i = 0; i < PINGFANG12.Width; i++)
//                 {
//                     // Serial.printf("%d ", i);
//                     if (FONT_BACKGROUND == _Color_Background)
//                     { // this process is to speed up the scan
//                         if (*ptr & (0x80 >> (i % 8)))
//                         {
//                             Paint_SetPixel(x + i, y + j, _Color_Foreground);
//                             // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
//                         }
//                     }
//                     else
//                     {
//                         if (*ptr & (0x80 >> (i % 8)))
//                         {
//                             Paint_SetPixel(x + i, y + j, _Color_Foreground);
//                             // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
//                         }
//                         else
//                         {
//                             Paint_SetPixel(x + i, y + j, _Color_Background);
//                             // Paint_DrawPoint(x + i, y + j, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
//                         }
//                     }
//                     if (i % 8 == 7)
//                     {
//                         ptr++;
//                     }
//                 }
//                 if (PINGFANG12.Width % 8 != 0)
//                 {
//                     ptr++;
//                 }
//                 // Serial.printf(" : end");
//                 // Serial.println();
//                 // EPD_4IN2_Display(BlackImage);
//                 // DEV_Delay_ms(500);
//             }

//             // 指向下一个位置，处理X坐标
//             // ptr = ptr + 2 * 22;
//             x += PINGFANG12.Width;
//         }
//         // 换行
//         x = 0;
//         y = y + 22;
//         if ((n + 1) % 13 == 0)
//         {
//             y = 0;
//             EPD_4IN2_Display(BlackImage);
//             DEV_Delay_ms(2500);
//             Paint_Clear(WHITE);
//         }
//     }

//     // 局部刷新
//     EPD_4IN2_Init_Partial();
//     // 局部刷新位置BUG的测试
//     Paint_ClearWindows(20, 180, 20 + Font20.Width * 7, 180 + Font20.Height, BLACK);
//     Paint_DrawString_EN(20, 180, "123456", &Font20, WHITE, BLACK); // 白字
//     // Paint_DrawString_EN(20, 180, "123456", &Font20, BLACK, WHITE);
//     EPD_4IN2_PartialDisplay(20, 180, 20 + Font20.Width * 6, 180 + Font20.Height, BlackImage);
//     DEV_Delay_ms(500);

//     // 局部刷新，应该确保起始位置和结束位置是8的整数倍
//     Paint_ClearWindows(16, 150, 16 + PINGFANG12.Width * 13, 150 + PINGFANG12.Height, WHITE);
//     Paint_DrawString_CN_From_File(16, 150, "这是一段测试文字十二号字体", &PINGFANG12, BLACK, WHITE);
//     EPD_4IN2_PartialDisplay(16, 150, 16 + PINGFANG12.Width * 13, 150 + PINGFANG12.Height, BlackImage);
//     DEV_Delay_ms(500);

// #if 1

//     EPD_4IN2_Init_Partial();
//     PAINT_TIME sPaint_time;
//     sPaint_time.Hour = 12;
//     sPaint_time.Min = 34;
//     sPaint_time.Sec = 56;
//     UBYTE num = 20;
//     for (;;)
//     {
//         sPaint_time.Sec = sPaint_time.Sec + 1;
//         if (sPaint_time.Sec == 60)
//         {
//             sPaint_time.Min = sPaint_time.Min + 1;
//             sPaint_time.Sec = 0;
//             if (sPaint_time.Min == 60)
//             {
//                 sPaint_time.Hour = sPaint_time.Hour + 1;
//                 sPaint_time.Min = 0;
//                 if (sPaint_time.Hour == 24)
//                 {
//                     sPaint_time.Hour = 0;
//                     sPaint_time.Min = 0;
//                     sPaint_time.Sec = 0;
//                 }
//             }
//         }
//         Paint_ClearWindows(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, WHITE);
//         Paint_DrawTime(150, 80, &sPaint_time, &Font20, BLACK, WHITE);

//         EPD_4IN2_PartialDisplay(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, BlackImage);
//         DEV_Delay_ms(500);
//         num = num - 1;
//         if (num == 0)
//         {
//             break;
//         }
//     }
// #endif

//     Serial.println("init and clear");
//     delay(2000);
//     EPD_4IN2_Init_Fast();
//     EPD_4IN2_Clear();

//     printf("Goto Sleep...\r\n");
//     EPD_4IN2_Sleep();
//     free(BlackImage);
//     BlackImage = NULL;
// }

// /* The main loop -------------------------------------------------------------*/
// void loop()
// {
//     //
// }
