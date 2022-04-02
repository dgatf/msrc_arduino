#include "frsky_d.h"

Frsky::Frsky(AbstractSerial &serial) : serial_(serial)
{
}

Frsky::~Frsky()
{
}

void Frsky::begin()
{
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

void Frsky::addSensor(Sensord *newSensorP)
{
    static Sensord *prevSensorP;
    if (sensorP == NULL)
    {
        prevSensorP = newSensorP;
        newSensorP->nextP = newSensorP;
    }
    sensorP = newSensorP;
    sensorP->nextP = prevSensorP->nextP;
    prevSensorP->nextP = newSensorP;
    prevSensorP = newSensorP;
}

void Frsky::update()
{
    if (sensorP != NULL) // send telemetry
    {
        static Sensord *spSensorP = sensorP; // loop sensors until correct timestamp or 1 sensors cycle
        Sensord *initialSensorP = spSensorP;
        while ( ((uint16_t)(millis() - spSensorP->timestamp()) <= (uint16_t)spSensorP->refresh() * 100) && spSensorP->nextP != initialSensorP )
        {
            spSensorP = spSensorP->nextP;
        }
        if ( (uint16_t)(millis() - spSensorP->timestamp()) >= (uint16_t)spSensorP->refresh() * 100 )
        {
            sendData(spSensorP->dataId(), spSensorP->valueFormatted());
#ifdef DEBUG
            DEBUG_PRINT("D:");
            DEBUG_PRINT_HEX(spSensorP->dataId());
            DEBUG_PRINT(" V:");
            DEBUG_PRINT(spSensorP->valueFormatted());
            spSensorP->valueFormatted(); // toggle type if date/time or lat/lon sensor
            DEBUG_PRINT(" T:");
            DEBUG_PRINT(spSensorP->timestamp());
            DEBUG_PRINTLN();
#endif
            spSensorP->setTimestamp(millis());
            spSensorP = spSensorP->nextP;
        }
    }
    // update sensor
    if (sensorP != NULL)
    {
        sensorP->update();
        sensorP = sensorP->nextP;
    }
}

void Frsky::setConfig()
{
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_PWM)
    {
        Sensord *sensorP;
        EscPWM *esc;
        esc = new EscPWM(ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM));
        esc->begin();
        sensorP = new Sensord(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM, esc);
        addSensor(sensorP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_HW_V3)
    {
        Sensord *sensorP;
        EscHW3 *esc;
        esc = new EscHW3(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM));
        esc->begin();
        sensorP = new Sensord(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM, esc);
        addSensor(sensorP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_HW_V4)
    {
        Sensord *sensorP;
        EscHW4 *esc;
        esc = new EscHW4(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP), 0);
        esc->begin();
        PwmOut pwmOut;
        pwmOut.setRpmP(esc->rpmP());
        sensorP = new Sensord(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VOLTS_BP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VOLTS_AP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
        sensorP = new Sensord(CURRENT_ID, esc->currentP(), CONFIG_REFRESH_CURR, esc);
        addSensor(sensorP);
        sensorP = new Sensord(TEMP1_ID, esc->tempFetP(), CONFIG_REFRESH_TEMP, esc);
        addSensor(sensorP);
        sensorP = new Sensord(TEMP2_ID, esc->tempBecP(), CONFIG_REFRESH_TEMP, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VFAS_ID, esc->cellVoltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
        sensorP = new Sensord(FUEL_ID, esc->consumptionP(), CONFIG_REFRESH_CURR, esc);
        addSensor(sensorP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_CASTLE)
    {
        Sensord *sensorP;
        EscCastle *esc;
        esc = new EscCastle(ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        sensorP = new Sensord(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VOLTS_BP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VOLTS_AP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
        sensorP = new Sensord(CURRENT_ID, esc->currentP(), CONFIG_REFRESH_CURR, esc);
        addSensor(sensorP);
        //sensorP = new Sensord(SBEC_POWER_FIRST_ID, esc->becVoltageP(), esc->becCurrentP(), CONFIG_REFRESH_VOLT, esc);
        //addSensor(sensorP);
        //sensorP = new Sensord(ESC_POWER_FIRST_ID + 1, esc->rippleVoltageP(), NULL, CONFIG_REFRESH_VOLT, esc);
        //addSensor(sensorP);
        sensorP = new Sensord(TEMP1_ID, esc->temperatureP(), CONFIG_REFRESH_TEMP, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VFAS_ID, esc->cellVoltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
        sensorP = new Sensord(FUEL_ID, esc->consumptionP(), CONFIG_REFRESH_CURR, esc);
        addSensor(sensorP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_KONTRONIK)
    {
        Sensord *sensorP;
        EscKontronik *esc;
        esc = new EscKontronik(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        sensorP = new Sensord(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VOLTS_BP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VOLTS_AP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
        sensorP = new Sensord(CURRENT_ID, esc->currentP(), CONFIG_REFRESH_CURR, esc);
        addSensor(sensorP);
        //sensorP = new Sensord(SBEC_POWER_FIRST_ID, esc->becVoltageP(), esc->becCurrentP(), CONFIG_REFRESH_VOLT, esc);
        //addSensor(sensorP);
        sensorP = new Sensord(TEMP1_ID, esc->tempFetP(), CONFIG_REFRESH_TEMP, esc);
        addSensor(sensorP);
        sensorP = new Sensord(TEMP2_ID, esc->tempBecP(), CONFIG_REFRESH_TEMP, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VFAS_ID, esc->cellVoltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
        sensorP = new Sensord(FUEL_ID, esc->consumptionP(), CONFIG_REFRESH_CURR, esc);
        addSensor(sensorP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_APD_F)
    {
        Sensord *sensorP;
        EscApdF *esc;
        esc = new EscApdF(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        sensorP = new Sensord(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VOLTS_BP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VOLTS_AP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
        sensorP = new Sensord(CURRENT_ID, esc->currentP(), CONFIG_REFRESH_CURR, esc);
        addSensor(sensorP);
        sensorP = new Sensord(TEMP1_ID, esc->tempP(), CONFIG_REFRESH_TEMP, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VFAS_ID, esc->cellVoltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_APD_HV)
    {
        Sensord *sensorP;
        EscApdHV *esc;
        esc = new EscApdHV(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        sensorP = new Sensord(RPM_ID, esc->rpmP(), CONFIG_REFRESH_RPM, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VOLTS_BP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VOLTS_AP_ID, esc->voltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
        sensorP = new Sensord(CURRENT_ID, esc->currentP(), CONFIG_REFRESH_CURR, esc);
        addSensor(sensorP);
        sensorP = new Sensord(TEMP1_ID, esc->tempP(), CONFIG_REFRESH_TEMP, esc);
        addSensor(sensorP);
        sensorP = new Sensord(VFAS_ID, esc->cellVoltageP(), CONFIG_REFRESH_VOLT, esc);
        addSensor(sensorP);
    }
    if (CONFIG_GPS)
    {
        Sensord *sensorP;
        Bn220 *gps;
        gps = new Bn220(GPS_SERIAL, GPS_BAUD_RATE);
        gps->begin();
        sensorP = new Sensord(GPS_LONG_BP_ID, gps->lonP(), CONFIG_REFRESH_GPS, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_LONG_AP_ID, gps->lonP(), CONFIG_REFRESH_GPS, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_LONG_EW_ID, gps->lonP(), CONFIG_REFRESH_GPS, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_LAT_BP_ID, gps->latP(), CONFIG_REFRESH_GPS, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_LAT_AP_ID, gps->latP(), CONFIG_REFRESH_GPS, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_LAT_NS_ID, gps->latP(), CONFIG_REFRESH_GPS, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_ALT_BP_ID, gps->altP(), CONFIG_REFRESH_GPS, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_ALT_AP_ID, gps->altP(), CONFIG_REFRESH_GPS, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_SPEED_BP_ID, gps->spdP(), CONFIG_REFRESH_GPS, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_SPEED_AP_ID, gps->spdP(), CONFIG_REFRESH_GPS, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_COURS_BP_ID, gps->cogP(), CONFIG_REFRESH_GPS, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_COURS_AP_ID, gps->cogP(), CONFIG_REFRESH_GPS, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_YEAR_ID, gps->dateP(), 10, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_DAY_MONTH_ID, gps->dateP(), 10, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_HOUR_MIN_ID, gps->timeP(), 10, gps);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_SEC_ID, gps->timeP(), 10, gps);
        addSensor(sensorP);
        //sensorP = new Sensord(VARIO_ID, gps->varioP(), 5, gps);
        //addSensor(sensorP);
    }
    if (CONFIG_AIRSPEED)
    {
        Sensord *sensorP;
        Pressure *pressure;
        pressure = new Pressure(PIN_PRESSURE, ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT));
        sensorP = new Sensord(GPS_SPEED_BP_ID, pressure->valueP(), 5, pressure);
        addSensor(sensorP);
        sensorP = new Sensord(GPS_SPEED_AP_ID, pressure->valueP(), 5, pressure);
        addSensor(sensorP);
    }
    if (CONFIG_VOLTAGE1)
    {
        Sensord *sensorP;
        Voltage *voltage;
        voltage = new Voltage(PIN_VOLTAGE1, ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), VOLTAGE1_MULTIPLIER);
        sensorP = new Sensord(VOLTS_BP_ID, voltage->valueP(), CONFIG_REFRESH_VOLT, voltage);
        addSensor(sensorP);
        sensorP = new Sensord(VOLTS_AP_ID, voltage->valueP(), CONFIG_REFRESH_VOLT, voltage);
        addSensor(sensorP);
    }
    /*if (CONFIG_VOLTAGE2)
    {
        Sensord *sensorP;
        Voltage *voltage;
        voltage = new Voltage(PIN_VOLTAGE2, ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), VOLTAGE2_MULTIPLIER);
        sensorP = new Sensord(A4_FIRST_ID, voltage->valueP(), CONFIG_REFRESH_VOLT, voltage);
        addSensor(sensorP);
    }*/
    if (CONFIG_CURRENT)
    {
        Sensord *sensorP;
        Current *current;
        current = new Current(PIN_CURRENT, ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), CURRENT_MULTIPLIER, CURRENT_OFFSET, CURRENT_AUTO_OFFSET);
        sensorP = new Sensord(CURRENT_ID, current->valueP(), CONFIG_REFRESH_CURR, current);
        addSensor(sensorP);
        sensorP = new Sensord(FUEL_ID, current->consumptionP(), CONFIG_REFRESH_CURR, current);
        addSensor(sensorP);
    }
    if (CONFIG_NTC1)
    {
        Sensord *sensorP;
        Ntc *ntc;
        ntc = new Ntc(PIN_NTC1, ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        sensorP = new Sensord(TEMP1_ID, ntc->valueP(), CONFIG_AVERAGING_ELEMENTS_TEMP, ntc);
        addSensor(sensorP);
    }
    if (CONFIG_NTC2)
    {
        Sensord *sensorP;
        Ntc *ntc;
        ntc = new Ntc(PIN_NTC2, ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        sensorP = new Sensord(TEMP2_ID, ntc->valueP(), CONFIG_AVERAGING_ELEMENTS_TEMP, ntc);
        addSensor(sensorP);
    }
    if (CONFIG_I2C1_TYPE == I2C_BMP280)
    {
        Sensord *sensorP;
        Bmp280 *bmp;
        bmp = new Bmp280(CONFIG_I2C1_ADDRESS, ALPHA(CONFIG_AVERAGING_ELEMENTS_VARIO));
        bmp->begin();
        sensorP = new Sensord(TEMP1_ID, bmp->temperatureP(), CONFIG_AVERAGING_ELEMENTS_TEMP, bmp);
        addSensor(sensorP);
        sensorP = new Sensord(BARO_ALT_BP_ID, bmp->altitudeP(), CONFIG_REFRESH_DEF, bmp);
        addSensor(sensorP);
        sensorP = new Sensord(BARO_ALT_AP_ID, bmp->altitudeP(), CONFIG_REFRESH_DEF, bmp);
        addSensor(sensorP);
        sensorP = new Sensord(VARIO_ID, bmp->varioP(), 5, bmp);
        addSensor(sensorP);
    }
    if (CONFIG_I2C1_TYPE == I2C_MS5611)
    {
        Sensord *sensorP;
        MS5611 *bmp;
        bmp = new MS5611(CONFIG_I2C1_ADDRESS, ALPHA(CONFIG_AVERAGING_ELEMENTS_VARIO));
        bmp->begin();
        sensorP = new Sensord(TEMP1_ID, bmp->temperatureP(), CONFIG_AVERAGING_ELEMENTS_TEMP, bmp);
        addSensor(sensorP);
        sensorP = new Sensord(BARO_ALT_BP_ID, bmp->altitudeP(), CONFIG_REFRESH_DEF, bmp);
        addSensor(sensorP);
        sensorP = new Sensord(BARO_ALT_AP_ID, bmp->altitudeP(), CONFIG_REFRESH_DEF, bmp);
        addSensor(sensorP);
        sensorP = new Sensord(VARIO_ID, bmp->varioP(), 5, bmp);
        addSensor(sensorP);
    }
}