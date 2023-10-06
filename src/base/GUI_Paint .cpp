#include "board/GUI_Paint.h"
#include "board/DEV_Config.h"
#include "board/EPD_4in2.h"
#include "board/Debug.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h> //memset()
#include <math.h>

#include "resource/char.h"
#include "resource/icon.h"

PAINT Paint;

/******************************************************************************
function: Create Image
parameter:
    image   :   Pointer to the image cache          存储区地址
    width   :   The width of the picture            图片的宽
    Height  :   The height of the picture           图片的高
    Color   :   Whether the picture is inverted     是否反色
******************************************************************************/
void Paint_NewImage(UBYTE *image, UWORD Width, UWORD Height, UWORD Rotate, UWORD Color)
{
    Paint.Image = NULL;
    Paint.Image = image;

    Paint.WidthMemory = Width;
    Paint.HeightMemory = Height;
    Paint.Color = Color;
    Paint.Scale = 2; // 颜色尺度，黑白 = 2 ; 红白 = 2
    Paint.WidthByte = (Width % 8 == 0) ? (Width / 8) : (Width / 8 + 1);
    Paint.HeightByte = Height;

    Paint.Rotate = Rotate;      // 旋转
    Paint.Mirror = MIRROR_NONE; // 翻转
    if (Rotate == ROTATE_0 || Rotate == ROTATE_180)
    {
        Paint.Width = Width;
        Paint.Height = Height;
    }
    else
    {
        Paint.Width = Height;
        Paint.Height = Width;
    }
}

/******************************************************************************
function: Select Image
parameter:
    image : Pointer to the image cache
******************************************************************************/
void Paint_SelectImage(UBYTE *image)
{
    Paint.Image = image;
}

/******************************************************************************
function: Select Image Rotate
parameter:
    Rotate : 0,90,180,270
******************************************************************************/
void Paint_SetRotate(UWORD Rotate) // 设置旋转
{
    if (Rotate == ROTATE_0 || Rotate == ROTATE_90 || Rotate == ROTATE_180 || Rotate == ROTATE_270)
    {
        // Debug("Set image Rotate %d\r\n", Rotate);
        Paint.Rotate = Rotate;
    }
    else
    {
        Debug("rotate = 0, 90, 180, 270\r\n");
    }
}

/******************************************************************************
function:	Select Image mirror
parameter:
    mirror   :Not mirror,Horizontal mirror,Vertical mirror,Origin mirror
******************************************************************************/
void Paint_SetMirroring(UBYTE mirror) // 设置翻转
{
    if (mirror == MIRROR_NONE || mirror == MIRROR_HORIZONTAL ||
        mirror == MIRROR_VERTICAL || mirror == MIRROR_ORIGIN)
    {
        // Debug("mirror image x:%s, y:%s\r\n",(mirror & 0x01)? "mirror":"none", ((mirror >> 1) & 0x01)? "mirror":"none");
        Paint.Mirror = mirror;
    }
    else
    {
        Debug("mirror should be MIRROR_NONE, MIRROR_HORIZONTAL, \
        MIRROR_VERTICAL or MIRROR_ORIGIN\r\n");
    }
}

void Paint_SetScale(UBYTE scale) // 设置颜色尺度
{
    if (scale == 2)
    {
        // 一个字节可以存储8个像素点信息
        Paint.Scale = scale;
        Paint.WidthByte = (Paint.WidthMemory % 8 == 0) ? (Paint.WidthMemory / 8) : (Paint.WidthMemory / 8 + 1);
    }
    else if (scale == 4)
    {
        // 一个自己只能存储4个具有四个灰度等级的像素信息
        Paint.Scale = scale;
        Paint.WidthByte = (Paint.WidthMemory % 4 == 0) ? (Paint.WidthMemory / 4) : (Paint.WidthMemory / 4 + 1);
    }
    else if (scale == 7)
    { // Only applicable with 5in65 e-Paper
        Paint.Scale = 7;
        Paint.WidthByte = (Paint.WidthMemory % 2 == 0) ? (Paint.WidthMemory / 2) : (Paint.WidthMemory / 2 + 1);
    }
    else
    {
        Debug("Set Scale Input parameter error\r\n");
        Debug("Scale Only support: 2 4 7\r\n");
    }
}

/******************************************************************************
function: Draw Pixels
parameter:
    Xpoint : At point X
    Ypoint : At point Y
    Color  : Painted colors
******************************************************************************/
void Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color) // 设置像素点的颜色
{
    // 像素是否超过宽高
    if (Xpoint > Paint.Width || Ypoint > Paint.Height)
    {
        // Debug("Exceeding display boundaries\r\n");
        Debug("Exceeding display boundaries:");
        Debug(Xpoint);
        Debug(Ypoint);
        Debug("\r\n");
        return;
    }
    UWORD X, Y; // 转化后的像素位置
    // 旋转变换
    switch (Paint.Rotate)
    {
    case 0:
        X = Xpoint;
        Y = Ypoint;
        break;
    case 90:
        X = Paint.WidthMemory - Ypoint - 1;
        Y = Xpoint;
        break;
    case 180:
        X = Paint.WidthMemory - Xpoint - 1;
        Y = Paint.HeightMemory - Ypoint - 1;
        break;
    case 270:
        X = Ypoint;
        Y = Paint.HeightMemory - Xpoint - 1;
        break;
    default:
        return;
    }
    // 镜像变换
    switch (Paint.Mirror)
    {
    case MIRROR_NONE:
        break;
    case MIRROR_HORIZONTAL:
        X = Paint.WidthMemory - X - 1;
        break;
    case MIRROR_VERTICAL:
        Y = Paint.HeightMemory - Y - 1;
        break;
    case MIRROR_ORIGIN:
        X = Paint.WidthMemory - X - 1;
        Y = Paint.HeightMemory - Y - 1;
        break;
    default:
        return;
    }

    if (X > Paint.WidthMemory || Y > Paint.HeightMemory)
    {
        Debug("Exceeding display boundaries:");
        Debug(X);
        Debug(Y);
        Debug("\r\n");

        return;
    }

    if (Paint.Scale == 2)
    {
        UDOUBLE Addr = X / 8 + Y * Paint.WidthByte;
        UBYTE Rdata = Paint.Image[Addr];
        if (Color == BLACK)
            Paint.Image[Addr] = Rdata & ~(0x80 >> (X % 8));
        else
            Paint.Image[Addr] = Rdata | (0x80 >> (X % 8));
    }
    else if (Paint.Scale == 4)
    {
        UDOUBLE Addr = X / 4 + Y * Paint.WidthByte;
        Color = Color % 4; // Guaranteed color scale is 4  --- 0~3
        UBYTE Rdata = Paint.Image[Addr];

        Rdata = Rdata & (~(0xC0 >> ((X % 4) * 2)));
        Paint.Image[Addr] = Rdata | ((Color << 6) >> ((X % 4) * 2));
    }
    else if (Paint.Scale == 7)
    {
        UWORD Width = Paint.WidthMemory * 3 % 8 == 0 ? Paint.WidthMemory * 3 / 8 : Paint.WidthMemory * 3 / 8 + 1;
        UDOUBLE Addr = (Xpoint * 3) / 8 + Ypoint * Width;
        UBYTE shift, Rdata, Rdata2;
        shift = (Xpoint + Ypoint * Paint.HeightMemory) % 8;

        switch (shift)
        {
        case 0:
            Rdata = Paint.Image[Addr] & 0x1f;
            Rdata = Rdata | ((Color << 5) & 0xe0);
            Paint.Image[Addr] = Rdata;
            break;
        case 1:
            Rdata = Paint.Image[Addr] & 0xe3;
            Rdata = Rdata | ((Color << 2) & 0x1c);
            Paint.Image[Addr] = Rdata;
            break;
        case 2:
            Rdata = Paint.Image[Addr] & 0xfc;
            Rdata2 = Paint.Image[Addr + 1] & 0x7f;
            Rdata = Rdata | ((Color >> 1) & 0x03);
            Rdata2 = Rdata2 | ((Color << 7) & 0x80);
            Paint.Image[Addr] = Rdata;
            Paint.Image[Addr + 1] = Rdata2;
            break;
        case 3:
            Rdata = Paint.Image[Addr] & 0x8f;
            Rdata = Rdata | ((Color << 4) & 0x70);
            Paint.Image[Addr] = Rdata;
            break;
        case 4:
            Rdata = Paint.Image[Addr] & 0xf1;
            Rdata = Rdata | ((Color << 1) & 0x0e);
            Paint.Image[Addr] = Rdata;
            break;
        case 5:
            Rdata = Paint.Image[Addr] & 0xfe;
            Rdata2 = Paint.Image[Addr + 1] & 0x3f;
            Rdata = Rdata | ((Color >> 2) & 0x01);
            Rdata2 = Rdata2 | ((Color << 6) & 0xc0);
            Paint.Image[Addr] = Rdata;
            Paint.Image[Addr + 1] = Rdata2;
            break;
        case 6:
            Rdata = Paint.Image[Addr] & 0xc7;
            Rdata = Rdata | ((Color << 3) & 0x38);
            Paint.Image[Addr] = Rdata;
            break;
        case 7:
            Rdata = Paint.Image[Addr] & 0xf8;
            Rdata = Rdata | (Color & 0x07);
            Paint.Image[Addr] = Rdata;
            break;
        }
    }
}

/******************************************************************************
function: Clear the color of the picture
parameter:
    Color : Painted colors
******************************************************************************/
void Paint_Clear(UWORD Color) // 清屏，设置所有像素的颜色
{
    if (Paint.Scale == 2 || Paint.Scale == 4)
    {
        for (UWORD Y = 0; Y < Paint.HeightByte; Y++)
        {
            for (UWORD X = 0; X < Paint.WidthByte; X++)
            { // 8 pixel =  1 byte
                UDOUBLE Addr = X + Y * Paint.WidthByte;
                Paint.Image[Addr] = Color;
            }
        }
    }
    else if (Paint.Scale == 7)
    {
        Color = (UBYTE)Color;
        UWORD Width = (Paint.WidthMemory * 3 % 8 == 0) ? (Paint.WidthMemory * 3 / 8) : (Paint.WidthMemory * 3 / 8 + 1);
        for (UWORD Y = 0; Y < Paint.HeightMemory; Y++)
        {
            for (UWORD X = 0; X < Width; X++)
            {
                UDOUBLE Addr = X + Y * Width;
                if (Addr % 3 == 0)
                    Paint.Image[Addr] = ((Color << 5) | (Color << 2) | (Color >> 1));
                else if (Addr % 3 == 1)
                    Paint.Image[Addr] = ((Color << 7) | (Color << 4) | (Color << 1) | (Color >> 2));
                else if (Addr % 3 == 2)
                    Paint.Image[Addr] = ((Color << 6) | (Color << 3) | Color);
            }
        }
    }
}

/******************************************************************************
function: Clear the color of a window
parameter:
    Xstart : x starting point
    Ystart : Y starting point
    Xend   : x end point
    Yend   : y end point
    Color  : Painted colors
******************************************************************************/
void Paint_ClearWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color)
{
    UWORD X, Y;
    for (Y = Ystart; Y < Yend; Y++)
    {
        for (X = Xstart; X < Xend; X++)
        { // 8 pixel =  1 byte
            Paint_SetPixel(X, Y, Color);
        }
    }
}

/******************************************************************************
function: Draw Point(Xpoint, Ypoint) Fill the color
parameter:
    Xpoint		: The Xpoint coordinate of the point
    Ypoint		: The Ypoint coordinate of the point
    Color		: Painted color
    Dot_Pixel	: point size
    Dot_Style	: point Style
******************************************************************************/
void Paint_DrawPoint(UWORD Xpoint, UWORD Ypoint, UWORD Color,
                     DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_Style)
{
    if (Xpoint > Paint.Width || Ypoint > Paint.Height)
    {
        Debug("Paint_DrawPoint Input exceeds the normal display range\r\n");
        return;
    }

    int16_t XDir_Num, YDir_Num;
    if (Dot_Style == DOT_FILL_AROUND)
    {
        for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++)
        {
            for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++)
            {
                if (Xpoint + XDir_Num - Dot_Pixel < 0 || Ypoint + YDir_Num - Dot_Pixel < 0)
                    break;
                // printf("x = %d, y = %d\r\n", Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel);
                Paint_SetPixel(Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel, Color);
            }
        }
    }
    else
    {
        for (XDir_Num = 0; XDir_Num < Dot_Pixel; XDir_Num++)
        {
            for (YDir_Num = 0; YDir_Num < Dot_Pixel; YDir_Num++)
            {
                Paint_SetPixel(Xpoint + XDir_Num - 1, Ypoint + YDir_Num - 1, Color);
            }
        }
    }
}

/******************************************************************************
function: Draw a line of arbitrary slope
parameter:
    Xstart ：Starting Xpoint point coordinates
    Ystart ：Starting Xpoint point coordinates
    Xend   ：End point Xpoint coordinate
    Yend   ：End point Ypoint coordinate
    Color  ：The color of the line segment
    Line_width : Line width
    Line_Style: Solid and dotted lines
******************************************************************************/
void Paint_DrawLine(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,
                    UWORD Color, DOT_PIXEL Line_width, LINE_STYLE Line_Style)
{
    if (Xstart > Paint.Width || Ystart > Paint.Height ||
        Xend > Paint.Width || Yend > Paint.Height)
    {
        Debug("Paint_DrawLine Input exceeds the normal display range\r\n");
        return;
    }

    UWORD Xpoint = Xstart;
    UWORD Ypoint = Ystart;
    int dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
    int dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

    // Increment direction, 1 is positive, -1 is counter;
    int XAddway = Xstart < Xend ? 1 : -1;
    int YAddway = Ystart < Yend ? 1 : -1;

    // Cumulative error
    int Esp = dx + dy;
    char Dotted_Len = 0;

    for (;;)
    {
        Dotted_Len++;
        // Painted dotted line, 2 point is really virtual
        if (Line_Style == LINE_STYLE_DOTTED && Dotted_Len % 3 == 0)
        {
            // Debug("LINE_DOTTED\r\n");
            Paint_DrawPoint(Xpoint, Ypoint, IMAGE_BACKGROUND, Line_width, DOT_STYLE_DFT);
            Dotted_Len = 0;
        }
        else
        {
            Paint_DrawPoint(Xpoint, Ypoint, Color, Line_width, DOT_STYLE_DFT);
        }
        if (2 * Esp >= dy)
        {
            if (Xpoint == Xend)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx)
        {
            if (Ypoint == Yend)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}

/******************************************************************************
function: Draw a rectangle
parameter:
    Xstart ：Rectangular  Starting Xpoint point coordinates
    Ystart ：Rectangular  Starting Xpoint point coordinates
    Xend   ：Rectangular  End point Xpoint coordinate
    Yend   ：Rectangular  End point Ypoint coordinate
    Color  ：The color of the Rectangular segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the rectangle
******************************************************************************/
void Paint_DrawRectangle(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,
                         UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
{
    if (Xstart > Paint.Width || Ystart > Paint.Height ||
        Xend > Paint.Width || Yend > Paint.Height)
    {
        Debug("Input exceeds the normal display range\r\n");
        return;
    }

    if (Draw_Fill)
    {
        UWORD Ypoint;
        for (Ypoint = Ystart; Ypoint < Yend; Ypoint++)
        {
            Paint_DrawLine(Xstart, Ypoint, Xend, Ypoint, Color, Line_width, LINE_STYLE_SOLID);
        }
    }
    else
    {
        Paint_DrawLine(Xstart, Ystart, Xend, Ystart, Color, Line_width, LINE_STYLE_SOLID);
        Paint_DrawLine(Xstart, Ystart, Xstart, Yend, Color, Line_width, LINE_STYLE_SOLID);
        Paint_DrawLine(Xend, Yend, Xend, Ystart, Color, Line_width, LINE_STYLE_SOLID);
        Paint_DrawLine(Xend, Yend, Xstart, Yend, Color, Line_width, LINE_STYLE_SOLID);
    }
}

/******************************************************************************
function: Use the 8-point method to draw a circle of the
            specified size at the specified position->
parameter:
    X_Center  ：Center X coordinate
    Y_Center  ：Center Y coordinate
    Radius    ：circle Radius
    Color     ：The color of the ：circle segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the Circle
******************************************************************************/
void Paint_DrawCircle(UWORD X_Center, UWORD Y_Center, UWORD Radius,
                      UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
{
    if (X_Center > Paint.Width || Y_Center >= Paint.Height)
    {
        Debug("Paint_DrawCircle Input exceeds the normal display range\r\n");
        return;
    }

    // Draw a circle from(0, R) as a starting point
    int16_t XCurrent, YCurrent;
    XCurrent = 0;
    YCurrent = Radius;

    // Cumulative error,judge the next point of the logo
    int16_t Esp = 3 - (Radius << 1);

    int16_t sCountY;
    if (Draw_Fill == DRAW_FILL_FULL)
    {
        while (XCurrent <= YCurrent)
        { // Realistic circles
            for (sCountY = XCurrent; sCountY <= YCurrent; sCountY++)
            {
                Paint_DrawPoint(X_Center + XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 1
                Paint_DrawPoint(X_Center - XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 2
                Paint_DrawPoint(X_Center - sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 3
                Paint_DrawPoint(X_Center - sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 4
                Paint_DrawPoint(X_Center - XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 5
                Paint_DrawPoint(X_Center + XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 6
                Paint_DrawPoint(X_Center + sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 7
                Paint_DrawPoint(X_Center + sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            }
            if (Esp < 0)
                Esp += 4 * XCurrent + 6;
            else
            {
                Esp += 10 + 4 * (XCurrent - YCurrent);
                YCurrent--;
            }
            XCurrent++;
        }
    }
    else
    { // Draw a hollow circle
        while (XCurrent <= YCurrent)
        {
            Paint_DrawPoint(X_Center + XCurrent, Y_Center + YCurrent, Color, Line_width, DOT_STYLE_DFT); // 1
            Paint_DrawPoint(X_Center - XCurrent, Y_Center + YCurrent, Color, Line_width, DOT_STYLE_DFT); // 2
            Paint_DrawPoint(X_Center - YCurrent, Y_Center + XCurrent, Color, Line_width, DOT_STYLE_DFT); // 3
            Paint_DrawPoint(X_Center - YCurrent, Y_Center - XCurrent, Color, Line_width, DOT_STYLE_DFT); // 4
            Paint_DrawPoint(X_Center - XCurrent, Y_Center - YCurrent, Color, Line_width, DOT_STYLE_DFT); // 5
            Paint_DrawPoint(X_Center + XCurrent, Y_Center - YCurrent, Color, Line_width, DOT_STYLE_DFT); // 6
            Paint_DrawPoint(X_Center + YCurrent, Y_Center - XCurrent, Color, Line_width, DOT_STYLE_DFT); // 7
            Paint_DrawPoint(X_Center + YCurrent, Y_Center + XCurrent, Color, Line_width, DOT_STYLE_DFT); // 0

            if (Esp < 0)
                Esp += 4 * XCurrent + 6;
            else
            {
                Esp += 10 + 4 * (XCurrent - YCurrent);
                YCurrent--;
            }
            XCurrent++;
        }
    }
}

/******************************************************************************
function: Show English characters
parameter:
    Xpoint           ：X coordinate
    Ypoint           ：Y coordinate
    Acsii_Char       ：To display the English characters
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void Paint_DrawChar(UWORD Xpoint, UWORD Ypoint, const char Acsii_Char,
                    sFONT *Font, UWORD Color_Foreground, UWORD Color_Background, int index)
{
    UWORD Page, Column;

    if (Xpoint > Paint.Width || Ypoint > Paint.Height)
    {
        Debug("Paint_DrawChar Input exceeds the normal display range\r\n");
        return;
    }

    if (index < 0)
    {
        index = (Acsii_Char - ' ');
    }
    uint32_t Char_Offset = index * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
    // uint32_t Char_Offset = (Acsii_Char - ' ') * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));

    const unsigned char *ptr = &Font->table[Char_Offset];

    for (Page = 0; Page < Font->Height; Page++)
    {
        for (Column = 0; Column < Font->Width; Column++)
        {

            // To determine whether the font background color and screen background color is consistent
            if (FONT_BACKGROUND == Color_Background)
            { // this process is to speed up the scan
                if (*ptr & (0x80 >> (Column % 8)))
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
                // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            }
            else
            {
                if (*ptr & (0x80 >> (Column % 8)))
                {
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
                    // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
                else
                {
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Background);
                    // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
            }
            // One pixel is 8 bits
            if (Column % 8 == 7)
                ptr++;
        } // Write a line
        if (Font->Width % 8 != 0)
            ptr++;
    } // Write all
}

/******************************************************************************
function:	Display the string
parameter:
    Xstart           ：X coordinate
    Ystart           ：Y coordinate
    pString          ：The first address of the English string to be displayed
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void Paint_DrawString_EN(UWORD Xstart, UWORD Ystart, const char *pString,
                         sFONT *Font, UWORD Color_Foreground, UWORD Color_Background)
{
    UWORD Xpoint = Xstart;
    UWORD Ypoint = Ystart;

    if (Xstart > Paint.Width || Ystart > Paint.Height)
    {
        Debug("Paint_DrawString_EN Input exceeds the normal display range\r\n");
        return;
    }

    while (*pString != '\0')
    {
        // if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
        if ((Xpoint + Font->Width) > Paint.Width)
        {
            Xpoint = Xstart;
            Ypoint += Font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if ((Ypoint + Font->Height) > Paint.Height)
        {
            Xpoint = Xstart;
            Ypoint = Ystart;
        }
        Paint_DrawChar(Xpoint, Ypoint, *pString, Font, Color_Foreground, Color_Background, -1);

        // The next character of the address
        pString++;

        // The next word of the abscissa increases the font of the broadband
        Xpoint += Font->Width;
    }
}

/******************************************************************************
function:	Display nummber
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    Nummber          : The number displayed
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
#define ARRAY_LEN 255
void Paint_DrawNum(UWORD Xpoint, UWORD Ypoint, int32_t Nummber,
                   sFONT *Font, UWORD Color_Foreground, UWORD Color_Background)
{

    int16_t Num_Bit = 0, Str_Bit = 0;
    uint8_t Str_Array[ARRAY_LEN] = {0}, Num_Array[ARRAY_LEN] = {0};
    uint8_t *pStr = Str_Array;

    if (Xpoint > Paint.Width || Ypoint > Paint.Height)
    {
        Debug("Paint_DisNum Input exceeds the normal display range\r\n");
        return;
    }

    // Converts a number to a string
    while (Nummber)
    {
        Num_Array[Num_Bit] = Nummber % 10 + '0';
        Num_Bit++;
        Nummber /= 10;
    }

    // The string is inverted
    while (Num_Bit > 0)
    {
        Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
        Str_Bit++;
        Num_Bit--;
    }

    // show
    Paint_DrawString_EN(Xpoint, Ypoint, (const char *)pStr, Font, Color_Background, Color_Foreground);
}

/******************************************************************************
function:	Display time
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    pTime            : Time-related structures
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void Paint_DrawTime(UWORD Xstart, UWORD Ystart, String time, sFONT *Font, UWORD Color_Foreground, UWORD Color_Background)
{
    // Paint_DrawTime(150, 80, &sPaint_time, &Font20, WHITE, BLACK);
    // uint8_t value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    UWORD Dx = Font->Width;
    Serial.println(int(time[0]) + int(time[1]));
    Paint_DrawChar(Xstart, Ystart, time[0], Font, Color_Foreground, Color_Background, time[0] - '0');
    Paint_DrawChar(Xstart + Dx, Ystart, time.charAt(1), Font, Color_Foreground, Color_Background, time.charAt(1) - '0');
    Paint_DrawChar(Xstart + Dx * 2 - 2, Ystart, time.charAt(2), Font, Color_Foreground, Color_Background, 11);
    Paint_DrawChar(Xstart + Dx * 3 - 4, Ystart, time.charAt(3), Font, Color_Foreground, Color_Background, time.charAt(3) - '0');
    Paint_DrawChar(Xstart + Dx * 4 - 4, Ystart, time.charAt(4), Font, Color_Foreground, Color_Background, time.charAt(4) - '0');
}

void Paint_DrawWeatherTime(UWORD Xstart, UWORD Ystart, String time, sFONT *Font, UWORD Color_Foreground, UWORD Color_Background)
{
    UWORD Dx = Font->Width;
    Serial.println(int(time[0]) + int(time[1]));
    Paint_DrawChar(Xstart, Ystart, time[0], Font, Color_Foreground, Color_Background, time[0] - '0');
    Paint_DrawChar(Xstart + Dx, Ystart, time.charAt(1), Font, Color_Foreground, Color_Background, time.charAt(1) - '0');
    Paint_DrawChar(Xstart + Dx * 2 - 2, Ystart, time.charAt(2), Font, Color_Foreground, Color_Background, 11);
    Paint_DrawChar(Xstart + Dx * 3 - 4, Ystart, time.charAt(3), Font, Color_Foreground, Color_Background, time.charAt(3) - '0');
    Paint_DrawChar(Xstart + Dx * 4 - 4, Ystart, time.charAt(4), Font, Color_Foreground, Color_Background, time.charAt(4) - '0');
}

void Paint_DrawWeatherInfo(UWORD Xstart, UWORD Ystart, String temp, String humidity, sFONT *Font, UWORD Color_Foreground, UWORD Color_Background)
{
    // Paint_DrawTime(150, 80, &sPaint_time, &Font20, WHITE, BLACK);
    // uint8_t value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    UWORD Dx = Font->Width;
    int index = 0;
    for (uint8_t i = 0; i < temp.length(); i++)
    {
        Paint_DrawChar(Xstart + Dx * i, Ystart, temp[i], Font, Color_Foreground, Color_Background, temp[i] - '0');
        index = i;
    }
    Paint_DrawChar(Xstart + Dx * index + 7, Ystart, '°', Font, Color_Foreground, Color_Background, 12);
    Paint_DrawChar(Xstart + Dx * index + 12, Ystart, 'C', Font, Color_Foreground, Color_Background, 13);
    for (uint8_t i = 0; i < humidity.length(); i++)
    {
        index = index + i;
        Paint_DrawChar(Xstart + Dx * (index + 1) + 14, Ystart, humidity[i], Font, Color_Foreground, Color_Background, humidity[i] - '0');
    }
    Paint_DrawChar(Xstart + Dx * (index + 2) + 14, Ystart, '%', Font, Color_Foreground, Color_Background, 10);
}

/******************************************************************************
function:	Display monochrome bitmap
parameter:
    image_buffer ：A picture data converted to a bitmap
info:
    Use a computer to convert the image into a corresponding array,
    and then embed the array directly into Imagedata.cpp as a .c file.
******************************************************************************/
void Paint_DrawBitMap(const unsigned char *image_buffer)
{
    UWORD x, y;
    UDOUBLE Addr = 0;

    for (y = 0; y < Paint.HeightByte; y++)
    {
        for (x = 0; x < Paint.WidthByte; x++)
        { // 8 pixel =  1 byte
            Addr = x + y * Paint.WidthByte;
            Paint.Image[Addr] = (unsigned char)image_buffer[Addr];
        }
    }
}

/******************************************************************************
function:	Display image
parameter:
    image            ：Image start address
    xStart           : X starting coordinates
    yStart           : Y starting coordinates
    xEnd             ：Image width
    yEnd             : Image height
******************************************************************************/
void Paint_DrawImage(const unsigned char *image_buffer, UWORD xStart, UWORD yStart, UWORD W_Image, UWORD H_Image)
{
    UWORD x, y;
    UWORD w_byte = (W_Image % 8) ? (W_Image / 8) + 1 : W_Image / 8;
    UDOUBLE Addr = 0;
    UDOUBLE pAddr = 0;
    for (y = 0; y < H_Image; y++)
    {
        for (x = 0; x < w_byte; x++)
        { // 8 pixel =  1 byte
            Addr = x + y * w_byte;
            pAddr = x + (xStart / 8) + ((y + yStart) * Paint.WidthByte);
            Paint.Image[pAddr] = (unsigned char)image_buffer[Addr] ^ 0xFF;
        }
    }
}

void Paint_DrawChar_From_File(UWORD Xpoint, UWORD Ypoint, const char Acsii_Char, file_char_acsll *font, UWORD Color_Foreground, UWORD Color_Background)
{
    Serial.println("Paint_DrawChar_From_File");
    UWORD Page, Column;

    if (Xpoint > Paint.Width || Ypoint > Paint.Height)
    {
        Debug("Paint_DrawChar Input exceeds the normal display range\r\n");
        return;
    }

    // uint32_t Char_Offset = (Acsii_Char - ' ') * font->Height * (font->Width / 8 + (font->Width % 8 ? 1 : 0));
    // const unsigned char *ptr = &Font->table[Char_Offset];   // 获得偏置值
    size_t len = font->Height * font->Width / 8;
    uint32_t Char_Offset = (Acsii_Char - ' ') * len;
    uint8_t dataspace[len];
    read(font->filename, dataspace, len, Char_Offset);

    const unsigned char *ptr = dataspace; // 获得偏置值

    for (Page = 0; Page < font->Height; Page++)
    {
        // 遍历字模的各行
        for (Column = 0; Column < font->Width; Column++)
        {
            // 遍历一行
            // To determine whether the font background color and screen background color is consistent
            if (FONT_BACKGROUND == Color_Background)
            { // this process is to speed up the scan
                // 文字的背景色 == 显现的背景色
                if (*ptr & (0x80 >> (Column % 8))) // Column % 8 是字节内的位置
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
                // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            }
            else
            {
                if (*ptr & (0x80 >> (Column % 8)))
                {
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
                    // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
                else
                {
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Background);
                    // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
            }
            // 一字节有八个位，行内序号 % 8 == 7 ，需要跳到下一个字节
            // 存在问题 ： 没有将可能的背景色清空
            // 上面的不是问题，因为字符的宽度，不是字模的宽度！！
            if (Column % 8 == 7)
                ptr++;
        }
        // 各行不足一个字节的补全，输完各行后，跳过补全的位
        if (font->Width % 8 != 0)
            ptr++;
    }
}

void Paint_DrawString_EN_From_File(UWORD Xstart, UWORD Ystart, const char *pString,
                                   file_char_acsll *font, UWORD Color_Foreground, UWORD Color_Background)
{
    Serial.println("Paint_DrawString_EN_From_File");
    UWORD Xpoint = Xstart;
    UWORD Ypoint = Ystart;

    if (Xstart > Paint.Width || Ystart > Paint.Height)
    {
        Debug("Paint_DrawString_EN Input exceeds the normal display range\r\n");
        return;
    }

    while (*pString != '\0')
    {
        // if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
        if ((Xpoint + font->Width) > Paint.Width)
        {
            Xpoint = Xstart;
            Ypoint += font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if ((Ypoint + font->Height) > Paint.Height)
        {
            Xpoint = Xstart;
            Ypoint = Ystart;
        }
        Paint_DrawChar_From_File(Xpoint, Ypoint, *pString, font, Color_Background, Color_Foreground);

        // The next character of the address
        pString++;

        // The next word of the abscissa increases the font of the broadband
        Xpoint += font->Width;
    }
}

void Paint_DrawString_CN_From_File(UWORD Xstart, UWORD Ystart, String str, file_char *font, UWORD Color_Foreground, UWORD Color_Background)
{
    // const char *p_text = pString;
    int x = Xstart, y = Ystart;
    int i, j;
    Serial.print("绘制汉字字符串: ");
    Serial.println(str);
    /* Send the string character by character on EPD */
    int str_length = str.length();
    if (str_length % 3 != 0)
    {
        Serial.println("字符串中包含非汉字字符");
    }
    String str_tmp;
    for (int k = 0; k < str_length; k = k + 3)
    {
        str_tmp = str.substring(k, k + 3);
        int Char_Offset = index_cn(font->index_file, str_tmp.c_str());
        if (Char_Offset < 0)
        {
            Serial.println("定位失败，找不到汉字：");
            Serial.println(str_tmp);
            require_char(str_tmp, font->index_file, font->data_file, font->font_size);
        }
        else
        {
            size_t len = font->Height * font->Width / 8;
            Serial.printf("偏移量 = %d  长度 = %d \n", Char_Offset, len);

            uint8_t dataspace[len];
            read(font->data_file, dataspace, len, Char_Offset * len);

            const unsigned char *ptr = dataspace; // 获得偏置值

            for (size_t k = 0; k < len; k++)
            {
                // Serial.printf("%0#x ", *ptr);
                ptr++;
            }
            // Serial.println("\n\n ");
            ptr = dataspace;

            // const char *ptr = &font->table[Num].matrix[0];

            for (j = 0; j < font->Height; j++)
            {
                for (i = 0; i < font->Width; i++)
                {
                    if (FONT_BACKGROUND == Color_Background)
                    { // this process is to speed up the scan
                        if (*ptr & (0x80 >> (i % 8)))
                        {
                            Paint_SetPixel(x + i, y + j, Color_Foreground);
                            // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                        }
                    }
                    else
                    {
                        if (*ptr & (0x80 >> (i % 8)))
                        {
                            Paint_SetPixel(x + i, y + j, Color_Foreground);
                            // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                        }
                        else
                        {
                            Paint_SetPixel(x + i, y + j, Color_Background);
                            // Paint_DrawPoint(x + i, y + j, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                        }
                    }
                    if (i % 8 == 7)
                    {
                        ptr++;
                    }
                }
                if (font->Width % 8 != 0)
                {
                    ptr++;
                }
            }
        }

        // 移动显示位置
        x += font->Width;
    }
}

void Paint_DrawAllCN(const char *file_name, file_char *font, UWORD Color_Foreground, UWORD Color_Background)
{
    // 此函数未经使用，仅为了编写方便
    int x = 0;
    int y = 0;
    int i, j, m, n;
    size_t len = 16 * 2 * 22; // 16 个汉字  *  每个汉字字模的大小
    uint8_t dataspace[len];   // 用于读取的缓冲区

    int rows = 3753 / 16; // 整个字库需要绘制的总行数
    int hasReadBytes = 0; // 实际读取到的字节数
    const unsigned char *ptr = dataspace;
    for (n = 0; n < rows; n++)
    {
        // 遍历整个字库，每行绘制16个汉字，处理各行
        hasReadBytes = read(font->data_file, dataspace, len, n * len);
        if (hasReadBytes % 44 != 0)
        {
            // 读取数据不完整
            Serial.println("The data read is incomplete");
            return;
        }
        ptr = dataspace;
        for (m = 0; m < hasReadBytes / 44; m++)
        {
            // 处理每行的汉字
            for (j = 0; j < font->Height; j++)
            {
                // 绘制一个字符
                for (i = 0; i < font->Width; i++)
                {
                    if (FONT_BACKGROUND == Color_Background)
                    { // this process is to speed up the scan
                        if (*ptr & (0x80 >> (i % 8)))
                        {
                            Paint_SetPixel(x + i, y + j, Color_Foreground);
                            // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                        }
                    }
                    else
                    {
                        if (*ptr & (0x80 >> (i % 8)))
                        {
                            Paint_SetPixel(x + i, y + j, Color_Foreground);
                            // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                        }
                        else
                        {
                            Paint_SetPixel(x + i, y + j, Color_Background);
                            // Paint_DrawPoint(x + i, y + j, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                        }
                    }
                    if (i % 8 == 7)
                    {
                        ptr++;
                    }
                }
                if (font->Width % 8 != 0)
                {
                    ptr++;
                }
            }

            // 指向下一个位置，处理X坐标
            ptr = ptr + 2 * 22;
            x += font->Width;
        }
        // 换行
        x = 0;
        y = y + 22;
    }
}

void Paint_DrawNum_From_File(UWORD Xpoint, UWORD Ypoint, int32_t Nummber, file_char_acsll *font, UWORD Color_Foreground, UWORD Color_Background)
{
    Serial.println("Paint_DrawNum_From_File");
    int16_t Num_Bit = 0, Str_Bit = 0;
    uint8_t Str_Array[ARRAY_LEN] = {0}, Num_Array[ARRAY_LEN] = {0};
    uint8_t *pStr = Str_Array;

    if (Xpoint > Paint.Width || Ypoint > Paint.Height)
    {
        Debug("Paint_DisNum Input exceeds the normal display range\r\n");
        return;
    }

    // Converts a number to a string
    while (Nummber)
    {
        Num_Array[Num_Bit] = Nummber % 10 + '0';
        Num_Bit++;
        Nummber /= 10;
    }

    // The string is inverted
    while (Num_Bit > 0)
    {
        Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
        Str_Bit++;
        Num_Bit--;
    }

    // show
    Paint_DrawString_EN_From_File(Xpoint, Ypoint, (const char *)pStr, font, Color_Background, Color_Foreground);
}

void Paint_DrawTime_From_File(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, file_char_acsll *font, UWORD Color_Foreground, UWORD Color_Background)
{
    uint8_t value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    UWORD Dx = font->Width;

    // Write data into the cache
    Paint_DrawChar_From_File(Xstart, Ystart, value[pTime->Hour / 10], font, Color_Background, Color_Foreground);
    Paint_DrawChar_From_File(Xstart + Dx, Ystart, value[pTime->Hour % 10], font, Color_Background, Color_Foreground);
    Paint_DrawChar_From_File(Xstart + Dx + Dx / 4 + Dx / 2, Ystart, ':', font, Color_Background, Color_Foreground);
    Paint_DrawChar_From_File(Xstart + Dx * 2 + Dx / 2, Ystart, value[pTime->Min / 10], font, Color_Background, Color_Foreground);
    Paint_DrawChar_From_File(Xstart + Dx * 3 + Dx / 2, Ystart, value[pTime->Min % 10], font, Color_Background, Color_Foreground);
}

void Paint_DrawIcon_From_File(int icon_code, UWORD xStart, UWORD yStart, UWORD W_Image, UWORD H_Image, UWORD Color_Foreground, UWORD Color_Background)
{
    Serial.println("Paint_DrawIcon_From_File");
    UWORD w_byte = (W_Image % 8) ? (W_Image / 8) + 1 : W_Image / 8;
    uint8_t length = w_byte * H_Image;
    uint8_t *dataspace = (uint8_t *)malloc(length);

    get_icon_data(icon_code, dataspace);

    int Page = 0;
    int Column = 0;
    const unsigned char *ptr = dataspace;
    for (Page = 0; Page < H_Image; Page++)
    {
        for (Column = 0; Column < W_Image; Column++)
        {
            if (FONT_BACKGROUND == Color_Background)
            {
                if (*ptr & (0x80 >> (Column % 8)))
                    Paint_SetPixel(xStart + Column, yStart + Page, Color_Foreground);
            }
            else
            {
                if (*ptr & (0x80 >> (Column % 8)))
                {
                    Paint_SetPixel(xStart + Column, yStart + Page, Color_Foreground);
                }
                else
                {
                    Paint_SetPixel(xStart + Column, yStart + Page, Color_Background);
                }
            }
            if (Column % 8 == 7)
                ptr++;
        }
        if (W_Image % 8 != 0)
            ptr++;
    }
    free(dataspace);
}

void verify_icon(const char *filename, uint8_t *dataspace, int index, UWORD xStart, UWORD yStart, UWORD W_Image, UWORD H_Image)
{
    UWORD x, y;
    UWORD w_byte = (W_Image % 8) ? (W_Image / 8) + 1 : W_Image / 8;
    UDOUBLE Addr = 0;
    UDOUBLE pAddr = 0;
    for (y = 0; y < H_Image; y++)
    {
        for (x = 0; x < w_byte; x++)
        { // 8 pixel =  1 byte
            Addr = x + y * w_byte;
            pAddr = x + (xStart / 8) + ((y + yStart) * Paint.WidthByte);
            Paint.Image[pAddr] = (unsigned char)dataspace[Addr];
        }
    }
}

/**
 * @description: 从文件中读取汉字字符并绘制,到末尾则换行
 * @param {UWORD} Xstart :
 * @param {UWORD} Ystart :
 * @param {String} str : 待绘制的字符串
 * @param {file_char} *font : 字符集
 * @param {UWORD} Color_Foreground :
 * @param {UWORD} Color_Background :
 * @return {Any}
 */
UWORD Paint_DrawString_CN_From_File_V2(UWORD Xstart, UWORD Ystart, String str, file_char *font, UWORD Color_Foreground, UWORD Color_Background)
{
    // const char *p_text = pString;
    int x = Xstart, y = Ystart;
    int i, j;
    Serial.print("绘制汉字字符串: ");
    Serial.println(str);
    /* Send the string character by character on EPD */
    int str_length = str.length();
    if (str_length % 3 != 0)
    {
        Serial.println("字符串中包含非汉字字符");
    }
    String str_tmp;
    for (int k = 0; k < str_length; k = k + 3)
    {
        if (x + font->Width > EPD_4IN2_WIDTH || y + font->Height > EPD_4IN2_HEIGHT)
        {
            // 无法绘制一个字符，则退出
            break;
        }
        str_tmp = str.substring(k, k + 3);
        int Char_Offset = index_cn(font->index_file, str_tmp.c_str());
        if (Char_Offset < 0)
        {
            Serial.println("定位失败，找不到汉字：");
            Serial.println(str_tmp);
            require_char(str_tmp, font->index_file, font->data_file, font->font_size);
        }
        else
        {
            size_t len = font->Height * font->Width / 8;
            Serial.printf("偏移量 = %d  长度 = %d \n", Char_Offset, len);

            uint8_t dataspace[len];
            read(font->data_file, dataspace, len, Char_Offset * len);

            const unsigned char *ptr = dataspace; // 获得偏置值

            ptr = dataspace;

            for (j = 0; j < font->Height; j++)
            {
                for (i = 0; i < font->Width; i++)
                {
                    if (FONT_BACKGROUND == Color_Background)
                    { // this process is to speed up the scan
                        if (*ptr & (0x80 >> (i % 8)))
                        {
                            Paint_SetPixel(x + i, y + j, Color_Foreground);
                            // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                        }
                    }
                    else
                    {
                        if (*ptr & (0x80 >> (i % 8)))
                        {
                            Paint_SetPixel(x + i, y + j, Color_Foreground);
                            // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                        }
                        else
                        {
                            Paint_SetPixel(x + i, y + j, Color_Background);
                            // Paint_DrawPoint(x + i, y + j, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                        }
                    }
                    if (i % 8 == 7)
                    {
                        ptr++;
                    }
                }
                if (font->Width % 8 != 0)
                {
                    ptr++;
                }
            }
        }
        // 移动到下一个位置
        x += font->Width;
        if (x + font->Width > EPD_4IN2_WIDTH)
        {
            // 超过最大宽度，向下移动一行
            x = Xstart;
            y = Ystart + font->Height;
        }
    }
    // 返回下一行的h位置
    // x返回到Xstart，则返回y，x不是Xstart，则y移动到下一行
    return y + ((x - Xstart) == 0 ? 0 : font->Height);
}

int is_cn_char_start(unsigned char byte)
{
    // 判断字节是否是汉字的起始字节
    return (byte & 0xE0) == 0xE0;
}

int is_cn_char(unsigned char byte)
{
    // 判断字节是否是汉字的起始字节
    return (byte & 0xC0) == 0x80;
}

int is_en_char(unsigned char byte)
{
    return byte <= 0x7F;
}

/**
 * @description: 绘制包含汉字和英文的字符串
 * @param {UWORD} Xstart :
 * @param {UWORD} Ystart :
 * @param {String} str :
 * @param {file_char} *font :
 * @param {UWORD} Color_Foreground :
 * @param {UWORD} Color_Background :
 * @return {UWORD}返回下一行的h位置
 */
UWORD Paint_DrawString_CN_From_File_V3(UWORD Xstart, UWORD Ystart, String str, file_char *cn_font, sFONT *en_font, UWORD Color_Foreground, UWORD Color_Background)
{
    // const char *p_text = pString;
    int x = Xstart, y = Ystart;
    int i, j;
    Serial.print("绘制汉字字符串: ");
    Serial.println(str);
    int str_length = str.length();
    int y_diff = (cn_font->Height - en_font->Height) * 2 / 3;
    if (str_length % 3 != 0)
    {
        Serial.println("字符串中包含非汉字字符");
    }
    String str_tmp;
    for (int k = 0; k < str_length;)
    {
        if (x + cn_font->Width > EPD_4IN2_WIDTH || y + cn_font->Height > EPD_4IN2_HEIGHT)
        {
            break;
        }

        // 判断下一个字符的类型
        if (is_en_char(str[k]))
        {
            // Paint_DrawString_EN(32, 12, WiFi.localIP().toString().c_str(), &Font12, BLACK, WHITE);
            // Paint_DrawChar_From_File(x, y, str[k], en_font, Color_Background, Color_Foreground);
            Paint_DrawChar(x, y + y_diff, str[k], en_font, Color_Foreground, Color_Background, -1);
            k = k + 1;
            // 移动到下一个位置
            x += en_font->Width;
            if (x + en_font->Width > EPD_4IN2_WIDTH)
            {
                // 超过最大宽度，向下移动一行
                x = Xstart;
                y = Ystart + en_font->Height;
            }
        }
        else if (is_cn_char_start(str[k]) && is_cn_char(str[k + 1]) && is_cn_char(str[k + 2]))
        {
            str_tmp = str.substring(k, k + 3);
            int Char_Offset = index_cn(cn_font->index_file, str_tmp.c_str());
            if (Char_Offset < 0)
            {
                Serial.println("定位失败，找不到汉字：");
                Serial.println(str_tmp);
                require_char(str_tmp, cn_font->index_file, cn_font->data_file, cn_font->font_size);
            }
            else
            {
                size_t len = cn_font->Height * cn_font->Width / 8;
                Serial.printf("偏移量 = %d  长度 = %d \n", Char_Offset, len);

                uint8_t dataspace[len];
                read(cn_font->data_file, dataspace, len, Char_Offset * len);

                const unsigned char *ptr = dataspace; // 获得偏置值

                ptr = dataspace;

                for (j = 0; j < cn_font->Height; j++)
                {
                    for (i = 0; i < cn_font->Width; i++)
                    {
                        if (FONT_BACKGROUND == Color_Background)
                        { // this process is to speed up the scan
                            if (*ptr & (0x80 >> (i % 8)))
                            {
                                Paint_SetPixel(x + i, y + j, Color_Foreground);
                                // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                            }
                        }
                        else
                        {
                            if (*ptr & (0x80 >> (i % 8)))
                            {
                                Paint_SetPixel(x + i, y + j, Color_Foreground);
                                // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                            }
                            else
                            {
                                Paint_SetPixel(x + i, y + j, Color_Background);
                                // Paint_DrawPoint(x + i, y + j, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                            }
                        }
                        if (i % 8 == 7)
                        {
                            ptr++;
                        }
                    }
                    if (cn_font->Width % 8 != 0)
                    {
                        ptr++;
                    }
                }
            }
            k = k + 3;

            // 移动到下一个位置
            x += cn_font->Width;
            if (x + cn_font->Width > EPD_4IN2_WIDTH)
            {
                // 超过最大宽度，向下移动一行
                x = Xstart;
                y = Ystart + cn_font->Height;
            }
        }

        // // 移动到下一个位置
        // x += cn_font->Width;
        // if (x + cn_font->Width > EPD_4IN2_WIDTH)
        // {
        //     // 超过最大宽度，向下移动一行
        //     x = Xstart;
        //     y = Ystart + cn_font->Height;
        // }
    }
    // 返回下一行的h位置
    // x返回到Xstart，则返回y，x不是Xstart，则y移动到下一行
    return y + ((x - Xstart) == 0 ? 0 : cn_font->Height);
}

/**
 * @description:    完善V3的功能：
 *                      添加Xend控制换行位置
 *                  修改返回值规则：
 *                      返回值 = 已经绘制的字节数  +  已经占用的行数 * 1000
 * @param {UWORD} Xstart :ccccc
 * @param {UWORD} Ystart :
 * @param {String} str :
 * @param {file_char} *font :
 * @param {UWORD} Color_Foreground :
 * @param {UWORD} Color_Background :
 * @return {UWORD} 已经绘制的字节数  +  已经占用的行数 * 1000
 */
UWORD Paint_DrawString_CN_From_File_V4(UWORD Xstart, UWORD Xend, UWORD Ystart, UWORD Yend, String str, file_char *cn_font, sFONT *en_font, UWORD Color_Foreground, UWORD Color_Background)
{
    int font_max_height = cn_font->Height > en_font->Height ? cn_font->Height : en_font->Height;
    // int font_max_width = cn_font->Width > en_font->Width ? cn_font->Width : en_font->Width;
    int y_diff = (cn_font->Height - en_font->Height) * 2 / 3; // 英文比汉字高，则为负值，英文绘制时y提升
    int x = Xstart, y = Ystart;                               // 各个字符绘制的位置
    int i;                                                    // 用于遍历字符各行的各个像素【一个字节八个像素】
    int j;                                                    // 用于遍历字符各行
    int k = 0;                                                // 用于遍历str_length个字节
    int line = 0;                                             // 存储换行的次数
    String str_tmp;
    // Serial.print("绘制汉字字符串: ");
    // Serial.println(str);
    int str_length = str.length();

    // int offset = 1000;
    while (k < str_length)
    {
        // 判断下一个字符的类型
        if (is_en_char(str[k]))
        {
            if (x + en_font->Width > EPD_4IN2_WIDTH || y + font_max_height > EPD_4IN2_HEIGHT)
            {
                break;
            }
            Paint_DrawChar(x, y + y_diff, str[k], en_font, Color_Foreground, Color_Background, -1);
            k = k + 1;
            // 移动到下一个位置
            x += en_font->Width;
            if (x + en_font->Width > EPD_4IN2_WIDTH)
            {
                // 超过最大宽度，向下移动一行
                x = Xstart;
                y = y + font_max_height;
                line = line + 1;
            }
        }
        else if (is_cn_char_start(str[k]) && is_cn_char(str[k + 1]) && is_cn_char(str[k + 2]))
        {
            if (x + cn_font->Width > EPD_4IN2_WIDTH || y + font_max_height > EPD_4IN2_HEIGHT)
            {
                // offset = 0;
                break;
            }
            str_tmp = str.substring(k, k + 3);
            int Char_Offset = index_cn(cn_font->index_file, str_tmp.c_str());
            if (Char_Offset < 0)
            {
                Serial.println("定位失败，找不到汉字：");
                Serial.println(str_tmp);
                require_char(str_tmp, cn_font->index_file, cn_font->data_file, cn_font->font_size);
            }
            else
            {
                size_t len = cn_font->Height * cn_font->Width / 8;
                // Serial.printf("偏移量 = %d  长度 = %d \n", Char_Offset, len);

                uint8_t dataspace[len];
                read(cn_font->data_file, dataspace, len, Char_Offset * len);

                const unsigned char *ptr = dataspace; // 获得偏置值

                ptr = dataspace;

                for (j = 0; j < cn_font->Height; j++)
                {
                    for (i = 0; i < cn_font->Width; i++)
                    {
                        if (FONT_BACKGROUND == Color_Background)
                        { // this process is to speed up the scan
                            if (*ptr & (0x80 >> (i % 8)))
                            {
                                Paint_SetPixel(x + i, y + j, Color_Foreground);
                            }
                        }
                        else
                        {
                            if (*ptr & (0x80 >> (i % 8)))
                            {
                                Paint_SetPixel(x + i, y + j, Color_Foreground);
                            }
                            else
                            {
                                Paint_SetPixel(x + i, y + j, Color_Background);
                            }
                        }
                        if (i % 8 == 7)
                        {
                            ptr++;
                        }
                    }
                    if (cn_font->Width % 8 != 0)
                    {
                        ptr++;
                    }
                }
            }
            k = k + 3;

            // 移动到下一个位置
            x += cn_font->Width;
            if (x + cn_font->Width > EPD_4IN2_WIDTH)
            {
                // 超过最大宽度，向下移动一行
                x = Xstart;
                y = y + font_max_height;
                line = line + 1;
            }
        }
    }
    // 返回下一行的h位置
    // x返回到Xstart，则返回y，x不是Xstart，则y移动到下一行
    // return y + ((x - Xstart) == 0 ? 0 : cn_font->Height);

    // 判断x是否回到开头，如果不是回到开头，已经占用的行数 = 换行次数 + 1
    // 返回值 = 已经绘制的字节数  +  已经占用的行数 * 10000
    if ((x - Xstart) != 0)
    {

        line = line + 1;
    }
    else
    {
        Serial.println("换行但是没有使用");
    }
    return k + line * 1000;
    // return k + ((x - Xstart) == 0 ? line : line + 1) * 1000;
}

/**
 * @description:
 *          ===============================================================
 *          【V5版本】
 *          优化了：每个汉字都会重新打开索引和字模文件造成的资源浪费，以及定位汉字
 *                 时，频繁申请内存空间。
 *          ===============================================================
 *          【V4版本】
 *          完善V3的功能：添加Xend控制换行位置
 *          修改返回值规则：返回值 = 已经绘制的字节数  +  已经占用的行数 * 1000
 *          ===============================================================
 * @param {UWORD} Xstart :ccccc
 * @param {UWORD} Ystart :
 * @param {String} str :
 * @param {file_char} *font :
 * @param {UWORD} Color_Foreground :
 * @param {UWORD} Color_Background :
 * @return {UWORD} 已经绘制的字节数  +  已经占用的行数 * 1000
 */
UWORD Paint_DrawString_CN_From_File_V5(UWORD Xstart, UWORD Xend, UWORD Ystart, UWORD Yend, String str, file_char *cn_font, sFONT *en_font, UWORD Color_Foreground, UWORD Color_Background)
{
    int font_max_height = cn_font->Height > en_font->Height ? cn_font->Height : en_font->Height;
    // int font_max_width = cn_font->Width > en_font->Width ? cn_font->Width : en_font->Width;
    int y_diff = (cn_font->Height - en_font->Height) * 2 / 3; // 英文比汉字高，则为负值，英文绘制时y提升
    int x = Xstart, y = Ystart;                               // 各个字符绘制的位置
    int i;                                                    // 用于遍历字符各行的各个像素【一个字节八个像素】
    int j;                                                    // 用于遍历字符各行
    int k = 0;                                                // 用于遍历str_length个字节
    int line = 0;                                             // 存储换行的次数
    String str_tmp;                                           // 指向当前要处理的字符
    int str_length = str.length();                            // 待绘制字符串总长度

    // 索引文件和字模文件的初始化
    File fp;  // 汉字索引文件句柄
    int flen; // 索引文件长度
    File datafp;
    if (LittleFS.exists(cn_font->index_file))
    {
        Serial.printf("定位汉字索引文件%s成功.\n", cn_font->index_file);
        fp = LittleFS.open(cn_font->index_file, "r");
        if (!fp)
        {
            Serial.println("索引文件打开失败.\n");
            return 0;
        }
        else
        {
            fp.seek(0, SeekEnd);
            flen = fp.position();
        }
    }
    else
    {
        Serial.printf("绘制长字符串，定位索引文件%s失败.\n", cn_font->index_file);
        return 0;
    }
    if (LittleFS.exists(cn_font->data_file))
    {
        datafp = LittleFS.open(cn_font->data_file, "r");
        if (!datafp)
        {
            Serial.println("数据文件打开失败.\n");
            return 0;
        }
    }
    else
    {
        Serial.printf("数据文件%s不存在.\n", cn_font->data_file);
        return 0;
    }
    // 索引文件处理时使用的变量
    int charIndex;             // 返回的汉字索引
    int indexBufferLen = 3 * 50; // 定位汉字使用的缓冲区
    unsigned char *indexBuffer = (unsigned char *)malloc(indexBufferLen);
    // 数据文件处理时使用的变量
    size_t charLen = cn_font->Height * cn_font->Width / 8;
    uint8_t charBuffer[charLen];           // 汉字缓冲区
    const unsigned char *ptr = charBuffer; // 用于遍历汉字字模各字节
    while (k < str_length)
    {
        // 判断下一个字符的类型
        if (is_en_char(str[k]))
        {
            if (x + en_font->Width > Xend || y + font_max_height > Yend)
            {
                break;
            }
            Paint_DrawChar(x, y + y_diff, str[k], en_font, Color_Foreground, Color_Background, -1);
            k = k + 1;
            // 移动到下一个位置
            x += en_font->Width;
            if (x + en_font->Width > Xend)
            {
                // 超过最大宽度，向下移动一行
                x = Xstart;
                y = y + font_max_height;
                line = line + 1;
            }
        }
        else if (is_cn_char_start(str[k]) && is_cn_char(str[k + 1]) && is_cn_char(str[k + 2]))
        {
            if (x + cn_font->Width > Xend || y + font_max_height > EPD_4IN2_HEIGHT)
            {
                break;
            }
            str_tmp = str.substring(k, k + 3);
            charIndex = index_cn_v2(fp, flen, str_tmp.c_str(), indexBufferLen, indexBuffer);
            Serial.print("index_cn_v2 : ");
            Serial.println(charIndex);
            if (charIndex < 0)
            {
                Serial.println("定位失败，找不到汉字：");
                Serial.println(str_tmp);
                require_char(str_tmp, cn_font->index_file, cn_font->data_file, cn_font->font_size);
            }
            else
            {
                // Serial.printf("偏移量 = %d  长度 = %d \n", charIndex, charLen);
                // Serial.println("Free heap space: " + String(ESP.getFreeHeap()) + " bytes");

                datafp.seek(charIndex* charLen, SeekSet);
                int len_tmp = datafp.read(charBuffer, charLen);

                ptr = charBuffer;
                for (j = 0; j < cn_font->Height; j++)
                {
                    for (i = 0; i < cn_font->Width; i++)
                    {
                        if (FONT_BACKGROUND == Color_Background)
                        {
                            if (*ptr & (0x80 >> (i % 8)))
                            {
                                Paint_SetPixel(x + i, y + j, Color_Foreground);
                            }
                        }
                        else
                        {
                            if (*ptr & (0x80 >> (i % 8)))
                            {
                                Paint_SetPixel(x + i, y + j, Color_Foreground);
                            }
                            else
                            {
                                Paint_SetPixel(x + i, y + j, Color_Background);
                            }
                        }
                        if (i % 8 == 7)
                        {
                            ptr++;
                        }
                    }
                    if (cn_font->Width % 8 != 0)
                    {
                        ptr++;
                    }
                }
            }
            k = k + 3;

            // 移动到下一个位置
            x += cn_font->Width;
            if (x + cn_font->Width > Xend)
            {
                // 超过最大宽度，向下移动一行
                x = Xstart;
                y = y + font_max_height;
                line = line + 1;
            }
        }
    }

    if ((x - Xstart) != 0)
    {
        // 判断x是否回到开头，如果不是回到开头，已经占用的行数 = 换行次数 + 1
        line = line + 1;
    }
    else
    {
        Serial.println("换行但是没有使用");
    }
    fp.close();
    free(indexBuffer);
    // 返回值 = 已经绘制的字节数  +  已经占用的行数 * 10000
    return k + line * 1000;
    // return k + ((x - Xstart) == 0 ? line : line + 1) * 1000;
}
