#include <WiFi.h>
#include "Wi-Fi.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "FreeRTOS.h"

const char* ssid       = WIFI_NAME;
const char* password   = WIFI_PASSWORD;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600*2;
const int   daylightOffset_sec = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

void setup()
{
  xTaskCreatePinnedToCore(
    SerialManagerTask,
    "Serial Task",
    4096,
    NULL,
    1,
    NULL,
    0
  );
  
  SendSerialMessage("Connecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
      vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  SendSerialMessage("CONNECTED!\n");

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
  vTaskDelete(NULL);
}

void loop()
{

}
