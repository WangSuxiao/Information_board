#ifndef _MODULE_TODO_H_
#define _MODULE_TODO_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "board/DEV_Config.h"
#include "board/EPD.h"
#include "board/GUI_Paint.h"
#include "config.h"
#include "module/todoManager.h"

#define DRAW_TODO_X_START 20
#define DRAW_TODO_X_END 380
#define DRAW_TODO_Y_START 140
#define DRAW_TODO_Y_END 300


// UWORD drawTODO_OnePage(sFONT *time_font, file_char *cn_font, sFONT *en_font, int start, int paddingTop);
UWORD drawTODO_OnePage(file_char_acsll *time_font, file_char *cn_font, sFONT *en_font, int start, TodoManager &todoManager, int paddingTop);
UWORD drawTODO_OnePage_V2(file_char_acsll *time_font, file_char *cn_font, file_char_acsll *en_font, int start, TodoManager &todoManager, int paddingTop);
u32_t drawTODO_OnePage_V3(file_char_acsll *time_font, file_char *cn_font, file_char_acsll *en_font, int itemStart, int charStart, TodoManager &todoManager, int paddingTop);

#endif
