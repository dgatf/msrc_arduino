#include "multiplex.h"

Multiplex::Multiplex(AbstractSerial &serial) : serial_(serial)
{
}

Multiplex::~Multiplex()
{
}

void Multiplex::begin()
{
    deviceBufferP = new CircularBuffer<Device>;
    serial_.begin(38400, SERIAL__8N1 | SERIAL__HALF_DUP);
    serial_.setTimeout(MULTIPLEX_SERIAL_TIMEOUT);
    pinMode(LED_BUILTIN, OUTPUT);
    setConfig();
}

void Multiplex::addSensor(SensorMultiplex *newSensorMultiplexP)
{
    static uint8_t cont = 1;
    if (cont < 16)
    {
        sensorMultiplexP[cont] = newSensorMultiplexP;
        cont++;
    }
}

void Multiplex::sendPacket(uint8_t address)
{
    if (!sensorMultiplexP[address])
        return;
    digitalWrite(LED_BUILTIN, HIGH);
    serial_.write(address << 4 | sensorMultiplexP[address]->dataId());
    int16_t value = sensorMultiplexP[address]->valueFormatted();
    serial_.write(value);
    serial_.write(value >> 8);
    digitalWrite(LED_BUILTIN, LOW);
#ifdef DEBUG
    DEBUG_PRINT_HEX((uint8_t)(address << 4 | sensorMultiplexP[address]->dataId()));
    DEBUG_PRINT(" ");
    DEBUG_PRINT_HEX((uint16_t)value);
    DEBUG_PRINTLN();
#endif
}

void Multiplex::update()
{
    uint8_t status = MULTIPLEX_WAIT;
    uint8_t address = 0;
#if defined(SIM_RX)
    static uint16_t ts = 0;
    if ((uint16_t)(millis() - ts) > 100)
    {
        status = MULTIPLEX_SEND;
        ts = millis();
        address++;
        if (address > 16)
            address = 0;
    }
#else
    if (serial_.availableTimeout() == 1)
    {
        address = serial_.read();
        if (address < 16)
        {
            status = MULTIPLEX_SEND;
        }
    }
#endif
    if (status == MULTIPLEX_SEND)
    {
        sendPacket(address);
    }

    // update device
    if (deviceBufferP->current())
    {
        deviceBufferP->current()->update();
        deviceBufferP->next();
    }
}

void Multiplex::setConfig()
{
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_PWM)
    {
        SensorMultiplex *sensorMultiplexP;
        EscPWM *esc;
        esc = new EscPWM(ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM));
        esc->begin();
        deviceBufferP->add(esc);
        sensorMultiplexP = new SensorMultiplex(FHSS_RPM, esc->rpmP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_HW_V3)
    {
        SensorMultiplex *sensorMultiplexP;
        EscHW3 *esc;
        esc = new EscHW3(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM));
        esc->begin();
        deviceBufferP->add(esc);
        sensorMultiplexP = new SensorMultiplex(FHSS_RPM, esc->rpmP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_HW_V4)
    {
        SensorMultiplex *sensorMultiplexP;
        EscHW4 *esc;
        esc = new EscHW4(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP), 0);
        esc->begin();
        deviceBufferP->add(esc);
        PwmOut pwmOut;
        pwmOut.setRpmP(esc->rpmP());
        sensorMultiplexP = new SensorMultiplex(FHSS_RPM, esc->rpmP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_CURRENT, esc->currentP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, esc->voltageP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_TEMP, esc->tempFetP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_TEMP, esc->tempBecP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, esc->cellVoltageP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_CONSUMPTION, esc->consumptionP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_CASTLE)
    {
        SensorMultiplex *sensorMultiplexP;
        EscCastle *esc;
        esc = new EscCastle(ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        deviceBufferP->add(esc);
        sensorMultiplexP = new SensorMultiplex(FHSS_RPM, esc->rpmP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_CURRENT, esc->currentP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, esc->voltageP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, esc->rippleVoltageP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_CURRENT, esc->becCurrentP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, esc->becVoltageP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_TEMP, esc->temperatureP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, esc->cellVoltageP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_CONSUMPTION, esc->consumptionP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_KONTRONIK)
    {
        SensorMultiplex *sensorMultiplexP;
        EscKontronik *esc;
        esc = new EscKontronik(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        deviceBufferP->add(esc);
        sensorMultiplexP = new SensorMultiplex(FHSS_RPM, esc->rpmP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_CURRENT, esc->currentP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, esc->voltageP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_CURRENT, esc->becCurrentP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, esc->becVoltageP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_TEMP, esc->tempFetP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_TEMP, esc->tempFetP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, esc->cellVoltageP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_CONSUMPTION, esc->consumptionP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_APD_F)
    {
        SensorMultiplex *sensorMultiplexP;
        EscApdF *esc;
        esc = new EscApdF(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        deviceBufferP->add(esc);
        //PwmOut pwmOut;
        //pwmOut.setRpmP(esc->rpmP());
        sensorMultiplexP = new SensorMultiplex(FHSS_RPM, esc->rpmP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_CURRENT, esc->currentP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, esc->voltageP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_TEMP, esc->tempP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_CONSUMPTION, esc->consumptionP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, esc->cellVoltageP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_APD_HV)
    {
        SensorMultiplex *sensorMultiplexP;
        EscApdHV *esc;
        esc = new EscApdHV(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        deviceBufferP->add(esc);
        //PwmOut pwmOut;
        //pwmOut.setRpmP(esc->rpmP());
        sensorMultiplexP = new SensorMultiplex(FHSS_RPM, esc->rpmP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_CURRENT, esc->currentP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, esc->voltageP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_TEMP, esc->tempP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_CONSUMPTION, esc->consumptionP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, esc->cellVoltageP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_GPS)
    {
        SensorMultiplex *sensorMultiplexP;
        Bn220 *gps;
        gps = new Bn220(GPS_SERIAL, GPS_BAUD_RATE);
        gps->begin();
        deviceBufferP->add(gps);
        sensorMultiplexP = new SensorMultiplex(FHSS_SPEED, gps->spdP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_ALTITUDE, gps->altP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VARIO, gps->varioP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_DISTANCE, gps->distP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_AIRSPEED)
    {
        SensorMultiplex *sensorMultiplexP;
        Pressure *pressure;
        pressure = new Pressure(PIN_PRESSURE, ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT));
        deviceBufferP->add(pressure);
        sensorMultiplexP = new SensorMultiplex(FHSS_SPEED, pressure->valueP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_VOLTAGE1)
    {
        SensorMultiplex *sensorMultiplexP;
        Voltage *voltage;
        voltage = new Voltage(PIN_VOLTAGE1, ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), VOLTAGE1_MULTIPLIER);
        deviceBufferP->add(voltage);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, voltage->valueP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_VOLTAGE2)
    {
        SensorMultiplex *sensorMultiplexP;
        Voltage *voltage;
        voltage = new Voltage(PIN_VOLTAGE2, ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), VOLTAGE2_MULTIPLIER);
        deviceBufferP->add(voltage);
        sensorMultiplexP = new SensorMultiplex(FHSS_VOLTAGE, voltage->valueP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_CURRENT)
    {
        SensorMultiplex *sensorMultiplexP;
        Current *current;
        current = new Current(PIN_CURRENT, ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), CURRENT_MULTIPLIER, CURRENT_OFFSET, CURRENT_AUTO_OFFSET);
        deviceBufferP->add(current);
        sensorMultiplexP = new SensorMultiplex(FHSS_CURRENT, current->valueP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_CONSUMPTION, current->consumptionP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_NTC1)
    {
        SensorMultiplex *sensorMultiplexP;
        Ntc *ntc;
        ntc = new Ntc(PIN_NTC1, ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        deviceBufferP->add(ntc);
        sensorMultiplexP = new SensorMultiplex(FHSS_TEMP, ntc->valueP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_NTC2)
    {
        SensorMultiplex *sensorMultiplexP;
        Ntc *ntc;
        ntc = new Ntc(PIN_NTC2, ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        deviceBufferP->add(ntc);
        sensorMultiplexP = new SensorMultiplex(FHSS_TEMP, ntc->valueP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_I2C1_TYPE == I2C_BMP280)
    {
        SensorMultiplex *sensorMultiplexP;
        Bmp280 *bmp;
        bmp = new Bmp280(CONFIG_I2C1_ADDRESS, ALPHA(CONFIG_AVERAGING_ELEMENTS_VARIO));
        bmp->begin();
        deviceBufferP->add(bmp);
        sensorMultiplexP = new SensorMultiplex(FHSS_TEMP, bmp->temperatureP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_ALTITUDE, bmp->altitudeP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VARIO, bmp->varioP());
        addSensor(sensorMultiplexP);
    }
    if (CONFIG_I2C1_TYPE == I2C_MS5611)
    {
        SensorMultiplex *sensorMultiplexP;
        MS5611 *bmp;
        bmp = new MS5611(CONFIG_I2C1_ADDRESS, ALPHA(CONFIG_AVERAGING_ELEMENTS_VARIO));
        bmp->begin();
        deviceBufferP->add(bmp);
        sensorMultiplexP = new SensorMultiplex(FHSS_TEMP, bmp->temperatureP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_ALTITUDE, bmp->altitudeP());
        addSensor(sensorMultiplexP);
        sensorMultiplexP = new SensorMultiplex(FHSS_VARIO, bmp->varioP());
        addSensor(sensorMultiplexP);
    }
}
