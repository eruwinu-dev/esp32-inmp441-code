#include "FileSystem.h"

void SPIFFSInit()
{
    while (!SPIFFS.begin(true))
        ;
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\r\n", dirname);
    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
    }
    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels)
            {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

int writeFile(fs::FS &fs, const char *path, const char *message)
{
    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        return -1;
    }
    if (file.print(message))
    {
        file.close();
        return 1;
    }
    else
    {
        file.close();
        return 0;
    }
}

int writeToWav(fs::FS &fs, const char *path, const byte *message, int length)
{
    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        return -1;
    }
    if (file.write(message, length))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int appendFile(fs::FS &fs, const char *path, const char *message)
{
    // Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        return -1;
    }
    if (file.print(message))
    {
        return 1;
    }
    else
    {
        // file.close();
        return 0;
    }
}

int closeFile(fs::FS &fs, const char *path)
{
    File file = fs.open(path, FILE_READ);
    file.close();
    return 1;
}
int deleteFile(fs::FS &fs, const char *path)
{
    fs.remove(path);
    return 1;
}