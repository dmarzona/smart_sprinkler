#ifndef __WEBSITE_VARIABLES__
#define __WEBSITE_VARIABLES__
#include <stdio.h>

#define BUFFER_SIZE 100

class WebsiteVariables
{
    public:
        WebsiteVariables();
        void UpdateString(const char* new_string);
        char* getString(int string_to_get);
        void UpdateTemperature(float new_temperature);
        void UpdatePressure(float new_pressure);
        void UpdateCurrentSense(float new_current_sense);
        void UpdatePumpPower(int power);
        void UpdateActivationTime(int time);
        float GetTemperature();
        float GetPressure();
        float GetCurrentSense();
        int GetPumpPower();
        int GetPumpPowerRaw();
        int GetActivationTime();
    
    private:
        float temperature;
        float pressure;
        float current_sense;
        int activation_time;
        int pump_power;
        char last_log_lines[5][BUFFER_SIZE];
};
#endif
