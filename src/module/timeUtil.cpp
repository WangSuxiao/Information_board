
#include "module/timeUtil.h"
#include <TimeLib.h>


// NTP Servers:
static const char ntpServerName[] = "ntp.aliyun.com";
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
const int timeZone = 8; // 东八区

WiFiUDP udp;

// 发送请求
void sendNTPpacket(const char *address)
{
    memset(packetBuffer, 0, NTP_PACKET_SIZE); // set all bytes in the buffer to 0

    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;          // Stratum, or type of clock
    packetBuffer[2] = 6;          // Polling Interval
    packetBuffer[3] = 0xEC;       // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    udp.beginPacket(address, 123); // NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}


// 获取时间戳
time_t getNtpTime()
{
    while (udp.parsePacket() > 0)
        ;
    sendNTPpacket(ntpServerName); // send an NTP packet to a time server

    delay(1000);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500)
    {
        if (udp.parsePacket() >= NTP_PACKET_SIZE )
        {
            Serial.println("Received NTP Response");

            udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

            unsigned long secsSince1900;
            // convert four bytes starting at location 40 to a long integer
            secsSince1900 = (unsigned long)packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)packetBuffer[43];

            // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
            const unsigned long seventyYears = 2208988800UL;
            unsigned long epoch = secsSince1900 - seventyYears + timeZone * SECS_PER_HOUR;
            Serial.print("Epoch Time: ");
            Serial.println(epoch);
            return epoch;
        }
    }
    return 0;
}


// 将时间戳转换为本地时间
void updataTime(PAINT_TIME &sPaint_time)
{
    time_t timestamp = getNtpTime();
    struct tm timeinfo;
    localtime_r(&timestamp, &timeinfo);

    // extern PAINT_TIME sPaint_time;
    sPaint_time.Year = timeinfo.tm_year + 1900;
    sPaint_time.Month = timeinfo.tm_mon;
    sPaint_time.Day = timeinfo.tm_mday;
    sPaint_time.Hour = timeinfo.tm_hour;
    sPaint_time.Min = timeinfo.tm_min;
    sPaint_time.Sec = timeinfo.tm_sec;
    sPaint_time.Week = timeinfo.tm_wday;

    Serial.print("Year: ");
    Serial.print(timeinfo.tm_year + 1900);
    Serial.print("Month: ");
    Serial.print(timeinfo.tm_mon + 1);
    Serial.print("Day: ");
    Serial.print(timeinfo.tm_mday);
    Serial.print("Hour: ");
    Serial.print(timeinfo.tm_hour);
    Serial.print("Minute: ");
    Serial.print(timeinfo.tm_min);
    Serial.print("Second: ");
    Serial.print(timeinfo.tm_sec);
    Serial.print("Weekday: ");
    Serial.print(timeinfo.tm_wday);
}



