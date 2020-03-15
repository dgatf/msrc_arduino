#ifndef VOLTAGE_H
#define VOLTAGE_H

#if F_CPU == 16000000UL
#define BOARD_VCC 5
#else
#define BOARD_VCC 3.3
#endif

#include <Arduino.h>
#include "smartport.h"

class VoltageInterface : public AbstractDevice
{
protected:
    uint8_t pin_;
    float value_ = 0;
    uint8_t alpha_;
    float readVoltage();
public:
    VoltageInterface(uint8_t pin, uint8_t alpha);
    virtual float read(uint8_t index);
};

#endif