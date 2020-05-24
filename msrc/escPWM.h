#ifndef ESCPWM_H
#define ESCPWM_H

#define COMP_TO_MICROS ((float)8000000UL / F_CPU)

//#define DEBUG_ESC

#include <Arduino.h>
#include "device.h"

extern void (*TIMER1_CAPT_handlerP)();
extern void (*TIMER1_OVF_handlerP)();

class EscPWMInterface : public AbstractDevice
{
private:
    uint8_t alphaRpm_;
    float rpm_;
    static void TIMER1_CAPT_handler();
    static void TIMER1_OVF_handler();

protected:
public:
    EscPWMInterface(uint8_t alphaRpm);
    void begin();
    float read(uint8_t index);
};

#endif