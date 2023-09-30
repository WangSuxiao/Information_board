/*
 * @Author       : WangSuxiao
 * @Date         : 2023-09-26 09:10:55
 * @LastEditTime : 2023-09-26 09:16:07
 * @Description  : 文件系统操作
 * @Tips         :
 */

#include "LittleFS.h"

void createFolderIfNotExists(const char *folderPath)
{
    if (!LittleFS.exists(folderPath))
    {
        if (LittleFS.mkdir(folderPath))
        {
            Serial.println("Folder created successfully!");
        }
        else
        {
            Serial.println("Failed to create folder!");
        }
    }
    else
    {
        Serial.println("Folder already exists.");
    }
}

void createFileIfNotExists(const char *filePath)
{
    if (!LittleFS.exists(filePath))
    {
        File file = LittleFS.open(filePath, "w");
        if (file)
        {
            file.close();
            Serial.println("File created successfully!");
        }
        else
        {
            Serial.println("Failed to create file!");
        }
    }
    else
    {
        Serial.println("File already exists.");
    }
}

void printFileSystemInfo()
{
    // 判断空间够不够
    FSInfo fs_info;
    LittleFS.info(fs_info);

    Serial.println("文件系统信息 ： ");

    // Get available space in bytes
    uint32_t available_space = fs_info.totalBytes - fs_info.usedBytes;
    Serial.print("Total space: ");
    Serial.print(fs_info.totalBytes);
    Serial.print(" bytes | Used space: ");
    Serial.print(fs_info.usedBytes);
    Serial.print(" bytes | Available space: ");
    Serial.print(available_space);
    Serial.println(" bytes");
}
