
// #include "edf/DEV_Config.h"
// #include "edf/EPD.h"
// #include "paint/GUI_Paint.h"
// #include <stdlib.h>
// #include "func/connectWIFI.h"
// #include "func/LBS.h"
// #include "func/weather.h"
// #include <ArduinoJson.h>

// bool WIFILOCATION = false;
// bool IPLOCATION = false;
// String lat = "";
// String lon = "";
// String city = "";
// String address = "";
// DynamicJsonDocument doc(4096);


// void setup()
// {

//     DEV_Module_Init(); // 文件系统、串口、SPI、GPIO初始化
//     EPD_4IN2_Init_Fast();
//     EPD_4IN2_Clear(); // 清屏
//     DEV_Delay_ms(500);
//     Serial.println(">>> Init_Fast & Clear Success ");

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


//     // 网络连接服务
//     // wifiConfig(BlackImage);
//     IPAddress dns(8, 8, 8, 8);
//     WiFi.config(IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0), dns);
//     WiFi.begin("tmp", "12345678");
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         delay(500);
//         Serial.print(".");
//     }
//     delay(2000);

//     // LBS
    
    
//     WIFILOCATION = getLocationByWIFI(doc);
//     if (WIFILOCATION)
//     {
//         Serial.print(0x00);
//         Serial.print(0x00);
//         Serial.print(0x00);
//         Serial.print(0x00);
//         Serial.print(0x00);
//         lat = doc["lat"].as<String>();
//         lon = doc["lon"].as<String>();
//         address = doc["address"].as<String>();
//     }
    
    
//     Paint_DrawLine(0, 80, 400, 80, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

//     Serial.println();
//     Serial.println(city.length());
//     Serial.println(400 - city.length() / 3 * 16 - 10 - 22);
//     Serial.println();
//     if (!city.isEmpty())
//     {
//         Paint_DrawIcon_From_File("/icon_20.b", 1, 400 - city.length() / 3 * 16 - 10 - 20, 3, 20, 20, WHITE, BLACK);
//         Paint_DrawString_CN_From_File(400 - city.length() / 3 * 16 - 10, 2, city.c_str(), &PINGFANG12, BLACK, WHITE);
//     }
//     // WiFi.localIP().toString()
//     Paint_DrawIcon_From_File("/icon_20.b", 0, 8, 3, 20, 20, WHITE, BLACK);
//     Paint_DrawString_EN(32, 12, WiFi.localIP().toString().c_str(), &Font12, BLACK, WHITE);

//     EPD_4IN2_Display(BlackImage);
//     DEV_Delay_ms(2000);

//     // 获取实时天气

//     // 获取逐小时天气

//     // 获取未来七天天气

//     // #if 1 // show image for array
//     //     printf("show image for array\r\n");
//     //     Paint_SelectImage(BlackImage);
//     //     Paint_Clear(WHITE);
//     // Paint_DrawBitMap(gImage_4in2);
//     //     Serial.println(">>> Paint_Clear and init with Image_4in2");
//     //     EPD_4IN2_Display(BlackImage);
//     //     Serial.println(">>> Display Image_4in2");
//     //     DEV_Delay_ms(500);
//     // #endif
// #if 0
//     // 验证所有图标的
//     int xStart;
//     int yStart;
//     UWORD w_byte = 4;
//     UDOUBLE Addr = 0;
//     UDOUBLE pAddr = 0;
//     int index;
//     UWORD x, y;
//     uint8_t *dataspace = (uint8_t *)malloc(32 * 32);
//     File dataFile = LittleFS.open("/icon.b", "r");
//     Serial.println(dataFile.available());
//     for (int i = 0; i < 2; i++)
//     {
//         for (int j = 0; j < 9; j++)
//         {
//             for (int k = 0; k < 12; k++)
//             {
//                 index = i * 108 + j * 12 + k;
//                 xStart = k * 32;
//                 yStart = j * 32;
//                 if (index < 201)
//                 {
//                     Serial.print(index);
//                     if (!dataFile)
//                     {
//                         Serial.println("Failed to open file");
//                         return;
//                     }
//                     if (dataFile.available() < 32 * 4)
//                     {
//                         Serial.println("Not enough data available");
//                         Serial.println(dataFile.available());
//                         dataFile.close();
//                         return;
//                     }

//                     if (dataFile.seek(index * 32 * 4, SeekSet) != true)
//                     {
//                         Serial.println("Failed to seek to position");
//                         dataFile.close();
//                         return;
//                     }
//                     if (dataFile.read(dataspace, 32 * 4) != 32 * 4)
//                     {
//                         Serial.println("Failed to read data");
//                         dataFile.close();
//                         return;
//                     }
//                     Serial.println(index);
//                     x = 0;
//                     y = 0;
//                     w_byte = 4;
//                     Addr = 0;
//                     pAddr = 0;
//                     for (y = 0; y < 32; y++)
//                     {
//                         for (x = 0; x < w_byte; x++)
//                         {
//                             Addr = x + y * w_byte;
//                             pAddr = x + (xStart / 8) + ((y + yStart) * Paint.WidthByte);
//                             Paint.Image[pAddr] = (unsigned char)dataspace[Addr];
//                         }
//                     }
//                     DEV_Delay_ms(100);
//                 }
//             }
//             EPD_4IN2_Display(BlackImage);
//             DEV_Delay_ms(1000);
//         }
//     }
//     dataFile.close();

// #endif

// #if 0 // Drawing on the image
//     Serial.println(">>> Drawing on the image");
//     Paint_SelectImage(BlackImage);
//     Paint_Clear(WHITE);

//     // Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
//     // Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
//     // Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
//     // Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//     // Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

//     // EPD_4IN2_Display(BlackImage);
//     // DEV_Delay_ms(2000);

//     // Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
//     // Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
//     // Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
//     // Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
//     // Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
//     // Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

//     // Serial.println(">>> EPD_Display\r\n");
//     // EPD_4IN2_Display(BlackImage);
//     // DEV_Delay_ms(2000);

//     // Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
//     // Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);
//     Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
//     // Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);
//     // Paint_DrawString_CN(130, 0, " 你好abc", &Font12CN, BLACK, WHITE);
//     // Paint_DrawString_CN(130, 20, "微雪电子", &Font24CN, WHITE, BLACK);

//     // Paint_DrawString_CN_From_File(130, 0, " 你好abc", &FXLED36, BLACK, WHITE);
//     // Paint_DrawString_CN_From_File(20, 168, "这是一段测试文字十二号字体二十二十", &PINGFANG12, BLACK, WHITE);
//     // Paint_DrawString_CN_From_File(20, 240, "这是一段测试文字十四二十二十", &PINGFANG14, BLACK, WHITE);
//     // Paint_DrawString_CN_From_File(130, 20, "微雪电子", &CN21, WHITE, BLACK);

//     Paint_DrawImage(w_100, 150, 100, 56, 56);

//     Serial.println(">>> EPD_Display\r\n");
//     EPD_4IN2_Display(BlackImage);
//     DEV_Delay_ms(2000);
// #endif

// #if 0

//         printf("Support for partial refresh, but the refresh effect is not good, but it is not recommended\r\n");
//     printf("Partial refresh\r\n");
//     EPD_4IN2_Init_Partial();

//     // Paint_DrawNum(110, 133, 123456789, &Font12, WHITE, BLACK);
//     // EPD_4IN2_PartialDisplay(110, 183, 110 + Font12.Width * 9, 183 + Font12.Height, BlackImage);

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
//         Paint_DrawTime(150, 80, &sPaint_time, &Font20, WHITE, BLACK);

//         EPD_4IN2_PartialDisplay(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, BlackImage);
//         DEV_Delay_ms(500); // Analog clock 1s
//         num = num - 1;
//         if (num == 0)
//         {
//             break;
//         }
//     }
// #endif

//     Serial.println("init and clear");
//     delay(2000);
//     // EPD_4IN2_Init_Fast();
//     // EPD_4IN2_Clear();

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
