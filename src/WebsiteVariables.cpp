#include "Arduino.h"
#include "WebsiteVariables.h"

WebsiteVariables::WebsiteVariables():
      pressure(0)
    , temperature(0)
    , current_sense(0)
    , activation_time(60)
    , pump_power(128)
{
    for(int i = 0; i <5; i++)
    {
        memset(last_log_lines[i], 0, BUFFER_SIZE);
    }
}
void WebsiteVariables::UpdateString(const char* new_string)
{
    static int string_count = 0;
    strcpy(last_log_lines[string_count%5], new_string);
    string_count++;
}

char* WebsiteVariables::getString(int string_to_get)
{
    return last_log_lines[string_to_get];
}

void WebsiteVariables::UpdateTemperature(float new_temperature)
{
    temperature = new_temperature;
}

void WebsiteVariables::UpdatePressure(float new_pressure)
{
    pressure = new_pressure;
}

void WebsiteVariables::UpdateCurrentSense(float new_current_sense)
{
    current_sense = new_current_sense;
}

void WebsiteVariables::UpdatePumpPower(int power)
{
    pump_power = power*255/100;
}

void WebsiteVariables::UpdateActivationTime(int time)
{
    activation_time = time;
}


float WebsiteVariables::GetTemperature()
{
    return temperature;
}

float WebsiteVariables::GetPressure()
{
    return pressure;
}

float WebsiteVariables::GetCurrentSense()
{
    return current_sense;
}

int WebsiteVariables::GetPumpPower()
{
    return pump_power*100/255;
}

int WebsiteVariables::GetPumpPowerRaw()
{
    return pump_power;
}

int WebsiteVariables::GetActivationTime()
{
    return activation_time;
}
