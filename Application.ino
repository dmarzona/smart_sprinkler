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

bool setHumiditySensor()
{
    Wire.begin(SDA_PIN, SCL_PIN);
    return sht40.begin(&Wire);
}

void mainApplication(void* parameter)
{
    if (!setHumiditySensor())
    {
        SendSerialMessage("Sensor not found\n");
    }
    else
    {
        SendSerialMessage("Sensor properly set\n");
    }

    pump.setPwmDirection1(application_information.GetPumpPowerRaw(0));
    pump.setPwmDirection2(application_information.GetPumpPowerRaw(1));
    pump.startWorker();

    application_time = current_epoch_time;

    variables.UpdateTemperature(sht40.readTemperatureC());
    variables.UpdateHumidity(sht40.readHumidityRH());
    variables.UpdateCurrentSense(pump.getCurrent());

    while(true)
    {   
        // log only every five minutes
        if ((current_epoch_time - application_time) >= 5*MINUTE_IN_SECONDS)
        {

            variables.UpdateTemperature(sht40.readTemperatureC());
            variables.UpdateHumidity(sht40.readHumidityRH());
            SendSerialMessage("Temperature: %.2f *C, RH: %.2f %\n", variables.GetTemperature(), variables.GetHumidity());

            variables.UpdateCurrentSense(pump.getCurrent());
            SendSerialMessage("Voltage at current sensor interface: %.2f V\n", variables.GetCurrentSense());
            // store time
            application_time = current_epoch_time;
        }

        for (uint8_t j = 0; j < PUMP_MAX_NUMBER; j++)
        {
            for(uint8_t i = 0; i < application_information.GetStoredActiveTimes(j); i++)
            {
                CTime temp_time;

                temp_time.UpdateEpoch(application_information.GetActiveTime(j,i));
                
                if(temp_time.timeDifference(current_epoch_time) == 0)
                {
                    pump.sendEvent((CPump::PumpEntries)j, application_information.GetActivationTime(j));
                }
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
    