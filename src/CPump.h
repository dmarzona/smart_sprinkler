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
        void activatePumpDirection1(bool activate);
        void activatePumpDirection2(bool activate);
        void setPwmDirection1(int pwm);
        void setPwmDirection2(int pwm);
        float getCurrent(void);
    
    private:
        CHalfBridge h_bridge1;
        CHalfBridge h_bridge2;
};

#endif