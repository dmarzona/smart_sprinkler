#ifndef __CPUMP__
#define __CPUMP__
#include <stdio.h>

class CHalfBridge
{
    public:
        CHalfBridge(int output_pin, int enable_pin, int cs_pin);
        void enableHalfBridge(bool enable);
        void setPwmHalfBridge(int pwm_set);
        void turnOn(void);
        void turnOff(void);
        bool isOn(void);
        float getCurrent(void);

    private:
        int output_pin;
        int enable_pin;
        int current_sensor_pin;
        int pwm;
        bool on;
};

class CPump
{
    public:
        CPump(int output_pin1, int enable_pin1, int cs_pin1, int output_pin2, int enable_pin2, int cs_pin2);
    
        enum PumpEntries
        {
            PUMP_0,
            PUMP_1
        };
        
        void setPwmDirection1(int pwm);
        void setPwmDirection2(int pwm);
        float getCurrent(void);
        void startWorker(void);
        void sendEvent(PumpEntries pump, uint32_t active_time);
        
    private:
        class PumpEvents
        {
            public:
                PumpEvents(PumpEntries in_pump_num, uint32_t in_pump_active_time)
                {
                    pump_num = in_pump_num;
                    pump_active_time = in_pump_active_time;
                };
                PumpEntries pump_num;
                uint32_t    pump_active_time;
        };
        void activatePumpDirection1(bool activate);
        void activatePumpDirection2(bool activate);
        static void worker(void* parameter);
        QueueHandle_t events; 
        CHalfBridge h_bridge1;
        CHalfBridge h_bridge2;
};

#endif