static const unsigned long DAY_IN_SECONDS = 86400L;
static const unsigned long HOUR_IN_SECONDS = 3600L;
static const unsigned long MINUTE_IN_SECONDS = 60L;

struct STime
{
  int seconds;
  int minutes;
  int hours;
};

time_t current_epoch_time = 0;
time_t last_saved_epoch_time = 0;
struct STime current_time;

static int CalculateSeconds(time_t epoch)
{
  return (int)(epoch % 60);
}

static int CalculateMinutes(time_t epoch)
{
  return (int)((epoch % HOUR_IN_SECONDS) / MINUTE_IN_SECONDS);
}

static int CalculateHours(time_t epoch)
{
  return (int)((epoch % DAY_IN_SECONDS) / HOUR_IN_SECONDS);
}

void StoreTime(time_t epoch, struct STime* time_struct)
{
  time_struct->hours = CalculateHours(epoch);
  time_struct->minutes = CalculateMinutes(epoch);
  time_struct->seconds = CalculateSeconds(epoch);
}

unsigned long GetEpoch()
{
  return current_epoch_time;
}

void updateTime(void* parameter)
{ 
  timeClient.begin();
  timeClient.update();

  current_epoch_time = timeClient.getEpochTime();
  last_saved_epoch_time = current_epoch_time;
  
  while(true)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    current_epoch_time++;
    if (current_epoch_time >= last_saved_epoch_time + DAY_IN_SECONDS)
    {
      timeClient.update();
      time_t temp_epoch_time = current_epoch_time;
      current_epoch_time = timeClient.getEpochTime();
      last_saved_epoch_time = current_epoch_time;
      int error = int((signed long)(current_epoch_time - (signed long)(temp_epoch_time)));
      snprintf(buffer, 50, "Current time updated, difference %d", error);
      SendSerialMessage(buffer);
    }

    StoreTime(current_epoch_time, &current_time);
    
    snprintf(buffer, 50, "%02d:%02d:%02d\n", current_time.hours, current_time.minutes, current_time.seconds);
    SendSerialMessage(buffer);
  }
}
