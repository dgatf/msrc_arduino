#ifndef CURRENT_H
#define CURRENT_H

#include <Arduino.h>
#include "voltage.h"

class Current : public Voltage, Consumption
{
protected:
    float consumption_ = 0;
    float offset_ = 0;
    bool autoOffset_ = false;
public:
    Current(uint8_t pin, uint8_t alpha, float multiplier, float offset, bool autoOffset);
    Current(uint8_t pin, uint8_t alpha);
    void update();
    float *consumptionP();
};

#endif