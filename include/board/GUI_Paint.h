#ifndef __GUI_PAINT_H
#define __GUI_PAINT_H

#include "board/Debug.h"
#include "board/DEV_Config.h"
#include "fonts/fonts.h"
#include "resource/char.h"

/**
 * Image attributes
**/
typedef struct {
    UBYTE *Image;
    UWORD Width;            // 屏幕(可能旋转了)宽的像素数
    UWORD Height;           // 屏幕(可能旋转了)高的像素数
    UWORD WidthMemory;      // 存储区(图像)宽的像素数
    UWORD HeightMemory;     // 存储区(图像)高的像素数
    UWORD Color;
    UWORD Rotate;
    UWORD Mirror;
    UWORD WidthByte;
    UWORD HeightByte;
    UWORD Scale;
} PAINT;
extern PAINT Paint;

/**
 * Display rotate
**/
#define ROTATE_0            0
#define ROTATE_90           90
#define ROTATE_180          180
#define ROTATE_270          270

/**
 * Display Flip
**/
typedef enum {
    MIRROR_NONE  = 0x00,
    MIRROR_HORIZONTAL = 0x01,
    MIRROR_VERTICAL = 0x02,
    MIRROR_ORIGIN = 0x03,
} MIRROR_IMAGE;
#define MIRROR_IMAGE_DFT MIRROR_NONE

/**
 * image color
**/
#define WHITE          0xFF
#define BLACK          0x00
#define RED            BLACK

#define IMAGE_BACKGROUND    WHITE
#define FONT_FOREGROUND     BLACK
#define FONT_BACKGROUND     WHITE

//4 Gray level
#define  GRAY1 0x03 //Blackest
#define  GRAY2 0x02
#define  GRAY3 0x01 //gray
#define  GRAY4 0x00 //white
/**
 * The size of the point
**/
typedef enum {
    DOT_PIXEL_1X1  = 1,		// 1 x 1
    DOT_PIXEL_2X2  , 		// 2 X 2
    DOT_PIXEL_3X3  ,		// 3 X 3
    DOT_PIXEL_4X4  ,		// 4 X 4
    DOT_PIXEL_5X5  , 		// 5 X 5
    DOT_PIXEL_6X6  , 		// 6 X 6
    DOT_PIXEL_7X7  , 		// 7 X 7
    DOT_PIXEL_8X8  , 		// 8 X 8
} DOT_PIXEL;
#define DOT_PIXEL_DFT  DOT_PIXEL_1X1  //Default dot pilex

/**
 * Point size fill style
**/
typedef enum {
    DOT_FILL_AROUND  = 1,		// dot pixel 1 x 1
    DOT_FILL_RIGHTUP  , 		// dot pixel 2 X 2
} DOT_STYLE;
#define DOT_STYLE_DFT  DOT_FILL_AROUND  //Default dot pilex

/**
 * Line style, solid or dashed
**/
typedef enum {
    LINE_STYLE_SOLID = 0,
    LINE_STYLE_DOTTED,
} LINE_STYLE;

/**
 * Whether the graphic is filled
**/
typedef enum {
    DRAW_FILL_EMPTY = 0,
    DRAW_FILL_FULL,
} DRAW_FILL;

/**
 * Custom structure of a time attribute
**/
typedef struct {
    UWORD Year;  //0000
    UBYTE  Month; //1 - 12
    UBYTE  Day;   //1 - 30
    UBYTE  Hour;  //0 - 23
    UBYTE  Min;   //0 - 59
    UBYTE  Sec;   //0 - 59
    UBYTE  Week;   //0 - 6
} PAINT_TIME;

extern PAINT_TIME sPaint_time;

//init and Clear
void Paint_NewImage(UBYTE *image, UWORD Width, UWORD Height, UWORD Rotate, UWORD Color);
void Paint_SelectImage(UBYTE *image);
void Paint_SetRotate(UWORD Rotate);
void Paint_SetMirroring(UBYTE mirror);
void Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color);
void Paint_SetScale(UBYTE scale);

void Paint_Clear(UWORD Color);
void Paint_ClearWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color);

//Drawing
void Paint_DrawPoint(UWORD Xpoint, UWORD Ypoint, UWORD Color, DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_FillWay);
void Paint_DrawLine(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color, DOT_PIXEL Line_width, LINE_STYLE Line_Style);
void Paint_DrawRectangle(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill);
void Paint_DrawCircle(UWORD X_Center, UWORD Y_Center, UWORD Radius, UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill);

//Display string
void Paint_DrawChar(UWORD Xstart, UWORD Ystart, const char Acsii_Char, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background, int index);
void Paint_DrawString_EN(UWORD Xstart, UWORD Ystart, const char * pString, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background);
void Paint_DrawNum(UWORD Xpoint, UWORD Ypoint, int32_t Nummber, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background);
void Paint_DrawTime(UWORD Xstart, UWORD Ystart, String time, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background);

//pic
void Paint_DrawBitMap(const unsigned char* image_buffer);
void Paint_DrawImage(const unsigned char *image_buffer, UWORD xStart, UWORD yStart, UWORD W_Image, UWORD H_Image);


//from file
void Paint_DrawChar_From_File(UWORD Xpoint, UWORD Ypoint, const char Acsii_Char, file_char_acsll *font, UWORD Color_Foreground, UWORD Color_Background);
void Paint_DrawString_EN_From_File(UWORD Xstart, UWORD Ystart, const char *pString, file_char_acsll *font, UWORD Color_Foreground, UWORD Color_Background);
// void Paint_DrawString_CN_From_File(UWORD Xstart, UWORD Ystart, const char *pString, file_char *font,UWORD Color_Foreground, UWORD Color_Background);
void Paint_DrawString_CN_From_File(UWORD Xstart, UWORD Ystart, String str, file_char *font,UWORD Color_Foreground, UWORD Color_Background);
UWORD Paint_DrawString_CN_From_File_V2(UWORD Xstart, UWORD Ystart, String str, file_char *font, UWORD Color_Foreground, UWORD Color_Background);
UWORD Paint_DrawString_CN_From_File_V3(UWORD Xstart, UWORD Ystart, String str, file_char *cn_font, sFONT *en_font, UWORD Color_Foreground, UWORD Color_Background);
UWORD Paint_DrawString_CN_From_File_V4(UWORD Xstart, UWORD Xend, UWORD Ystart,UWORD Yend, String str, file_char *cn_font, sFONT *en_font, UWORD Color_Foreground, UWORD Color_Background);
void Paint_DrawNum_From_File(UWORD Xpoint, UWORD Ypoint, int32_t Nummber, file_char_acsll *font, UWORD Color_Foreground, UWORD Color_Background);
void verify_icon(const char *filename, uint8_t *dataspace, int index, UWORD xStart, UWORD yStart, UWORD W_Image, UWORD H_Image);
// void Paint_DrawIcon_From_File(const char *filename, int index, UWORD xStart, UWORD yStart, UWORD W_Image, UWORD H_Image, UWORD Color_Foreground,UWORD Color_Background);
void Paint_DrawTime_From_File(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, file_char_acsll *font, UWORD Color_Foreground, UWORD Color_Background);
void Paint_DrawIcon_From_File(int icon_code, UWORD xStart, UWORD yStart, UWORD W_Image, UWORD H_Image, UWORD Color_Foreground,UWORD Color_Background);
// Verify
void Paint_DrawAllCN(const char *file_name, file_char *font, UWORD Color_Foreground, UWORD Color_Background);


void Paint_DrawWeatherInfo(UWORD Xstart, UWORD Ystart, String temp, String humidity,sFONT *Font, UWORD Color_Foreground, UWORD Color_Background);
#endif





