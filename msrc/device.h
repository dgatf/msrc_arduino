#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include "constants.h"
#include "functions.h"
#include "softserial.h"
#include "hardserial.h"

class Device
{
private:
protected:
public:
    Device();
    virtual ~Device();
    virtual void update() = 0;
};

#endif