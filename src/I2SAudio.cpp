#include "I2SAudio.h"

void i2sInit()
{
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = 0,
        .dma_buf_count = 64,
        .dma_buf_len = 1024,
    };

    // i2s_config_t i2s_config = {
    //     .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    //     .sample_rate = I2S_SAMPLE_RATE,
    //     .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    //     .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    //     .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    //     .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    //     .dma_buf_count = 8,
    //     .dma_buf_len = 256,
    // };

    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD,
    };

    if (i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL) != ESP_OK)
    {
        printf("Failed installing driver.\n");
    }
    else
    {
        printf("I2S driver installed.\n");
    }

    if (i2s_set_pin(I2S_PORT, &pin_config) != ESP_OK)
    {
        printf("Failed configuring pins.");
    }
    else
    {
        printf("I2S pins set.\n");
    }
    if (i2s_set_clk(I2S_PORT, I2S_SAMPLE_RATE, i2s_bits_per_sample_t(I2S_SAMPLE_BITS), I2S_CHANNEL_MONO) != ESP_OK)
    {
        printf("Failed setting the clock.");
    }
    else
    {
        printf("I2S clock set.\n");
    }
    if (i2s_zero_dma_buffer(I2S_PORT) != ESP_OK)
    {
        printf("Failed cleaning the buffer.");
    }
    else
    {
        printf("I2S DMA buffer cleaned.\n");
    }
}

void i2s_adc(char *i2s_read_buff, uint8_t *flash_write_buff)
{
    size_t bytes_read = 0;

    if (i2s_read(I2S_PORT, (void *)i2s_read_buff, I2S_READ_LEN, &bytes_read, 100) == ESP_OK)
    {
        if (bytes_read > 0)
        {
            i2s_adc_data_scale(flash_write_buff, (uint8_t *)i2s_read_buff, I2S_READ_LEN);
        }
        else
        {
            Serial.println("Bytes lost");
        }
    }
}

int16_t i2s_measurement()
{
    int16_t mic_data = 0;
    int32_t digital_sample = 0;
    size_t bytes_read = 0;
    i2s_read(I2S_PORT, &digital_sample, sizeof(digital_sample), &bytes_read, 10);
    if (bytes_read > 0)
    {
        mic_data = digital_sample >> 16;
    }
    return mic_data;
}

void i2s_adc_data_scale(uint8_t *d_buff, uint8_t *s_buff, uint32_t len)
{
    uint32_t j = 0;
    uint32_t dac_value = 0;
    for (int i = 0; i < len; i += 2)
    {
        dac_value = ((((uint16_t)(s_buff[i + 1] & 0xf) << 8) | ((s_buff[i + 0]))));
        d_buff[j++] = 0;
        d_buff[j++] = dac_value * 256 / 2048;
    }
}

void example_disp_buf(uint8_t *buf, int length)
{
    printf("\n");
    for (int i = 0; i < length; i++)
    {
        printf("%02x", buf[i]);
        if ((i + 1) % 8 == 0)
        {
            printf("\n");
        }
    }
}

void i2s_cleanup(char *i2s_read_buff, uint8_t *flash_write_buff)
{
    free(i2s_read_buff);
    i2s_read_buff = NULL;
    free(flash_write_buff);
    flash_write_buff = NULL;

    i2s_driver_uninstall(I2S_PORT);
}

void wavHeader(byte *header, int wavSize)
{
    header[0] = 'R';
    header[1] = 'I';
    header[2] = 'F';
    header[3] = 'F';
    unsigned int fileSize = wavSize + 44 - 8;
    header[4] = (byte)(fileSize & 0xFF);
    header[5] = (byte)((fileSize >> 8) & 0xFF);
    header[6] = (byte)((fileSize >> 16) & 0xFF);
    header[7] = (byte)((fileSize >> 24) & 0xFF);
    header[8] = 'W';
    header[9] = 'A';
    header[10] = 'V';
    header[11] = 'E';
    header[12] = 'f';
    header[13] = 'm';
    header[14] = 't';
    header[15] = ' ';
    header[16] = 0x10;
    header[17] = 0x00;
    header[18] = 0x00;
    header[19] = 0x00;
    header[20] = 0x01;
    header[21] = 0x00;
    header[22] = 0x01;
    header[23] = 0x00;
    header[24] = 0x80;
    header[25] = 0x3E;
    header[26] = 0x00;
    header[27] = 0x00;
    header[28] = 0x00;
    header[29] = 0x7D;
    header[30] = 0x00;
    header[31] = 0x00;
    header[32] = 0x02;
    header[33] = 0x00;
    header[34] = 0x10;
    header[35] = 0x00;
    header[36] = 'd';
    header[37] = 'a';
    header[38] = 't';
    header[39] = 'a';
    header[40] = (byte)(wavSize & 0xFF);
    header[41] = (byte)((wavSize >> 8) & 0xFF);
    header[42] = (byte)((wavSize >> 16) & 0xFF);
    header[43] = (byte)((wavSize >> 24) & 0xFF);
}