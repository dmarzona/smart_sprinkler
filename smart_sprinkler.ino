#include <WiFi.h>
#include "Wi-Fi.h"
#include "time.h"

const char* ssid       = WIFI_NAME;
const char* password   = WIFI_PASSWORD;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

struct tm current_time;
struct tm time_next_update;

void printTime(struct tm* time_to_print)
{
  Serial.println(time_to_print, "%A, %B %d %Y %H:%M:%S");
}

void getTime(struct tm* timeinfo)
{
  if(!getLocalTime(timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
}

void GetTimeFromInternet()
{
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // Needed to store properly the time!
  getTime(&current_time);

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void setup()
{
  Serial.begin(115200);
  GetTimeFromInternet();
  getTime(&time_next_update);
  time_next_update.tm_min +=1;
}

void loop()
{
  getTime(&current_time);
  if(difftime(mktime(&time_next_update), mktime(&current_time)) < 0)
  {
      GetTimeFromInternet();
      getTime(&time_next_update);
      printTime(&time_next_update);
      time_next_update.tm_min +=1;
  }
  
  delay(1000);
}
