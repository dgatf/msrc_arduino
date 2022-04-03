#include "frsky_d.h"

Frsky::Frsky(AbstractSerial &serial) : serial_(serial)
{
}

Frsky::~Frsky()
{
}

void Frsky::begin()
{
    deviceBufferP = new CircularBuffer<Device>;
    sensorBufferP = new CircularBuffer<SensorFrskyD>;
    serial_.begin(9600, SERIAL__8N1_RXINV_TXINV);
    pinMode(LED_BUILTIN, OUTPUT);
    setConfig();
}

void Frsky::sendByte(uint8_t c, bool header)
{
    if ((c == 0x5D || c == 0x5E) && !header)
    {
        serial_.write(0x5D);
        c ^= 0x60;
    }
    serial_.write(c);
}

void Frsky::sendData(uint8_t dataId, uint16_t value)
{
    digitalWrite(LED_BUILTIN, HIGH);
    uint8_t *u8p;
    // header
    sendByte(0x5E, true);
    // dataId
    sendByte(dataId, false);
    // value
    u8p = (uint8_t *)&value;
    sendByte(u8p[0], false);
    sendByte(u8p[1], false);
    // footer
    sendByte(0x5E, true);

    digitalWrite(LED_BUILTIN, LOW);
}

void Frsky::update()
{
    if (sensorBufferP->current()) // send telemetry
    {
        SensorFrskyD *initialSensorP = sensorBufferP->current(); // loop sensors until correct timestamp or 1 sensors cycle
        while ( ((uint16_t)(millis() - sensorBufferP->current()->timestamp()) <= (uint16_t)sensorBufferP->current()->refresh() * 100) && sensorBufferP->current() != initialSensorP )
        {
            sensorBufferP->next();
        }
        if ( (uint16_t)(millis() - sensorBufferP->current()->timestamp()) >= (uint16_t)sensorBufferP->current()->refresh() * 100 )
        {
            sendData(sensorBufferP->current()->dataId(), sensorBufferP->current()->valueFormatted());
#ifdef DEBUG
            DEBUG_PRINT("D:");
            DEBUG_PRINT_HEX(sensorBufferP->current()->dataId());
            DEBUG_PRINT(" V:");
            DEBUG_PRINT(sensorBufferP->current()->valueFormatted());
            sensorBufferP->current()->valueFormatted(); // toggle type if date/time or lat/lon sensor
            DEBUG_PRINT(" T:");
            DEBUG_PRINT(sensorBufferP->current()->timestamp());
            DEBUG_PRINTLN();
#endif
            sensorBufferP->current()->setTimestamp(millis());
            sensorBufferP->next();
        }
    }
    // update device
    if (deviceBufferP->current())
    {
        deviceBufferP->current()->update();
        deviceBufferP->next();
    }
}

void Frsky::setConfig()
{
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_PWM)
    {
        SensorFrskyD *sensorP;
        EscPWM *esc;
        esc = new EscPWM(ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM));
        esc->begin();
        deviceBufferP->add(esc);
        sensorP = new SensorFrskyD(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM);
        sensorBufferP->add(sensorP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_HW_V3)
    {
        SensorFrskyD *sensorP;
        EscHW3 *esc;
        esc = new EscHW3(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM));
        esc->begin();
        deviceBufferP->add(esc);
        sensorP = new SensorFrskyD(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM);
        sensorBufferP->add(sensorP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_HW_V4)
    {
        SensorFrskyD *sensorP;
        EscHW4 *esc;
        esc = new EscHW4(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP), 0);
        esc->begin();
        deviceBufferP->add(esc);
        PwmOut pwmOut;
        pwmOut.setRpmP(esc->rpmP());
        sensorP = new SensorFrskyD(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VOLTS_BP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VOLTS_AP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(CURRENT_ID, esc->currentP(), CONFIG_REFRESH_CURR);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(TEMP1_ID, esc->tempFetP(), CONFIG_REFRESH_TEMP);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(TEMP2_ID, esc->tempBecP(), CONFIG_REFRESH_TEMP);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VFAS_ID, esc->cellVoltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(FUEL_ID, esc->consumptionP(), CONFIG_REFRESH_CURR);
        sensorBufferP->add(sensorP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_CASTLE)
    {
        SensorFrskyD *sensorP;
        EscCastle *esc;
        esc = new EscCastle(ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        deviceBufferP->add(esc);
        sensorP = new SensorFrskyD(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VOLTS_BP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VOLTS_AP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(CURRENT_ID, esc->currentP(), CONFIG_REFRESH_CURR);
        sensorBufferP->add(sensorP);
        //sensorP = new SensorFrskyD(SBEC_POWER_FIRST_ID, esc->becVoltageP(), esc->becCurrentP(), CONFIG_REFRESH_VOLT);
        //sensorBufferP->add(sensorP);
        //sensorP = new SensorFrskyD(ESC_POWER_FIRST_ID + 1, esc->rippleVoltageP(), NULL, CONFIG_REFRESH_VOLT);
        //sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(TEMP1_ID, esc->temperatureP(), CONFIG_REFRESH_TEMP);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VFAS_ID, esc->cellVoltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(FUEL_ID, esc->consumptionP(), CONFIG_REFRESH_CURR);
        sensorBufferP->add(sensorP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_KONTRONIK)
    {
        SensorFrskyD *sensorP;
        EscKontronik *esc;
        esc = new EscKontronik(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        deviceBufferP->add(esc);
        sensorP = new SensorFrskyD(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VOLTS_BP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VOLTS_AP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(CURRENT_ID, esc->currentP(), CONFIG_REFRESH_CURR);
        sensorBufferP->add(sensorP);
        //sensorP = new SensorFrskyD(SBEC_POWER_FIRST_ID, esc->becVoltageP(), esc->becCurrentP(), CONFIG_REFRESH_VOLT);
        //sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(TEMP1_ID, esc->tempFetP(), CONFIG_REFRESH_TEMP);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(TEMP2_ID, esc->tempBecP(), CONFIG_REFRESH_TEMP);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VFAS_ID, esc->cellVoltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(FUEL_ID, esc->consumptionP(), CONFIG_REFRESH_CURR);
        sensorBufferP->add(sensorP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_APD_F)
    {
        SensorFrskyD *sensorP;
        EscApdF *esc;
        esc = new EscApdF(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        deviceBufferP->add(esc);
        sensorP = new SensorFrskyD(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VOLTS_BP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VOLTS_AP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(CURRENT_ID, esc->currentP(), CONFIG_REFRESH_CURR);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(TEMP1_ID, esc->tempP(), CONFIG_REFRESH_TEMP);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VFAS_ID, esc->cellVoltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_APD_HV)
    {
        SensorFrskyD *sensorP;
        EscApdHV *esc;
        esc = new EscApdHV(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        deviceBufferP->add(esc);
        sensorP = new SensorFrskyD(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VOLTS_BP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VOLTS_AP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(CURRENT_ID, esc->currentP(), CONFIG_REFRESH_CURR);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(TEMP1_ID, esc->tempP(), CONFIG_REFRESH_TEMP);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VFAS_ID, esc->cellVoltageP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
    }
    if (CONFIG_GPS)
    {
        SensorFrskyD *sensorP;
        Bn220 *gps;
        gps = new Bn220(GPS_SERIAL, GPS_BAUD_RATE);
        gps->begin();
        deviceBufferP->add(gps);
        sensorP = new SensorFrskyD(GPS_LONG_BP_ID, gps->lonP(), CONFIG_REFRESH_GPS);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_LONG_AP_ID, gps->lonP(), CONFIG_REFRESH_GPS);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_LONG_EW_ID, gps->lonP(), CONFIG_REFRESH_GPS);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_LAT_BP_ID, gps->latP(), CONFIG_REFRESH_GPS);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_LAT_AP_ID, gps->latP(), CONFIG_REFRESH_GPS);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_LAT_NS_ID, gps->latP(), CONFIG_REFRESH_GPS);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_ALT_BP_ID, gps->altP(), CONFIG_REFRESH_GPS);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_ALT_AP_ID, gps->altP(), CONFIG_REFRESH_GPS);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_SPEED_BP_ID, gps->spdP(), CONFIG_REFRESH_GPS);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_SPEED_AP_ID, gps->spdP(), CONFIG_REFRESH_GPS);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_COURS_BP_ID, gps->cogP(), CONFIG_REFRESH_GPS);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_COURS_AP_ID, gps->cogP(), CONFIG_REFRESH_GPS);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_YEAR_ID, gps->dateP(), 10);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_DAY_MONTH_ID, gps->dateP(), 10);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_HOUR_MIN_ID, gps->timeP(), 10);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_SEC_ID, gps->timeP(), 10);
        sensorBufferP->add(sensorP);
        //sensorP = new SensorFrskyD(VARIO_ID, gps->varioP(), 5);
        //sensorBufferP->add(sensorP);
    }
    if (CONFIG_AIRSPEED)
    {
        SensorFrskyD *sensorP;
        Pressure *pressure;
        pressure = new Pressure(PIN_PRESSURE, ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT));
        deviceBufferP->add(pressure);
        sensorP = new SensorFrskyD(GPS_SPEED_BP_ID, pressure->valueP(), 5);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(GPS_SPEED_AP_ID, pressure->valueP(), 5);
        sensorBufferP->add(sensorP);
    }
    if (CONFIG_VOLTAGE1)
    {
        SensorFrskyD *sensorP;
        Voltage *voltage;
        voltage = new Voltage(PIN_VOLTAGE1, ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), VOLTAGE1_MULTIPLIER);
        deviceBufferP->add(voltage);
        sensorP = new SensorFrskyD(VOLTS_BP_ID, voltage->valueP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VOLTS_AP_ID, voltage->valueP(), CONFIG_REFRESH_VOLT);
        sensorBufferP->add(sensorP);
    }
    /*if (CONFIG_VOLTAGE2)
    {
        SensorFrskyD *sensorP;
        Voltage *voltage;
        voltage = new Voltage(PIN_VOLTAGE2, ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), VOLTAGE2_MULTIPLIER);
        deviceBufferP->add(voltage);
        sensorP = new SensorFrskyD(A4_FIRST_ID, voltage->valueP(), CONFIG_REFRESH_VOLT, voltage);
        sensorBufferP->add(sensorP);
    }*/
    if (CONFIG_CURRENT)
    {
        SensorFrskyD *sensorP;
        Current *current;
        current = new Current(PIN_CURRENT, ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), CURRENT_MULTIPLIER, CURRENT_OFFSET, CURRENT_AUTO_OFFSET);
        deviceBufferP->add(current);
        sensorP = new SensorFrskyD(CURRENT_ID, current->valueP(), CONFIG_REFRESH_CURR);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(FUEL_ID, current->consumptionP(), CONFIG_REFRESH_CURR);
        sensorBufferP->add(sensorP);
    }
    if (CONFIG_NTC1)
    {
        SensorFrskyD *sensorP;
        Ntc *ntc;
        ntc = new Ntc(PIN_NTC1, ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        deviceBufferP->add(ntc);
        sensorP = new SensorFrskyD(TEMP1_ID, ntc->valueP(), CONFIG_AVERAGING_ELEMENTS_TEMP);
        sensorBufferP->add(sensorP);
    }
    if (CONFIG_NTC2)
    {
        SensorFrskyD *sensorP;
        Ntc *ntc;
        ntc = new Ntc(PIN_NTC2, ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        deviceBufferP->add(ntc);
        sensorP = new SensorFrskyD(TEMP2_ID, ntc->valueP(), CONFIG_AVERAGING_ELEMENTS_TEMP);
        sensorBufferP->add(sensorP);
    }
    if (CONFIG_I2C1_TYPE == I2C_BMP280)
    {
        SensorFrskyD *sensorP;
        Bmp280 *bmp;
        bmp = new Bmp280(CONFIG_I2C1_ADDRESS, ALPHA(CONFIG_AVERAGING_ELEMENTS_VARIO));
        bmp->begin();
        deviceBufferP->add(bmp);
        sensorP = new SensorFrskyD(TEMP1_ID, bmp->temperatureP(), CONFIG_AVERAGING_ELEMENTS_TEMP);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(BARO_ALT_BP_ID, bmp->altitudeP(), CONFIG_REFRESH_DEF);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(BARO_ALT_AP_ID, bmp->altitudeP(), CONFIG_REFRESH_DEF);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VARIO_ID, bmp->varioP(), 5);
        sensorBufferP->add(sensorP);
    }
    if (CONFIG_I2C1_TYPE == I2C_MS5611)
    {
        SensorFrskyD *sensorP;
        MS5611 *bmp;
        bmp = new MS5611(CONFIG_I2C1_ADDRESS, ALPHA(CONFIG_AVERAGING_ELEMENTS_VARIO));
        bmp->begin();
        deviceBufferP->add(bmp);
        sensorP = new SensorFrskyD(TEMP1_ID, bmp->temperatureP(), CONFIG_AVERAGING_ELEMENTS_TEMP);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(BARO_ALT_BP_ID, bmp->altitudeP(), CONFIG_REFRESH_DEF);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(BARO_ALT_AP_ID, bmp->altitudeP(), CONFIG_REFRESH_DEF);
        sensorBufferP->add(sensorP);
        sensorP = new SensorFrskyD(VARIO_ID, bmp->varioP(), 5);
        sensorBufferP->add(sensorP);
    }
}