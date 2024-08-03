#include <Adafruit_BMP280.h>
#include "src/CTime.h"

Adafruit_BMP280 bmp;
extern CTime current_epoch_time;
CTime application_time;
CTime first_activation;
CTime second_activation;

void mainApplication(void* parameter)
{
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
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
    