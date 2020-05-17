#ifndef DEVICE_H
#define DEVICE_H

//#define SIM_SENSORS

#include <Arduino.h>

class AbstractDevice
{
private:
protected:
    float calcAverage(float alpha, float value, float newValue);

public:
    AbstractDevice();
    virtual float read(uint8_t index) = 0;
};

#endif