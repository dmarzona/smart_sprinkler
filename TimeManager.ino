#include "src/CTime.h"

CTime current_epoch_time;
CTime last_saved_epoch_time;

void updateTime(void* parameter)
{ 
  timeClient.begin();
  timeClient.update();

  current_epoch_time.UpdateEpoch(timeClient.getEpochTime());
  last_saved_epoch_time.UpdateEpoch(current_epoch_time.GetEpoch());
  
  while(true)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    current_epoch_time.Increase();
    if (current_epoch_time.GetDay() >= last_saved_epoch_time.GetDay() + 1)
    {
      timeClient.update();
      time_t temp_epoch_time = current_epoch_time.GetEpoch();
      current_epoch_time.UpdateEpoch(timeClient.getEpochTime());
      last_saved_epoch_time.UpdateEpoch(current_epoch_time.GetEpoch());
      int error = abs(int((signed long)(current_epoch_time.GetEpoch()) - (signed long)(temp_epoch_time)));
      SendSerialMessage("Current time updated, difference %d\n", error);
    }    
    SendSerialMessage("%s\n", current_epoch_time.getDateString());
  }
}
