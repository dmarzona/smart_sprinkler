#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "freertos/FreeRTOS.h"
#include "src/CFlash.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600*2;
const int   daylightOffset_sec = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

const int buttonPin = 1;
const int currentSensorPin = 5;

SemaphoreHandle_t wait_for_connection_semaphore;

CFlash application_information;

void checkWifiStatus(void* parameter)
{
  while(true)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      SendSerialMessage("WiFi lost, reconnecting...\n");
      WiFi.disconnect();
      WiFi.reconnect();
    }
    vTaskDelay(60000 / portTICK_PERIOD_MS);
  }
}

void setup()
{
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
