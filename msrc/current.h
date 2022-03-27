#ifndef CURRENT_H
#define CURRENT_H

#include <Arduino.h>
#include "voltage.h"

class Current : public Voltage, Consumption
{
protected:
    float consumption_ = 0;
    uint16_t offset_ = 0;
public:
    Current(uint8_t pin, uint8_t alpha, float multiplier, uint16_t offset);
    Current(uint8_t pin, uint8_t alpha);
    void update();
    float *consumptionP();
};

#endif