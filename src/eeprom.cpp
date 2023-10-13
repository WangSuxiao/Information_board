// #include <EEPROM.h>
// #include <Arduino.h>

// int eepromAddressFlag = 0;
// int eepromAddressMode = sizeof(int);

// int flag = 1432; // 初始化flag
// int mode = 1101; // 初始化mode

// void setup()
// {
//     EEPROM.begin(512);
//     EEPROM.put(eepromAddressFlag, flag);
//     EEPROM.put(eepromAddressMode, mode);
//     EEPROM.commit();
//     Serial.begin(115200);
// }

// void loop()
// {
//     int readFlag, readMode;
//     EEPROM.get(eepromAddressFlag, readFlag);
//     EEPROM.get(eepromAddressMode, readMode);

//     Serial.println("Data stored in EEPROM:");
//     Serial.print("Stored Flag: ");
//     Serial.println(flag);
//     Serial.print("Stored Mode: ");
//     Serial.println(mode);
//     Serial.println("Data read from EEPROM:");
//     Serial.print("Read Flag: ");
//     Serial.println(readFlag);
//     Serial.print("Read Mode: ");
//     Serial.println(readMode);
//     delay(1000);
// }
