/*
 * @Author       : WangSuxiao
 * @Date         : 2023-07-18 20:10:08
 * @LastEditTime : 2023-09-30 22:22:54
 * @Description  : 字体相关结构体的定义
 * @Tips         :
 */
#ifndef __FONTS_H
#define __FONTS_H


/* Max size of bitmap will based on a font24 (17x24) */
#define MAX_HEIGHT_FONT         41
#define MAX_WIDTH_FONT          32
#define OFFSET_BITMAP           54

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
// typedef struct _tFont
// {
//   const uint8_t *table;
//   uint16_t Width;
//   uint16_t Height;
// } sFONT;

typedef struct _tFont
{
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;
} sFONT;

extern sFONT Font24;
extern sFONT Font20;
extern sFONT Font16;
extern sFONT Font12;
extern sFONT Font8;
extern sFONT Time14;
extern sFONT Weather22;

#ifdef __cplusplus
}
#endif

#endif /* __FONTS_H */


