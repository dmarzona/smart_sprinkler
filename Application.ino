#include <Adafruit_BMP280.h>
#include "src/CTime.h"
#include "src/WebsiteVariables.h"

Adafruit_BMP280 bmp;
extern CTime current_epoch_time;
extern WebsiteVariables variables;
extern CFlash application_information;
CTime application_time;
CTime pump_activation_time;

bool pump_override = false;

float getCurrentSensor()
{
    int current_sensor_read = analogRead(currentSensorPin);
    float voltage_on_current_sensor = 3.3*(float)(current_sensor_read)/4096.0;
    return voltage_on_current_sensor;
}

void mainApplication(void* parameter)
{
    static int pump_start = false;

    if (!bmp.begin(0x76))
    {
        SendSerialMessage("Sensor not found\n");
    }
    else
    {
        bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                        Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                        Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                        Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                        Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
        SendSerialMessage("Sensor properly set\n");
    }

    application_time = current_epoch_time;

    if (bmp.takeForcedMeasurement())
    {
        variables.UpdateTemperature(bmp.readTemperature());
        variables.UpdatePressure(bmp.readPressure());
        variables.UpdateCurrentSense(getCurrentSensor());
    }

    while(true)
    {   
        // log only every five minutes
        if ((current_epoch_time - application_time) >= 5*MINUTE_IN_SECONDS)
        {
            if (bmp.takeForcedMeasurement())
            {
                variables.UpdateTemperature(bmp.readTemperature());
                variables.UpdatePressure(bmp.readPressure());
                log("Temperature: %.2f *C, Pressure: %.2f Pa", variables.GetTemperature(), variables.GetPressure());
            }

            variables.UpdateCurrentSense(getCurrentSensor());
            log("Voltage at current sensor interface: %.2f V", variables.GetCurrentSense());
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
                analogWrite(pumpPin, application_information.GetPumpPowerRaw());
                log("Pump active");
                break;
            }
        }

        if (((current_epoch_time - pump_activation_time) >= application_information.GetActivationTime()) && pump_start)
        {
            pump_start = false;
            analogWrite(pumpPin, 0);
            log("Pump turn-off");
        }

        if(pump_start)
        {
            variables.UpdateCurrentSense(getCurrentSensor());
            log("Voltage at current sensor interface: %.2f V", variables.GetCurrentSense());
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
    