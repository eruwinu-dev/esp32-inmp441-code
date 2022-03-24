#include <Arduino.h>
#include <I2SAudio.h>
#include <FileSystem.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "Huzzah32-Access-Point";
const char *password = "esp32wifi";

AsyncWebServer server(80);

char *i2s_read_buff = (char *)calloc(I2S_READ_LEN, sizeof(char));
uint8_t *flash_write_buff = (uint8_t *)calloc(I2S_READ_LEN, sizeof(char));

const char filename[] = "/recording.wav";

int is_recording = 0;
bool startRecording = false;
bool endRecording = false;

File file;

const long interval = 500;

void createWavFile()
{
	byte header[44];
	wavHeader(header, FLASH_RECORD_SIZE);
	file = SPIFFS.open(filename, FILE_APPEND);
	file.write(header, 44);
}

void asyncServerInit()
{
	Serial.println("Setting Access Point...");
	WiFi.softAP(ssid, password);
	IPAddress IP = WiFi.softAPIP();
	Serial.print("AP IP Address: ");
	Serial.println(IP);

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/index.html", "text/html"); });

	server.on("/style", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/style.css", "text/css"); });

	server.on(
		"/start",
		HTTP_POST,
		[](AsyncWebServerRequest *request) {},
		NULL,
		[](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
		{
			String requestData = String((const char *)data);
			int requestLength = requestData.toInt();
			if (requestLength > 0)
			{
				is_recording = requestLength;
				startRecording = true;
			}

			request->send(200);
		});

	server.on(
		"/end",
		HTTP_POST,
		[](AsyncWebServerRequest *request) {},
		NULL,
		[](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
		{
			endRecording = true;
			request->send(200);
		});

	server.begin();
	Serial.println("Started Server.");
}

void setup()
{
	Serial.begin(115200);
	Serial.println();
	SPIFFSInit();
	deleteFile(SPIFFS, "/recording.wav");
	listDir(SPIFFS, "/", 0);
	asyncServerInit();
	while (1)
	{
		if (startRecording)
		{
			break;
		}
		delay(1);
	}
	i2sInit();
	while (1)
	{
		if (startRecording)
		{
			endRecording = false;
			break;
		}
	}
	createWavFile();
	int flash_wr_size = 0;
	size_t bytes_read = 0;
	for (int i = 0; i < 2; i++)
	{

		i2s_read(I2S_PORT, (void *)i2s_read_buff, I2S_READ_LEN, &bytes_read, 100);
	}
	i2s_zero_dma_buffer(I2S_PORT);
	Serial.println("Recording start.");
	delay(500);
	while (flash_wr_size < FLASH_RECORD_SIZE) // try FLASH_RECORD_SIZE / 1.5
	{
		i2s_adc(i2s_read_buff, flash_write_buff);
		file.write((const byte *)flash_write_buff, I2S_READ_LEN);
		flash_wr_size += I2S_READ_LEN;
	}
	Serial.println();
	file.close();
	i2s_cleanup(i2s_read_buff, flash_write_buff);
	endRecording = true;
	Serial.println("Recording stopped. Server running.");
	server.on("/recording", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/recording.wav", "audio/mpeg"); });
	server.begin();
	while (1)
	{
	}
}
void loop()
{
}
