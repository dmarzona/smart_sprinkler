#include "src/CTime.h"

CTime current_epoch_time;
CTime last_saved_epoch_time;

void updateTime(void* parameter)
{ 
  timeClient.begin();
  timeClient.update();

  current_epoch_time.UpdateEpoch(timeClient.getEpochTime());
  last_saved_epoch_time = current_epoch_time;
  
  while(true)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    current_epoch_time.Increase();
    if ((current_epoch_time - last_saved_epoch_time) >= DAY_IN_SECONDS && WiFi.status() == WL_CONNECTED)
    {
      timeClient.update();
      time_t temp_epoch_time = current_epoch_time.GetEpoch();
      current_epoch_time.UpdateEpoch(timeClient.getEpochTime());
      last_saved_epoch_time = current_epoch_time;
      int error = abs(int((signed long)(current_epoch_time.GetEpoch()) - (signed long)(temp_epoch_time)));
      log("Current time updated, difference %d", error);
    }    
  }
}
