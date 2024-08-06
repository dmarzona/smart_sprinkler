#include <Adafruit_BMP280.h>
#include "src/CTime.h"

Adafruit_BMP280 bmp;
extern CTime current_epoch_time;
CTime application_time;
CTime first_activation;
CTime second_activation;
CTime pump_activation_time;

void mainApplication(void* parameter)
{
    int button_state = false;
    static int pump_start = false;
    
    first_activation.UpdateEpoch("1970-01-01 06:00:00");
    second_activation.UpdateEpoch("1970-01-01 21:00:00");

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

    while(true)
    {
        button_state = digitalRead(buttonPin);
        
        // log only every five minutes
        if ((current_epoch_time - application_time) >= 5*MINUTE_IN_SECONDS)
        {
            if (bmp.takeForcedMeasurement())
            {
                log("Temperature: %.2f *C, Pressure: %.2f Pa", bmp.readTemperature(), bmp.readPressure());
            }
            // store time
            application_time = current_epoch_time;
        }

        if((first_activation.GetHours()   == current_epoch_time.GetHours()   &&
            first_activation.GetMinutes() == current_epoch_time.GetMinutes() &&
            first_activation.GetSeconds() == current_epoch_time.GetSeconds())
            ||
           (second_activation.GetHours()   == current_epoch_time.GetHours()   &&
            second_activation.GetMinutes() == current_epoch_time.GetMinutes() &&
            second_activation.GetSeconds() == current_epoch_time.GetSeconds())
            ||
            button_state == HIGH
          )
        {
            pump_start = true;
            pump_activation_time = current_epoch_time;
            analogWrite(pumpPin, 128);
            log("Pump active");
        }

        if (((current_epoch_time - pump_activation_time) >= 1*MINUTE_IN_SECONDS) && pump_start)
        {
            pump_start = false;
            analogWrite(pumpPin, 0);
            log("Pump turn-off");
        }

        if(button_state == HIGH)
        {
            SendSerialMessage("Pump activated by external button");
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
    