#ifndef I2SAudio_h
#define I2SAudio_h

#include <Arduino.h>
#include <FreeRTOS.h>
#include <driver/i2s.h>
#include <inttypes.h>
#include <SPIFFS.h>

#define I2S_WS 15
#define I2S_SD 32
#define I2S_SCK 14
#define I2S_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE 16000
#define I2S_SAMPLE_BITS 16
#define I2S_READ_LEN (16 * 1024)
#define I2S_CHANNEL_NUM 1
#define RECORD_TIME 5
// #define FLASH_RECORD_SIZE (I2S_CHANNEL_NUM * I2S_SAMPLE_RATE * I2S_SAMPLE_BITS / 8 * RECORD_TIME)
#define FLASH_RECORD_SIZE (I2S_READ_LEN * RECORD_TIME * 2)

void i2sInit();
void i2s_adc(char *i2s_read_buff, uint8_t *flash_write_buff);
void i2s_adc_data_scale(uint8_t *d_buff, uint8_t *s_buff, uint32_t len);
int16_t i2s_measurement();
void example_disp_buf(uint8_t *buf, int length);
void i2s_cleanup(char *i2s_read_buff, uint8_t *flash_write_buff);
void wavHeader(unsigned char *header, int wavSize);
#endif