#ifndef __WEBSITE_VARIABLES__
#define __WEBSITE_VARIABLES__
#include <stdio.h>

class WebsiteVariables
{
    public:
        WebsiteVariables();
        void UpdateTemperature(float new_temperature);
        void UpdateHumidity(float new_humidity);
        void UpdateCurrentSense(float new_current_sense);
        float GetTemperature();
        float GetHumidity();
        float GetCurrentSense();
    
    private:
        float temperature;
        float humidity;
        float current_sense;
};
#endif
