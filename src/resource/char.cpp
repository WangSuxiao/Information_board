#include "resource/char.h"


String serverUrl = "http://www.wangsuxiao.com";
String resource = "/addchar?ch=";

file_char_acsll FXLED36{"/font/FXLED36.tf", 13680, 32, 36};

file_char PINGFANG18{"/font/cn18/index.txt", "/font/cn18/pingfang_18.tf", 24, 33, 18};
file_char PINGFANG16{"/font/cn16/index.txt", "/font/cn16/pingfang_16.tf", 24, 21, 16};
file_char PINGFANG12{"/font/cn12/index.txt", "/font/cn12/pingfang_12.tf", 16, 22, 12};
file_char CN18{"/font/cn18/index.txt", "/font/cn18/data", 24, 24, 18};

int read(const char *file_name, uint8_t *data_space, size_t length, uint32_t offset)
{
    if (LittleFS.exists(file_name))
    {
        // Serial.printf("%s FOUND.\n", file_name);
    }
    else
    {
        Serial.printf("%s NOT FOUND.\n", file_name);
    }

    // 建立File对象用于从SPIFFS中读取文件
    File dataFile = LittleFS.open(file_name, "r");

    // 读取文件内容并且通过串口监视器输出文件信息
    //  for(int i=0; i<dataFile.size(); i++){
    //    Serial.printf("%0#x \t",dataFile.read());
    //    delay(200);
    //  }

    dataFile.seek(offset, SeekSet);
    int len = dataFile.read(data_space, length);
    // Serial.printf("read %d bytes \n",len);
    // Serial.println();

    // 完成文件读取后关闭文件
    dataFile.close();

    return len;
}

/**
 * @description: 像数据文件和索引文件中追加汉字
 * @param {char} *index_file : 索引文件
 * @param {char} *data_file : 数据文件
 * @param {String} cnchar : 追加的汉字
 * @param {uint8_t} *data_space : 汉字的字模数据
 * @param {size_t} length : 字模数据的长度
 * @return {Any}
 */
bool append(const char *index_filename, const char *data_filename, uint8_t *cn_char, uint8_t *data_space, size_t length)
{
    // 判断空间够不够
    FSInfo fs_info;
    LittleFS.info(fs_info);
    Serial.println("");
    Serial.println("添加数据");

    // Get available space in bytes
    uint32_t available_space = fs_info.totalBytes - fs_info.usedBytes;
    Serial.print("Total space: ");
    Serial.print(fs_info.totalBytes);
    Serial.print(" bytes | Used space: ");
    Serial.print(fs_info.usedBytes);
    Serial.print(" bytes | Available space: ");
    Serial.print(available_space);
    Serial.println(" bytes");

    // 判断文件是否存在
    File index_file = LittleFS.open(index_filename, "a");
    if (!index_file)
    {
        Serial.println("Failed to open index_file for writing");
        return false;
    }
    else
    {
        Serial.println("open index_file index_filename");
    }

    File data_file = LittleFS.open(data_filename, "a");
    if (!data_file)
    {
        Serial.println("Failed to open data_file for writing");
        return false;
    }
    else
    {
        Serial.println("open index_file data_file");
    }
    index_file.write(cn_char, 3);
    index_file.close();

    data_file.write(data_space, length);
    data_file.close();

    return true;
}

int is_chinese_character_start(unsigned char byte)
{
    // 判断字节是否是汉字的起始字节
    return (byte & 0xE0) == 0xE0;
}

int index_cn(const char *file_name, const char *p_text)
{
    int index = -1;
    // 打开文件
    if (LittleFS.exists(file_name))
    {
        Serial.printf("%s FOUND.\n", file_name);
    }
    else
    {
        Serial.printf("%s NOT FOUND.\n", file_name);
    }

    File fp = LittleFS.open(file_name, "r");

    if (!fp)
    {
        Serial.println("Failed to open file.\n");
        return index;
    }
    else
    {
        Serial.println("open index file SUCCESS.\n");
    }

    fp.seek(0, SeekEnd);
    int filelen = fp.position();
    fp.seek(0);

    int bufferlen = 3 * 50;
    int epoch = (filelen + bufferlen - 1) / bufferlen;
    unsigned char *buffer = (unsigned char *)malloc(bufferlen);
    Serial.println("Free heap space: " + String(ESP.getFreeHeap()) + " bytes");
    Serial.println("batch: " + String(bufferlen) + "epoch: " + String(epoch));
    int pointer = 0;
    int counter = 0;
    for (int _epoch = 0; _epoch < epoch; _epoch++)
    {
        index = -1;
        fp.read(buffer, bufferlen);
        for (pointer = 0; pointer < bufferlen; pointer++)
        {
            counter = 1;
            if (is_chinese_character_start(buffer[pointer]))
            {
                counter = 1;
                while ((buffer[pointer + counter] & 0xC0) == 0x80)
                {
                    counter++;
                }
                if (counter == 3)
                {
                    if (buffer[pointer] == *p_text && buffer[pointer + 1] == *(p_text + 1) && buffer[pointer + 2] == *(p_text + 2))
                    {
                        index = pointer;
                        break;

                    }
                }
                else
                {
                    Serial.println("ERROR : 汉字编码长度不是3");
                }
                pointer += counter - 1;
            }
            else
            {
                Serial.print("ERROR : 第一个字符不是汉字字符");
                Serial.println(pointer);
                Serial.println(" ");
                Serial.println(_epoch);
            }
        }
        if (index != -1)
        {
            index = (index + _epoch * bufferlen) / 3;
            Serial.printf("The character is found at position %d in the file.", index);
            break;
        }

    }

    fp.close();
    free(buffer);
    return index;
}



String urlEncode(const String &value)
{
    String encodedValue = "";
    char c;
    char code[4];
    // for (unsigned int i = 0; i < 3; i++)
    for (unsigned int i = 0; i < value.length(); i++)
    {
        c = value.charAt(i);
        if (isalnum(c))
        {
            encodedValue += c;
        }
        else
        {
            sprintf(code, "%%%02X", c);
            encodedValue += code;
        }
    }
    Serial.print("encodedValue : ");
    Serial.println(encodedValue);
    return encodedValue;
}

/**
 * @description:
 * @param {String} &value : 汉字字符
 * @param {char} *index_filename : 索引文件，新增汉字的索引的追加位置
 * @param {char} *data_filename : 数据文件，新增汉字的字模的追加位置
 * @param {int} font_size : 字号
 * @return {Any}
 */
void require_char(const String &value,const char *index_filename, const char *data_filename,int font_size)
{

    WiFiClient client;
    HTTPClient http;
    String response = "";

    // WiFiClient client;
    // HTTPClient http;

    // http.begin(client, serverUrl + resource + urlEncode(value.substring(0,3)));

    http.begin(client, "www.wangsuxiao.com", 80, "/querycharacter");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String post_data = "character=" + value + "&font_size=" + font_size;
    int httpCode = http.POST(post_data);

    // int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        response = http.getString();
        Serial.print("response : ");
        Serial.print(response);
        Serial.print("length : ");
        Serial.print(response.length());
        Serial.print("value : ");
        Serial.println(value);
        Serial.println(value.length());
        Serial.println(value.substring(0,3));

        // Process the response data
        int ch_size = (response.length() + 1) / 5;
        unsigned char dataArray[ch_size];
        int dataIndex = 0;
        int startIndex = 0;
        int commaIndex = response.indexOf(',');
        while (commaIndex != -1 && dataIndex < ch_size)
        {
            String hexValue = response.substring(startIndex, commaIndex);
            unsigned char value = (unsigned char)strtol(hexValue.c_str(), NULL, 16);
            dataArray[dataIndex] = value;
            dataIndex++;
            startIndex = commaIndex + 1;
            commaIndex = response.indexOf(',', startIndex);
        }

        String hexValue = response.substring(startIndex);
        unsigned char lastvalue = (unsigned char)strtol(hexValue.c_str(), NULL, 16);
        dataArray[dataIndex] = lastvalue;
        Serial.print("请求字模");
        Serial.print(value.substring(0,3));
        for (int i = 0; i < ch_size; i++)
        {
            Serial.print("dataArray[");
            Serial.print(i);
            Serial.print("] = 0x");
            Serial.print(dataArray[i], HEX);
            Serial.print(" ");
        }

        append(index_filename, data_filename,(uint8_t *)(value.substring(0,3).c_str()),dataArray,ch_size);
        // for (int i = 0; i < ch_size; i++)
        // {
        //     Serial.print("dataArray[");
        //     Serial.print(i);
        //     Serial.print("] = 0x");
        //     Serial.println(dataArray[i], HEX);
        // }
    }
    else
    {
        Serial.println("HTTP GET request failed.");
    }
    http.end();
}
