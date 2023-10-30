#ifndef _CHAR_H_
#define _CHAR_H_

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <stdint.h>
#include <stdint.h>
#include "LittleFS.h"

typedef struct
{
    const char *index_file;
    const char *data_file;
    uint16_t Width;
    uint16_t Height;
    uint16_t font_size;
} file_char;

typedef struct _mytFont
{
    const char *filename;
    uint32_t Length;    // 字库大小
    uint16_t Width;     // 字模宽
    uint16_t Height;    // 字模高
    uint16_t Indentation;   // 字模自带缩进
    uint16_t Indentation_s; // 较短的缩进
    uint16_t Indentation_l; // 较大的缩进
    uint16_t Indentation_L; // 最大的缩进
    uint16_t Indentation_t; // 去除顶部的几行
    uint16_t Indentation_b; // 去除底部的几行
} file_char_acsll;

// extern file_char PINGFANG18;
extern file_char PINGFANG16;
extern file_char PINGFANG12;
extern file_char_acsll FXLED36;

extern file_char_acsll TODO_TIME_LIGHT10;
extern file_char_acsll TODO_INFO_LIGHT10;

int read(const char *file_name, uint8_t *data_space, size_t length, uint32_t offset);
int index_cn(const char *file_name, const char *p_text);
bool append(const char *index_filename, const char *data_filename, uint8_t *cn_char, uint8_t *data_space, size_t length);

void require_char(const String &value, const char *index_filename, const char *data_filename, int font_size);

int index_cn_v2(File fp, int filelen, const char *p_text, int bufferlen, unsigned char *buffer);
#endif
