#include <Wire.h>
#include <SHT40.h>
#include "src/CTime.h"
#include "src/WebsiteVariables.h"
#include "src/CPump.h"

// I2C pins for humidity sensor
#define SDA_PIN 8
#define SCL_PIN 18

// Bridge pin definitions
#define PWM1  9
#define PWM2 48
#define EN1  46
#define EN2  47
#define CS1   3
#define CS2  14

SHT40 sht40;
extern CTime current_epoch_time;
extern WebsiteVariables variables;
extern CFlash application_information;
CTime application_time;
CTime pump_activation_time;
CPump pump(PWM1, EN1, CS1, PWM2, EN2, CS2);

bool pump_override = false;

bool setHumiditySensor()
{
    Wire.begin(SDA_PIN, SCL_PIN);
    return sht40.begin(&Wire);
}

void mainApplication(void* parameter)
{
    static int pump_start = false;

    if (!setHumiditySensor())
    {
        SendSerialMessage("Sensor not found\n");
    }
    else
    {
        SendSerialMessage("Sensor properly set\n");
    }

    application_time = current_epoch_time;

    variables.UpdateTemperature(sht40.readTemperatureC());
    variables.UpdatePressure(sht40.readHumidityRH());
    variables.UpdateCurrentSense(pump.getCurrent());

    while(true)
    {   
        // log only every five minutes
        if ((current_epoch_time - application_time) >= 5*MINUTE_IN_SECONDS)
        {

            variables.UpdateTemperature(sht40.readTemperatureC());
            variables.UpdatePressure(sht40.readHumidityRH());
            SendSerialMessage("Temperature: %.2f *C, RH: %.2f %\n", variables.GetTemperature(), variables.GetPressure());

            variables.UpdateCurrentSense(pump.getCurrent());
            SendSerialMessage("Voltage at current sensor interface: %.2f V\n", variables.GetCurrentSense());
            // store time
            application_time = current_epoch_time;
        }

        for(uint8_t i = 0; i < application_information.GetStoredActiveTimes(); i++)
        {
            CTime temp_time;

            temp_time.UpdateEpoch(application_information.GetActiveTime(i));
            
            if( temp_time.GetHours()   == current_epoch_time.GetHours()   &&
                temp_time.GetMinutes() == current_epoch_time.GetMinutes() &&
                temp_time.GetSeconds() == current_epoch_time.GetSeconds()
                ||
                pump_override)
            {
                pump_start = true;
                pump_override = false;
                pump_activation_time = current_epoch_time;
                pump.setPwmDirection1(application_information.GetPumpPowerRaw());
                pump.activatePumpDirection1(true);
                SendSerialMessage("Pump active\n");
                break;
            }
        }

        if (((current_epoch_time - pump_activation_time) >= application_information.GetActivationTime()) && pump_start)
        {
            pump_start = false;
            pump.activatePumpDirection1(false);
            SendSerialMessage("Pump turn-off\n");
        }

        if(pump_start)
        {
            variables.UpdateCurrentSense(pump.getCurrent());
            SendSerialMessage("Voltage at current sensor interface: %.2f V\n", variables.GetCurrentSense());
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
    