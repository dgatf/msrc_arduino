#include "jetiex.h"

JetiEx::JetiEx(AbstractSerial &serial) : serial_(serial)
{
}

JetiEx::~JetiEx()
{
}

void JetiEx::begin()
{
    deviceBufferP = new CircularBuffer<Device>;
    serial_.begin(baudRate, SERIAL__8N1 | SERIAL__HALF_DUP);
    serial_.setTimeout(JETIEX_TIMEOUT);
    pinMode(LED_BUILTIN, OUTPUT);
    setConfig();
}

uint8_t JetiEx::addSensor(SensorJetiEx *newSensorJetiExP)
{
    static uint8_t number = 1;
    sensorJetiExP[number] = newSensorJetiExP;
    if (number < 16)
        number++;
    return number - 1;
}

bool JetiEx::addSensorValueToBuffer(uint8_t *buffer, uint8_t &posBuffer, uint8_t &sensorNumber)
{
    if (sensorJetiExP[sensorNumber])
    {
        float valueF = *sensorJetiExP[sensorNumber]->valueP();
        uint8_t format = sensorJetiExP[sensorNumber]->format() << 5;
        if (sensorJetiExP[sensorNumber]->type() == JETIEX_TYPE_INT6)
        {
            if (posBuffer > 25) // 29 bytes max:  25+2=pos27=byte28 +1crc=byte29
                return false;
            else
            {
                int8_t value = valueF * pow(10, sensorJetiExP[sensorNumber]->format());
                if (value > 0x1F)
                    value = 0x1F;
                else if (value < -0x1F)
                    value = -0x1F;
                value &= ~(3 << 5);
                value |= format;
                buffer[posBuffer] = sensorNumber << 4 | sensorJetiExP[sensorNumber]->type();
                buffer[posBuffer + 1] = value;
                posBuffer += 2;
            }
        }
        else if (sensorJetiExP[sensorNumber]->type() == JETIEX_TYPE_INT14)
        {
            if (posBuffer > 24)
                return false;
            else
            {
                int16_t value = valueF * pow(10, sensorJetiExP[sensorNumber]->format());
                if (value > 0x1FFF)
                    value = 0x1FFF;
                if (value < -0x1FFF)
                    value = -0x1FFF;
                value &= ~((uint16_t)3 << (5 + 8));
                value |= (uint16_t)format << 8;
                buffer[posBuffer] = sensorNumber << 4 | sensorJetiExP[sensorNumber]->type();
                buffer[posBuffer + 1] = value;
                buffer[posBuffer + 2] = value >> 8;
                posBuffer += 3;
            }
        }
        else if (sensorJetiExP[sensorNumber]->type() == JETIEX_TYPE_INT22)
        {
            if (posBuffer > 23)
                return false;
            else
            {
                int32_t value = valueF * pow(10, sensorJetiExP[sensorNumber]->format());
                if (value > 0x1FFFFF)
                    value = 0x1FFFFF;
                else if (value < -0x1FFFFF)
                    value = -0x1FFFFF;
                value &= ~((uint32_t)3 << (5 + 16));
                value |= (uint32_t)format << 16;
                buffer[posBuffer] = sensorNumber << 4 | sensorJetiExP[sensorNumber]->type();
                buffer[posBuffer + 1] = value;
                buffer[posBuffer + 2] = value >> 8;
                buffer[posBuffer + 3] = value >> 16;
                posBuffer += 4;
            }
        }
        else if (sensorJetiExP[sensorNumber]->type() == JETIEX_TYPE_INT30)
        {
            if (posBuffer > 22)
                return false;
            else
            {
                int32_t value = valueF * pow(10, sensorJetiExP[sensorNumber]->format());
                if (value > 0x1FFFFFFF)
                    value = 0x1FFFFFFF;
                else if (value < -0x1FFFFFFF)
                    value = -0x1FFFFFFF;
                value &= ~((uint32_t)3 << (5 + 24));
                value |= (uint32_t)format << 24;
                buffer[posBuffer] = sensorNumber << 4 | sensorJetiExP[sensorNumber]->type();
                buffer[posBuffer + 1] = value;
                buffer[posBuffer + 2] = value >> 8;
                buffer[posBuffer + 3] = value >> 16;
                buffer[posBuffer + 4] = value >> 24;
                posBuffer += 5;
            }
        }
        else if (sensorJetiExP[sensorNumber]->type() == JETIEX_TYPE_TIMEDATE)
        {
            if (posBuffer > 23)
                return false;
            else
            {
                // rawvalue: yymmdd/hhmmss
                // byte 1: day/second
                // byte 2: month/minute
                // byte 3(bits 1-5): year/hour
                // byte 3(bit 6): 0=time 1=date
                uint32_t value = valueF;
                uint8_t hourYearFormat = format;
                hourYearFormat |= value / 10000;                             // hour, year
                uint8_t minuteMonth = (value / 100 - (value / 10000) * 100); // minute, month
                uint8_t secondDay = value - (value / 100) * 100;             // second, day
                buffer[posBuffer] = sensorNumber << 4 | sensorJetiExP[sensorNumber]->type();
                buffer[posBuffer + 1] = secondDay;
                buffer[posBuffer + 2] = minuteMonth;
                buffer[posBuffer + 3] = hourYearFormat;
                posBuffer += 4;
            }
        }
        else if (sensorJetiExP[sensorNumber]->type() == JETIEX_TYPE_COORDINATES)
        {
            if (posBuffer > 22)
                return false;
            else
            {
                // rawvalue: minutes
                // byte 1-2: degrees (decimals)
                // byte 3: degrees (integer)
                // byte 4(bit 6): 0=lat 1=lon
                // byte 4(bit 7): 0=+(N,E), 1=-(S,W)
                if (valueF < 0)
                    format |= 1 << 6;
                buffer[posBuffer] = sensorNumber << 4 | sensorJetiExP[sensorNumber]->type();
                uint8_t degrees = valueF / 60;
                uint16_t degreesDecimals = (valueF / 60 - degrees) * 10000;
                buffer[posBuffer + 1] = degreesDecimals;      // degrees (dec, l)
                buffer[posBuffer + 2] = degreesDecimals >> 8; // degrees (dec, h)
                buffer[posBuffer + 3] = degrees;              // degrees (int)
                buffer[posBuffer + 4] = format;               // format
                posBuffer += 5;
            }
        }
    }
    else
        return false;
    sensorNumber++;
    return true;
}

bool JetiEx::addSensorTextToBuffer(uint8_t *buffer, uint8_t &posBuffer, uint8_t &sensorNumber)
{
    if (sensorJetiExP[sensorNumber])
    {
        uint8_t lenText = strlen(sensorJetiExP[sensorNumber]->textP());
        uint8_t lenUnit = strlen(sensorJetiExP[sensorNumber]->unitP());

        if (posBuffer + lenText + lenUnit + 2 < 28)
        {
            buffer[posBuffer] = sensorNumber;
            buffer[posBuffer + 1] = lenText << 3 | lenUnit;
            posBuffer += 2;
            strcpy((char *)buffer + posBuffer, sensorJetiExP[sensorNumber]->textP());
            posBuffer += lenText;
            strcpy((char *)buffer + posBuffer, sensorJetiExP[sensorNumber]->unitP());
            posBuffer += lenUnit;
            sensorNumber++;
            return true;
        }
    }
    return false;
}

uint8_t JetiEx::createExBuffer(uint8_t *buffer, bool sendValue)
{
    static uint8_t sensorNumberTelemetry = 1;
    static uint8_t sensorNumberText = 1;
    uint8_t posBuffer = 7;
    if (sensorJetiExP[1] == NULL)
        return 0;
    if (sendValue)
    {
        while (addSensorValueToBuffer(buffer, posBuffer, sensorNumberTelemetry) && sensorJetiExP[sensorNumberTelemetry] != NULL)
            ;
        if (sensorJetiExP[sensorNumberTelemetry] == NULL)
            sensorNumberTelemetry = 1;
        buffer[1] = 0x40;
    }
    else
    {
        /*while*/ (addSensorTextToBuffer(buffer, posBuffer, sensorNumberText) && sensorJetiExP[sensorNumberText] != NULL);
        if (sensorJetiExP[sensorNumberText] == NULL)
            sensorNumberText = 1;
    }
    buffer[0] = 0x0F;
    buffer[1] |= posBuffer - 1;
    buffer[2] = JETIEX_MFG_ID_LOW;
    buffer[3] = JETIEX_MFG_ID_HIGH;
    buffer[4] = JETIEX_DEV_ID_LOW;
    buffer[5] = JETIEX_DEV_ID_HIGH;
    buffer[6] = 0x00;
    buffer[posBuffer] = crc8(buffer + 1, posBuffer - 1);
    return posBuffer + 1;
}

void JetiEx::sendPacket(uint8_t packetId)
{
    digitalWrite(LED_BUILTIN, HIGH);
    static uint8_t packetCount = 0;
    uint8_t buffer[36] = {0};
    uint8_t lengthExBuffer = createExBuffer(buffer + 6, packetCount % 16);
    buffer[0] = 0x3B;
    buffer[1] = 0x01;
    buffer[2] = lengthExBuffer + 8;
    buffer[3] = packetId;
    buffer[4] = 0x3A;
    buffer[5] = lengthExBuffer;
    uint16_t crc = crc16(buffer, lengthExBuffer + 6);
    buffer[lengthExBuffer + 6] = crc;
    buffer[lengthExBuffer + 7] = crc >> 8;
    serial_.writeBytes(buffer, lengthExBuffer + 8);
#ifdef DEBUG
    if (packetCount % 16)
        DEBUG_PRINT("V"); // values
    else
        DEBUG_PRINT("T"); // text
    DEBUG_PRINT(">");
    for (uint8_t i = 0; i < lengthExBuffer + 8; i++)
    {
        DEBUG_PRINT_HEX(buffer[i]);
        DEBUG_PRINT(" ");
        delayMicroseconds(100);
    }
    DEBUG_PRINTLN();
#endif
    packetCount++;
    digitalWrite(LED_BUILTIN, LOW);
}

void JetiEx::update()
{
    uint8_t status = JETIEX_WAIT;
    static bool mute = true;
#if defined(SIM_RX)
    static uint16_t ts = 0;
    static uint8_t packetId = 0;
    if ((uint16_t)(millis() - ts) > 100)
    {
        if (!mute)
        {
            status = JETIEX_SEND;
            packetId++;
        }
        mute = !mute;
        ts = millis();
    }
#else
    uint8_t packetId;
    uint8_t length = serial_.availableTimeout();
    static uint16_t ts = 0;
    if (length)
    {
        uint8_t buff[length];
        serial_.readBytes(buff, length);
#ifdef DEBUG_PACKET2
        DEBUG_PRINT("<");
        for (uint8_t i = 0; i < length; i++)
        {
            DEBUG_PRINT_HEX(buff[i]);
            DEBUG_PRINT(" ");
            delayMicroseconds(100);
        }
        DEBUG_PRINTLN();
#endif
        uint8_t packet[JETIEX_PACKET_LENGHT];
        if (buff[0] == 0x3E && buff[1] == 0x3 && length - buff[2] == JETIEX_PACKET_LENGHT)
        {
            memcpy(packet, buff + buff[2], JETIEX_PACKET_LENGHT);
#ifdef DEBUG_PACKET
            DEBUG_PRINT("P:");
            for (uint8_t i = 0; i < JETIEX_PACKET_LENGHT; i++)
            {
                DEBUG_PRINT_HEX(packet[i]);
                DEBUG_PRINT(" ");
            }
            DEBUG_PRINTLN();
#endif
        }
        else if (length == JETIEX_PACKET_LENGHT)
        {
            memcpy(packet, buff, JETIEX_PACKET_LENGHT);
        }
        else
        {
            return;
        }
        if (crc16(packet, JETIEX_PACKET_LENGHT) == 0)
        {
            ts = millis();
            if (packet[0] == 0x3D && packet[1] == 0x01 && packet[4] == 0x3A)
            {
                if (!mute)
                {
                    status = JETIEX_SEND;
                    packetId = packet[3];
                }
                mute = !mute;
            }
        }
    }
#endif
    if (status == JETIEX_SEND)
    {
#ifdef SIM_RX
        if (1)
#else
        if (serial_.timestamp() < 1500)
#endif
            sendPacket(packetId);
#ifdef DEBUG
        else
        {
            DEBUG_PRINT("KO");
            DEBUG_PRINTLN();
        }
#endif
    }
    if ((uint16_t)(millis() - ts) > 5000)
    {
        if (baudRate == 125000L)
            baudRate = 250000L;
        else
            baudRate = 125000L;
        serial_.begin(baudRate, SERIAL__8N1 | SERIAL__HALF_DUP);
        ts = millis();
#ifdef DEBUG
        DEBUG_PRINT("BR:");
        DEBUG_PRINT(baudRate);
        DEBUG_PRINTLN();
#endif
    }

    // update device
    if (deviceBufferP->current())
    {
        deviceBufferP->current()->update();
        deviceBufferP->next();
    }
}

void JetiEx::setConfig()
{
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_PWM)
    {
        SensorJetiEx *sensorJetiExP;
        EscPWM *esc;
        esc = new EscPWM(ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM));
        esc->begin();
        deviceBufferP->add(esc);
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT22, 0, esc->rpmP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("RPM");
        sensorJetiExP->setUnit("RPM");
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_HW_V3)
    {
        SensorJetiEx *sensorJetiExP;
        EscHW3 *esc;
        esc = new EscHW3(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM));
        esc->begin();
        deviceBufferP->add(esc);
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT22, 0, esc->rpmP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("RPM");
        sensorJetiExP->setUnit("RPM");
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_HW_V4)
    {
        SensorJetiEx *sensorJetiExP;
        EscHW4 *esc;
        esc = new EscHW4(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP), 0);
        esc->begin();
        deviceBufferP->add(esc);
        PwmOut pwmOut;
        pwmOut.setRpmP(esc->rpmP());
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT22, 0, esc->rpmP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("RPM");
        sensorJetiExP->setUnit("RPM");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, esc->currentP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Current");
        sensorJetiExP->setUnit("A");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->voltageP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Voltage");
        sensorJetiExP->setUnit("V");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, esc->tempFetP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Temp FET");
        sensorJetiExP->setUnit("C");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, esc->tempBecP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Temp BEC");
        sensorJetiExP->setUnit("C");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->cellVoltageP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Cell Voltage");
        sensorJetiExP->setUnit("V");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, esc->consumptionP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Consumption");
        sensorJetiExP->setUnit("mAh");
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_CASTLE)
    {
        SensorJetiEx *sensorJetiExP;
        EscCastle *esc;
        esc = new EscCastle(ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        deviceBufferP->add(esc);
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT22, 0, esc->rpmP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("RPM");
        sensorJetiExP->setUnit("RPM");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, esc->currentP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Current");
        sensorJetiExP->setUnit("A");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->voltageP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Voltage");
        sensorJetiExP->setUnit("V");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->rippleVoltageP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Ripple Voltage");
        sensorJetiExP->setUnit("V");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->becCurrentP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("BEC Current");
        sensorJetiExP->setUnit("A");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->becVoltageP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("BEC Voltage");
        sensorJetiExP->setUnit("V");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, esc->temperatureP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Temperature");
        sensorJetiExP->setUnit("C");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->cellVoltageP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Cell Voltage");
        sensorJetiExP->setUnit("V");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, esc->consumptionP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Consumption");
        sensorJetiExP->setUnit("mAh");
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_KONTRONIK)
    {
        SensorJetiEx *sensorJetiExP;
        EscKontronik *esc;
        esc = new EscKontronik(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        deviceBufferP->add(esc);
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT22, 0, esc->rpmP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("RPM");
        sensorJetiExP->setUnit("RPM");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, esc->currentP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Current");
        sensorJetiExP->setUnit("A");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->voltageP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Voltage");
        sensorJetiExP->setUnit("V");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, esc->becCurrentP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("BEC Current");
        sensorJetiExP->setUnit("A");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->becVoltageP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("BEC Voltage");
        sensorJetiExP->setUnit("V");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, esc->tempFetP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Temp FET");
        sensorJetiExP->setUnit("C");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, esc->tempBecP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Temp BEC");
        sensorJetiExP->setUnit("C");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->cellVoltageP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Cell Voltage");
        sensorJetiExP->setUnit("V");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, esc->consumptionP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Consumption");
        sensorJetiExP->setUnit("mAh");
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_APD_F)
    {
        SensorJetiEx *sensorJetiExP;
        EscApdF *esc;
        esc = new EscApdF(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        deviceBufferP->add(esc);
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT22, 0, esc->rpmP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("RPM");
        sensorJetiExP->setUnit("RPM");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, esc->currentP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Current");
        sensorJetiExP->setUnit("A");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->voltageP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Voltage");
        sensorJetiExP->setUnit("V");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, esc->tempP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Temperature");
        sensorJetiExP->setUnit("C");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, esc->consumptionP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Consumption");
        sensorJetiExP->setUnit("mAh");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->cellVoltageP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Cell Voltage");
        sensorJetiExP->setUnit("V");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, esc->consumptionP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Consumption");
        sensorJetiExP->setUnit("mAh");
    }
    if (CONFIG_ESC_PROTOCOL == PROTOCOL_APD_HV)
    {
        SensorJetiEx *sensorJetiExP;
        EscApdHV *esc;
        esc = new EscApdHV(ESC_SERIAL, ALPHA(CONFIG_AVERAGING_ELEMENTS_RPM), ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        esc->begin();
        deviceBufferP->add(esc);
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT22, 0, esc->rpmP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("RPM");
        sensorJetiExP->setUnit("RPM");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, esc->currentP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Current");
        sensorJetiExP->setUnit("A");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->voltageP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Voltage");
        sensorJetiExP->setUnit("V");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, esc->tempP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Temperature");
        sensorJetiExP->setUnit("C");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, esc->cellVoltageP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Cell Voltage");
        sensorJetiExP->setUnit("V");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, esc->consumptionP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Consumption");
        sensorJetiExP->setUnit("mAh");
    }
    if (CONFIG_GPS)
    {
        SensorJetiEx *sensorJetiExP;
        Bn220 *gps;
        gps = new Bn220(GPS_SERIAL, GPS_BAUD_RATE);
        gps->begin();
        deviceBufferP->add(gps);
#ifdef JETI_GPS_SPEED_UNITS_KMH
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, gps->spdKmhP());
        sensorJetiExP->setUnit("km/h");
#else
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, gps->spdP());
        sensorJetiExP->setUnit("kts");
#endif
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Speed");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT22, 1, gps->altP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Altitude");
        sensorJetiExP->setUnit("m");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_COORDINATES, JETIEX_FORMAT_LAT, gps->latP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Latitude");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_COORDINATES, JETIEX_FORMAT_LON, gps->lonP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Longitude");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT6, JETIEX_FORMAT_0_DECIMAL, gps->satP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Sats");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, gps->varioP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Vario");
        sensorJetiExP->setUnit("m/s");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, gps->distP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Dist Home");
        sensorJetiExP->setUnit("m");
#ifndef JETIEX_LOW_MEMORY
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_TIMEDATE, JETIEX_FORMAT_TIME, gps->timeP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Time");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_TIMEDATE, JETIEX_FORMAT_DATE, gps->dateP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Date");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, JETIEX_FORMAT_2_DECIMAL, gps->hdopP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("HDOP");
#endif
    }
    if (CONFIG_AIRSPEED)
    {
        SensorJetiEx *sensorJetiExP;
        Pressure *pressure;
        pressure = new Pressure(PIN_PRESSURE, ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT));
        deviceBufferP->add(pressure);
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, pressure->valueP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Air speed");
        sensorJetiExP->setUnit("m/s");
    }
    if (CONFIG_VOLTAGE1)
    {
        SensorJetiEx *sensorJetiExP;
        Voltage *voltage;
        voltage = new Voltage(PIN_VOLTAGE1, ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), VOLTAGE1_MULTIPLIER);
        deviceBufferP->add(voltage);
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, voltage->valueP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Voltage 1");
        sensorJetiExP->setUnit("V");
    }
    if (CONFIG_VOLTAGE2)
    {
        SensorJetiEx *sensorJetiExP;
        Voltage *voltage;
        voltage = new Voltage(PIN_VOLTAGE2, ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT), VOLTAGE2_MULTIPLIER);
        deviceBufferP->add(voltage);
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 2, voltage->valueP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Voltage 2");
        sensorJetiExP->setUnit("V");
    }
    if (CONFIG_CURRENT)
    {
        SensorJetiEx *sensorJetiExP;
        Current *current;
        current = new Current(PIN_CURRENT, ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR), CURRENT_MULTIPLIER, CURRENT_OFFSET, CURRENT_AUTO_OFFSET);
        deviceBufferP->add(current);
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, current->valueP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Current");
        sensorJetiExP->setUnit("A");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, current->consumptionP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Consumption");
        sensorJetiExP->setUnit("mAh");
    }
    if (CONFIG_NTC1)
    {
        SensorJetiEx *sensorJetiExP;
        Ntc *ntc;
        ntc = new Ntc(PIN_NTC1, ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        deviceBufferP->add(ntc);
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, ntc->valueP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Temp 1");
        sensorJetiExP->setUnit("C");
    }
    if (CONFIG_NTC2)
    {
        SensorJetiEx *sensorJetiExP;
        Ntc *ntc;
        ntc = new Ntc(PIN_NTC2, ALPHA(CONFIG_AVERAGING_ELEMENTS_TEMP));
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 0, ntc->valueP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Temp 2");
        sensorJetiExP->setUnit("C");
    }
    if (CONFIG_I2C1_TYPE == I2C_BMP280)
    {
        SensorJetiEx *sensorJetiExP;
        Bmp280 *bmp;
        bmp = new Bmp280(CONFIG_I2C1_ADDRESS, ALPHA(CONFIG_AVERAGING_ELEMENTS_VARIO));
        bmp->begin();
        deviceBufferP->add(bmp);
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, bmp->altitudeP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Altitude");
        sensorJetiExP->setUnit("m");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, bmp->varioP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Vario");
        sensorJetiExP->setUnit("m/s");
#ifndef JETIEX_LOW_MEMORY
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, bmp->temperatureP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Temperature");
        sensorJetiExP->setUnit("C");
#endif
    }
    if (CONFIG_I2C1_TYPE == I2C_MS5611)
    {
        SensorJetiEx *sensorJetiExP;
        MS5611 *bmp;
        bmp = new MS5611(CONFIG_I2C1_ADDRESS, ALPHA(CONFIG_AVERAGING_ELEMENTS_VARIO));
        bmp->begin();
        deviceBufferP->add(bmp);
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, bmp->altitudeP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Altitude");
        sensorJetiExP->setUnit("m");
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, bmp->varioP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Vario");
        sensorJetiExP->setUnit("m/s");
#ifndef JETIEX_LOW_MEMORY
        sensorJetiExP = new SensorJetiEx(JETIEX_TYPE_INT14, 1, bmp->temperatureP());
        sensorJetiExP->setSensorId(addSensor(sensorJetiExP));
        sensorJetiExP->setText("Temperature");
        sensorJetiExP->setUnit("C");
#endif
    }
}

uint8_t JetiEx::crc8(uint8_t *crc, uint8_t crc_length)
{
    uint8_t crc_up = 0;
    uint8_t c;
    for (c = 0; c < crc_length; c++)
    {
        crc_up = update_crc8(crc[c], crc_up);
    }
    return crc_up;
}

uint8_t JetiEx::update_crc8(uint8_t crc, uint8_t crc_seed)
{
    uint8_t crc_u;
    uint8_t i;
    crc_u = crc;
    crc_u ^= crc_seed;
    for (i = 0; i < 8; i++)
    {
        crc_u = (crc_u & 0x80) ? 0x07 ^ (crc_u << 1) : (crc_u << 1);
    }
    return crc_u;
}

uint16_t JetiEx::crc16(uint8_t *p, uint16_t len)
{
    uint16_t crc16_data = 0;
    while (len--)
    {
        crc16_data = update_crc16(crc16_data, p[0]);
        p++;
    }
    return (crc16_data);
}

uint16_t JetiEx::update_crc16(uint16_t crc, uint8_t data)
{
    uint16_t ret_val;
    data ^= (uint8_t)(crc) & (uint8_t)(0xFF);
    data ^= data << 4;
    ret_val = ((((uint16_t)data << 8) | ((crc & 0xFF00) >> 8)) ^ (uint8_t)(data >> 4) ^ ((uint16_t)data << 3));
    return ret_val;
}