
#include "edf/DEV_Config.h"
#include "edf/EPD.h"
#include "paint/GUI_Paint.h"
#include <stdlib.h>
#include "func/connectWIFI.h"
#include "func/LBS.h"
#include "func/weather.h"
#include <ArduinoJson.h>

bool WIFILOCATION = false;
bool IPLOCATION = false;
String lat = "";
String lon = "";
String city = "";
String address = "";
DynamicJsonDocument doc(4096);
int ICON_INDEX[] = {100, 101, 102, 103, 104, 150, 151, 152, 153, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 350, 351, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 456, 457, 499, 500, 501, 502, 503, 504, 507, 508, 509, 510, 511, 512, 513, 514, 515, 800, 801, 802, 803, 804, 805, 806, 807, 900, 901, 999, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010, 1011, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019, 1020, 1021, 1022, 1023, 1024, 1025, 1026, 1027, 1028, 1029, 1030, 1031, 1032, 1033, 1034, 1035, 1036, 1037, 1038, 1039, 1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048, 1049, 1050, 1051, 1052, 1053, 1054, 1055, 1056, 1057, 1058, 1059, 1060, 1061, 1062, 1063, 1064, 1065, 1066, 1067, 1068, 1069, 1071, 1072, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1084, 1085, 1086, 1087, 1088, 1089, 1101, 1302, 1402, 1601, 1602, 1603, 1604, 1605, 1606, 1607, 1701, 1702, 1703, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019, 2020, 2021, 2022, 2023, 2024, 2025, 2026, 2027, 2028, 2052, 2053, 2054};
/**
 * @description: 用于将unicode编码转为UTF8编码
 * @param {String} str : unicode编码
 */
String urlDecode(String str)
{
    String decoded = "";
    char c;
    int len = str.length();
    for (int i = 0; i < len; i++)
    {
        if (str[i] == '+')
        {
            decoded += ' ';
        }
        else if (str[i] == '%')
        {
            if (isxdigit(str[i + 1]) && isxdigit(str[i + 2]))
            {
                c = strtoul(str.substring(i + 1, i + 3).c_str(), NULL, 16);
                decoded += c;
                i += 2;
            }
            else
            {
                decoded += '%';
            }
        }
        else
        {
            decoded += str[i];
        }
    }
    return decoded;
}

/**
 * @description: 根据图标编号值，获取图标在文件中的偏移.不存在就返回-1
 * @param {int} item :
 * @return {Any}
 */
int find_icon_index(int item)
{
    for (int i = 0; i < 201; i++)
    {
        if (ICON_INDEX[i] == item)
        {
            return i;
        }
    }
    return -1;
}

void updateRealtimeWeather()
{
}

void setup()
{
    DEV_Module_Init(); // 文件系统、串口、SPI、GPIO初始化
    EPD_4IN2_Init_Fast();
    EPD_4IN2_Clear(); // 清屏
    DEV_Delay_ms(500);
    Serial.println(">>> Init_Fast & Clear Success ");

    // 创建图像缓冲区
    UBYTE *BlackImage;
    UWORD imageSize = ((EPD_4IN2_WIDTH % 8 == 0) ? (EPD_4IN2_WIDTH / 8) : (EPD_4IN2_WIDTH / 8 + 1)) * EPD_4IN2_HEIGHT;
    if ((BlackImage = (UBYTE *)malloc(imageSize)) == NULL)
    {
        while (1)
        {
            Serial.println("Failed to apply for black memory...\r\n");
            delay(5000);
        }
    }
    Paint_NewImage(BlackImage, EPD_4IN2_WIDTH, EPD_4IN2_HEIGHT, 0, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    // 网络连接服务
    // wifiConfig(BlackImage);
    IPAddress dns(8, 8, 8, 8);
    WiFi.config(IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0), dns);
    WiFi.begin("tmp", "12345678");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    delay(500);

    // LBS

    IPLOCATION = getLocationByIP(doc);
    if (IPLOCATION)
    {
        lon = doc["content"]["point"]["x"].as<String>();
        lat = doc["content"]["point"]["y"].as<String>();
        city = doc["content"]["address_detail"]["city"].as<String>();
        city.replace("\\u", "%u");
        city = urlDecode(city);
    }
    delay(500);

    WIFILOCATION = getLocationByWIFI(doc);
    if (WIFILOCATION)
    {
        lat = doc["lat"].as<String>();
        lon = doc["lon"].as<String>();
        address = doc["address"].as<String>();
    }

    Serial.println("---------------Location Info--------------");
    Serial.println(lon);
    Serial.println(lat);
    Serial.println(city);
    Serial.println(address);
    Serial.println("------------------------------------------");

    // 绘制框架
    EPD_4IN2_Init_Fast();
    Paint_Clear(WHITE);
    DEV_Delay_ms(500);
    Paint_DrawLine(0, 25, 400, 25, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(0, 80, 400, 80, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Serial.println();
    Serial.println(city.length());
    Serial.println(400 - city.length() / 3 * 16 - 10 - 22);
    Serial.println();
    if (!city.isEmpty())
    {
        Paint_DrawIcon_From_File("/icon_20.b", 1, 400 - city.length() / 3 * 16 - 10 - 20, 3, 20, 20, WHITE, BLACK);
        Paint_DrawString_CN_From_File(400 - city.length() / 3 * 16 - 10, 2, city.c_str(), &PINGFANG12, BLACK, WHITE);
    }
    // WiFi.localIP().toString()
    Paint_DrawIcon_From_File("/icon_20.b", 0, 8, 3, 20, 20, WHITE, BLACK);
    Paint_DrawString_EN(32, 12, WiFi.localIP().toString().c_str(), &Font12, BLACK, WHITE);

    EPD_4IN2_Display(BlackImage);
    DEV_Delay_ms(500);

    // 获取实时天气
    delay(500);
    getWeatherInfo(doc, "/weather/now", lon, lat, "101190405", "SkqZeGILxdGHyoflbhcgddcWykFgbZlh");

    String temp = doc["temp"].as<String>();
    String city = doc["city"].as<String>();
    String wind_d = doc["wind_d"].as<String>();
    String wind_s = doc["wind_s"].as<String>();
    String humidity = doc["humidity"].as<String>();
    String air_press = doc["air_press"].as<String>();
    String weather = doc["weather"].as<String>();
    String source = doc["source"].as<String>();
    int icon = doc["icon"].as<int>();

    Serial.println(temp);
    Serial.println(city);
    Serial.println(wind_d);
    Serial.println(wind_s);
    Serial.println(humidity);
    Serial.println(air_press);
    Serial.println(weather);
    Serial.println(icon);
    Serial.println(source);

    // 宽度 ：54 start = 26
    // 126  天气图标 @ 天气描述 @ 实时温度

    // 天气图标 x: 6  y:26+9 = 34

    Paint_DrawIcon_From_File("icon.b", find_icon_index(icon), 8, 35, 32, 32, WHITE, BLACK); // 数据集中0为黑色 ; 而板子0定义为白色
    Paint_DrawString_CN_From_File(50, 29, weather.c_str(), &PINGFANG14, BLACK, WHITE);
    Paint_DrawString_EN(50, 54, String(temp).c_str(), &Font20, BLACK, WHITE);
    EPD_4IN2_Display(BlackImage);
    DEV_Delay_ms(2000);

    // 148 时间

    // 126 体感温度 相对湿度 风速

    // 获取逐小时天气

    // 获取未来七天天气

    // #if 1 // show image for array
    //     printf("show image for array\r\n");
    //     Paint_SelectImage(BlackImage);
    //     Paint_Clear(WHITE);
    // Paint_DrawBitMap(gImage_4in2);
    //     Serial.println(">>> Paint_Clear and init with Image_4in2");
    //     EPD_4IN2_Display(BlackImage);
    //     Serial.println(">>> Display Image_4in2");
    //     DEV_Delay_ms(500);
    // #endif
#if 0
    // 验证所有图标的
    int xStart;
    int yStart;
    UWORD w_byte = 4;
    UDOUBLE Addr = 0;
    UDOUBLE pAddr = 0;
    int index;
    UWORD x, y;
    uint8_t *dataspace = (uint8_t *)malloc(32 * 32);
    File dataFile = LittleFS.open("/icon.b", "r");
    Serial.println(dataFile.available());
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            for (int k = 0; k < 12; k++)
            {
                index = i * 108 + j * 12 + k;
                xStart = k * 32;
                yStart = j * 32;
                if (index < 201)
                {
                    Serial.print(index);
                    if (!dataFile)
                    {
                        Serial.println("Failed to open file");
                        return;
                    }
                    if (dataFile.available() < 32 * 4)
                    {
                        Serial.println("Not enough data available");
                        Serial.println(dataFile.available());
                        dataFile.close();
                        return;
                    }

                    if (dataFile.seek(index * 32 * 4, SeekSet) != true)
                    {
                        Serial.println("Failed to seek to position");
                        dataFile.close();
                        return;
                    }
                    if (dataFile.read(dataspace, 32 * 4) != 32 * 4)
                    {
                        Serial.println("Failed to read data");
                        dataFile.close();
                        return;
                    }
                    Serial.println(index);
                    x = 0;
                    y = 0;
                    w_byte = 4;
                    Addr = 0;
                    pAddr = 0;
                    for (y = 0; y < 32; y++)
                    {
                        for (x = 0; x < w_byte; x++)
                        {
                            Addr = x + y * w_byte;
                            pAddr = x + (xStart / 8) + ((y + yStart) * Paint.WidthByte);
                            Paint.Image[pAddr] = (unsigned char)dataspace[Addr];
                        }
                    }
                    DEV_Delay_ms(100);
                }
            }
            EPD_4IN2_Display(BlackImage);
            DEV_Delay_ms(1000);
        }
    }
    dataFile.close();

#endif

#if 0 // Drawing on the image
    Serial.println(">>> Drawing on the image");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    // Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    // Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    // Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    // Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    // Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

    // EPD_4IN2_Display(BlackImage);
    // DEV_Delay_ms(2000);

    // Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    // Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    // Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    // Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    // Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    // Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

    // Serial.println(">>> EPD_Display\r\n");
    // EPD_4IN2_Display(BlackImage);
    // DEV_Delay_ms(2000);

    // Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
    // Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);
    Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
    // Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);
    // Paint_DrawString_CN(130, 0, " 你好abc", &Font12CN, BLACK, WHITE);
    // Paint_DrawString_CN(130, 20, "微雪电子", &Font24CN, WHITE, BLACK);

    // Paint_DrawString_CN_From_File(130, 0, " 你好abc", &FXLED36, BLACK, WHITE);
    // Paint_DrawString_CN_From_File(20, 168, "这是一段测试文字十二号字体二十二十", &PINGFANG12, BLACK, WHITE);
    // Paint_DrawString_CN_From_File(20, 240, "这是一段测试文字十四二十二十", &PINGFANG14, BLACK, WHITE);
    // Paint_DrawString_CN_From_File(130, 20, "微雪电子", &CN21, WHITE, BLACK);

    Paint_DrawImage(w_100, 150, 100, 56, 56);

    Serial.println(">>> EPD_Display\r\n");
    EPD_4IN2_Display(BlackImage);
    DEV_Delay_ms(2000);
#endif

#if 1

    printf("Support for partial refresh, but the refresh effect is not good, but it is not recommended\r\n");
    printf("Partial refresh\r\n");
    EPD_4IN2_Init_Partial();

    // Paint_DrawNum(110, 133, 123456789, &Font12, WHITE, BLACK);
    // EPD_4IN2_PartialDisplay(110, 183, 110 + Font12.Width * 9, 183 + Font12.Height, BlackImage);

    PAINT_TIME sPaint_time;
    sPaint_time.Hour = 12;
    sPaint_time.Min = 34;
    sPaint_time.Sec = 56;
    UBYTE num = 20;
    for (;;)
    {
        sPaint_time.Sec = sPaint_time.Sec + 1;
        if (sPaint_time.Sec == 60)
        {
            sPaint_time.Min = sPaint_time.Min + 1;
            sPaint_time.Sec = 0;
            if (sPaint_time.Min == 60)
            {
                sPaint_time.Hour = sPaint_time.Hour + 1;
                sPaint_time.Min = 0;
                if (sPaint_time.Hour == 24)
                {
                    sPaint_time.Hour = 0;
                    sPaint_time.Min = 0;
                    sPaint_time.Sec = 0;
                }
            }
        }
        Paint_ClearWindows(120, 50, 120 + FXLED36.Width * 7, 50 + FXLED36.Height, WHITE);
        Paint_DrawTime_From_File(120,50,&sPaint_time,&FXLED36, WHITE, BLACK);
        EPD_4IN2_PartialDisplay(120, 50, 120 + FXLED36.Width * 7, 50 + FXLED36.Height, BlackImage);


        // Paint_ClearWindows(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, WHITE);
        // Paint_DrawTime(150, 80, &sPaint_time, &Font20, WHITE, BLACK);
        // EPD_4IN2_PartialDisplay(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, BlackImage);
        DEV_Delay_ms(500); // Analog clock 1s
        num = num - 1;
        if (num == 0)
        {
            break;
        }
    }
#endif

    Serial.println("init and clear");
    delay(2000);
    // EPD_4IN2_Init_Fast();
    // EPD_4IN2_Clear();

    printf("Goto Sleep...\r\n");
    EPD_4IN2_Sleep();
    free(BlackImage);
    BlackImage = NULL;
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
    //
}
