#include "Arduino.h"
#include "WebsiteVariables.h"

WebsiteVariables::WebsiteVariables():
      humidity(0)
    , temperature(0)
    , current_sense(0)
{

}

void WebsiteVariables::UpdateTemperature(float new_temperature)
{
    temperature = new_temperature;
}

void WebsiteVariables::UpdateHumidity(float new_humidity)
{
    humidity = new_humidity;
}

void WebsiteVariables::UpdateCurrentSense(float new_current_sense)
{
    current_sense = new_current_sense;
}

float WebsiteVariables::GetTemperature()
{
    return temperature;
}

float WebsiteVariables::GetHumidity()
{
    return humidity;
}

float WebsiteVariables::GetCurrentSense()
{
    return current_sense;
}

