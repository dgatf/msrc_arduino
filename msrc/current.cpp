#include "current.h"

Current::Current(uint8_t pin, uint8_t alpha, float multiplier, float offset, bool autoOffset) : Voltage(pin, alpha, multiplier), offset_(offset), autoOffset_(autoOffset) {}

Current::Current(uint8_t pin, uint8_t alpha) : Voltage(pin, alpha) {}

void Current::update()
{
    static uint16_t ts = 0;
    if ((uint16_t)(millis() - ts) < ANALOG_SENSOR_INTERVAL)
        return;
    ts = millis();
    if (autoOffset_)
    {
        if (millis() > 5000)
        {
            offset_ = value_;
            autoOffset_ = false;
        }
    }
    value_ = calcAverage(alpha_ / 100.0F, value_, readVoltage());
#ifdef SIM_SENSORS
    value_ = 12.34;
    consumption_ = 12.34;
#endif
    value_ = abs(value_ - offset_);
    if (!autoOffset_)
        consumption_ += calcConsumption(value_);
}

float *Current::consumptionP()
{
    return &consumption_;
}