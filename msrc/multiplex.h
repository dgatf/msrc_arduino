#ifndef MULTIPLEX_H
#define MULTIPLEX_H

#include <Arduino.h>
#include "softserial.h"
#include "hardserial.h"
#include "sensor.h"
#include "constants.h"

#include "escHW3.h"
#include "escHW4.h"
#include "escPWM.h"
#include "escCastle.h"
#include "escKontronik.h"
#include "escApdF.h"
#include "escApdHV.h"
#include "voltage.h"
#include "current.h"
#include "ntc.h"
#include "pressure.h"
#include "bmp280.h"
#include "ms5611.h"
#include "bn220.h"
#include "configeeprom.h"
#include "pwmout.h"

#define MULTIPLEX_SERIAL_TIMEOUT 2000

#define MULTIPLEX_WAIT 0
#define MULTIPLEX_SEND 1

class Multiplex
{
private:
    AbstractSerial &serial_;
    SensorMultiplex *sensorMultiplexP[16] = {NULL};
    CircularBuffer<Device> *deviceBufferP;
    void setConfig();
    void sendPacket(uint8_t address);

public:
    Multiplex(AbstractSerial &serial);
    ~Multiplex();
    void begin();
    void addSensor(SensorMultiplex *newSensorMultiplexP);
    void update();
};

#endif