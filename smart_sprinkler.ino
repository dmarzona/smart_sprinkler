#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "freertos/FreeRTOS.h"

const char* ssid       = WIFI_NAME;
const char* password   = WIFI_PASSWORD;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600*2;
const int   daylightOffset_sec = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

const int pumpPin = 16;
const int buttonPin = 1;
const int currentSensorPin = 5;

SemaphoreHandle_t wait_for_connection_semaphore;

void checkWifiStatus(void* parameter)
{
  while(true)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      log("WiFi lost, reconnecting...");
      WiFi.disconnect();
      WiFi.reconnect();
    }
    vTaskDelay(60000 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  pinMode(pumpPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLDOWN);

  wait_for_connection_semaphore = xSemaphoreCreateBinary();
  if(NULL != wait_for_connection_semaphore)
  {
    xSemaphoreGive(wait_for_connection_semaphore);
  }

  xTaskCreatePinnedToCore(
    SerialManagerTask,
    "Serial Task",
    4096,
    NULL,
    1,
    NULL,
    0
  );

  if(xSemaphoreTake(wait_for_connection_semaphore, (TickType_t)10) == pdTRUE)
  {
    xTaskCreatePinnedToCore(
      PreStartApplication,
      "Pre-start",
      4096,
      NULL,
      1,
      NULL,
      0
    );
  }

  // Wait until connection to local network is done
  while (xSemaphoreTake(wait_for_connection_semaphore, (TickType_t)10) == pdFALSE)
  {
  }

  xTaskCreatePinnedToCore(
    SDCardManager,
    "SD Manager",
    4096,
    NULL,
    1,
    NULL,
    0
  );

  xTaskCreatePinnedToCore(
    updateTime,
    "Time",
    4096,
    NULL,
    1,
    NULL,
    0
  );

  xTaskCreatePinnedToCore(
    mainApplication,
    "Application",
    4096,
    NULL,
    1,
    NULL,
    0
  );

  xTaskCreatePinnedToCore(
    webSiteTask,
    "WebSite",
    4096,
    NULL,
    1,
    NULL,
    0
  );

  xTaskCreatePinnedToCore(
    checkWifiStatus,
    "WiFi check",
    2048,
    NULL,
    1,
    NULL,
    0
  );

  vTaskDelete(NULL);
}

void loop()
{

}
