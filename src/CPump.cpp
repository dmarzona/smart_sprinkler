#include "Arduino.h"
#include "CPump.h"
#include "driver/ledc.h"

// PWM settings
const int pwmFreq = 20000;   // 20 kHz
const int pwmResolution = 8; // 8-bit (0-255)

extern void SendSerialMessage(const char *format, ...);

CHalfBridge::CHalfBridge(int output_pin, int enable_pin, int cs_pin):
     output_pin(output_pin)
    ,enable_pin(enable_pin)
    ,current_sensor_pin(cs_pin)
    ,pwm(0)
    ,on(false)
{
    pinMode(enable_pin, OUTPUT);
    ledcAttach(output_pin, pwmFreq, pwmResolution);
}

void CHalfBridge::enableHalfBridge(bool enable)
{
    digitalWrite(enable_pin, enable ? HIGH : LOW);
}

void CHalfBridge::setPwmHalfBridge(int pwm_set)
{
    pwm = constrain(pwm_set, 0, 255);
}

void CHalfBridge::turnOn(void)
{
    ledcWrite(output_pin, pwm);
    on = true;
}

void CHalfBridge::turnOff(void)
{
    ledcWrite(output_pin, 0);
    on = false;
}

bool CHalfBridge::isOn(void)
{
    return on;
}

float CHalfBridge::getCurrent(void)
{
    return (float)analogRead(current_sensor_pin)/1.0f; // ToDo: set proper scale value
}

//CPump

CPump::CPump(int output_pin1, int enable_pin1, int cs_pin1, int output_pin2, int enable_pin2, int cs_pin2):
     h_bridge1(output_pin1, enable_pin1, cs_pin1)
    ,h_bridge2(output_pin2, enable_pin2, cs_pin2)
{
    events = xQueueCreate(5, sizeof(PumpEvents));
}

void CPump::activatePumpDirection1(bool activate)
{
    h_bridge1.enableHalfBridge(activate);
    h_bridge2.enableHalfBridge(activate);
    if (activate)
    {
        h_bridge1.turnOn();
        h_bridge2.turnOff();
    }
    else
    {
        h_bridge1.turnOff();
        h_bridge2.turnOff();
    }
}

void CPump::activatePumpDirection2(bool activate)
{
    h_bridge1.enableHalfBridge(activate);
    h_bridge2.enableHalfBridge(activate);
    if (activate)
    {
        h_bridge2.turnOn();
        h_bridge1.turnOff();
    }
    else
    {
        h_bridge2.turnOff();
        h_bridge1.turnOff();
    }
}

void CPump::setPwmDirection1(int pwm)
{
    h_bridge1.setPwmHalfBridge(pwm);
}

void CPump::setPwmDirection2(int pwm)
{
    h_bridge2.setPwmHalfBridge(pwm);
}

float CPump::getCurrent(void)
{
    if(h_bridge1.isOn())
    {
        return h_bridge1.getCurrent();
    }
    else if(h_bridge2.isOn())
    {
        return h_bridge2.getCurrent();
    }
    else
    {
        return 0.0f;
    }
}

void CPump::sendEvent(PumpEntries pump, uint32_t active_time)
{
    PumpEvents temp_event(pump, active_time);
    xQueueSend(events, (void *)&temp_event, 0);
}

void CPump::startWorker(void)
{
    xTaskCreatePinnedToCore(
        CPump::worker,
        "Pump Worker Task",
        4096,
        this,
        1,
        NULL,
        0
    );
}

void CPump::worker(void* parameter)
{
    PumpEvents received_event(PUMP_0, 0);
    CPump* pump = static_cast<CPump*>(parameter);
    
    while(true)
    {
        if(xQueueReceive(pump->events, (void *)&received_event, portMAX_DELAY) == pdTRUE)
        {
            SendSerialMessage("Pump %d activated for %d s\n", received_event.pump_num, received_event.pump_active_time);
            switch(received_event.pump_num)
            {
                case PUMP_0:
                {
                    pump->activatePumpDirection1(true);
                    vTaskDelay(received_event.pump_active_time*1000/portTICK_PERIOD_MS);
                    pump->activatePumpDirection1(false);
                    break;
                }
                case PUMP_1:
                {
                    pump->activatePumpDirection2(true);
                    vTaskDelay(received_event.pump_active_time*1000/portTICK_PERIOD_MS);
                    pump->activatePumpDirection2(false);
                    break;
                }
            }
        }
    }
}
    
