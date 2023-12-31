#ifndef I2C_H
#define I2C_H

#include "constants.h"
#include <Arduino.h>

#define I2C_BIG_ENDIAN 1
#define I2C_LITTLE_ENDIAN 0

class I2C
{
private:
protected:
public:
    I2C();
    int16_t readInt(uint8_t device, uint8_t reg, uint8_t endian);
    uint16_t readUInt(uint8_t device, uint8_t reg, uint8_t endian);
    uint8_t readBytes(uint8_t device, uint8_t reg, uint8_t *data, uint8_t length);
    uint8_t writeBytes(uint8_t device, uint8_t reg, uint8_t *data, uint8_t length);
    uint8_t writeBytes(uint8_t device, uint8_t reg);
};

#endif