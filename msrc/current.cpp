#include "current.h"

Current::Current(uint8_t pin, uint8_t alpha, float multiplier, uint16_t offset) : Voltage(pin, alpha, multiplier), offset_(offset) {}

Current::Current(uint8_t pin, uint8_t alpha) : Voltage(pin, alpha) {}

void Current::update()
{
    value_ = calcAverage(alpha_ / 100.0F, value_, readVoltage(offset_));
    consumption_ = calcConsumption(value_);
#ifdef SIM_SENSORS
    value_ = 12.34;
    consumption_ = 12.34;
#endif
}

float *Current::consumptionP()
{
    return &consumption_;
}