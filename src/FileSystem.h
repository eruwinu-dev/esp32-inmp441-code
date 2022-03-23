#ifndef FileSystem_h
#define FileSystem_h

#include <FreeRTOS.h>
#include "FS.h"
#include "SPIFFS.h"

void SPIFFSInit();
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
int writeFile(fs::FS &fs, const char *path, const char *message);
int writeToWav(fs::FS &fs, const char *path, const byte *message, int length);
int appendFile(fs::FS &fs, const char *path, const char *message);
int closeFile(fs::FS &fs, const char *path);
int deleteFile(fs::FS &fs, const char *path);

#endif