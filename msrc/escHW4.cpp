#include "escHW4.h"

EscHW4::EscHW4(AbstractSerial &serial, uint8_t alphaRpm, uint8_t alphaVolt, uint8_t alphaCurr, uint8_t alphaTemp, uint8_t type) : serial_(serial), alphaRpm_(alphaRpm), alphaVolt_(alphaVolt), alphaCurr_(alphaCurr), alphaTemp_(alphaTemp), type_(type)
{
}

void EscHW4::begin()
{
    serial_.begin(19200, SERIAL__8N1);
    serial_.setTimeout(ESCHW4_ESCSERIAL_TIMEOUT);
}

void EscHW4::update()
{
    uint8_t lenght = serial_.availableTimeout();
    if (lenght == ESCHWV4_PACKET_LENGHT || lenght == ESCHWV4_PACKET_LENGHT + 1)
    {
        uint8_t data[ESCHWV4_PACKET_LENGHT];
        serial_.readBytes(data, ESCHWV4_PACKET_LENGHT);
        thr_ = (uint16_t)data[4] << 8 | data[5]; // 0-1024
        pwm_ = (uint16_t)data[6] << 8 | data[7]; // 0-1024
        float rpm = (uint32_t)data[8] << 16 | (uint16_t)data[9] << 8 | data[10];
        // try to filter invalid data frames
        if (thr_ < 1024 &&
            pwm_ < 1024 &&
            rpm < 200000 &&
            data[11] < 0xF &&
            data[13] < 0xF &&
            data[15] < 0xF &&
            data[17] < 0xF)
        {
            uint16_t rawCur = (uint16_t)data[13] << 8 | data[14];
            if (rawCurrentOffset_ == -1 && rawCur > 0)
            {
                rawCurrentOffset_ = rawCur;
#ifdef DEBUG_HW4
                DEBUG_PRINT("CO:");
                DEBUG_PRINT(rawCurrentOffset_);
                DEBUG_PRINTLN();
#endif
            }
            float voltage = calcVolt((uint16_t)data[11] << 8 | data[12]);
            float current = 0;
            if (thr_ > CURRENT_THRESHOLD / 100.0 * 1024)
                current = calcCurr(rawCur);
            if (current > ESCHW4_CURRENT_MAX)
                current = 0;
            float tempFET = calcTemp((uint16_t)data[15] << 8 | data[16]);
            float tempBEC = calcTemp((uint16_t)data[17] << 8 | data[18]);
            rpm *= RPM_MULTIPLIER;
            rpm_ = calcAverage(alphaRpm_ / 100.0F, rpm_, rpm);
            consumption_ += calcConsumption(current, ESCHW4_CURRENT_MAX);
            voltage_ = calcAverage(alphaVolt_ / 100.0F, voltage_, voltage);
            current_ = calcAverage(alphaCurr_ / 100.0F, current_, current);
            tempFet_ = calcAverage(alphaTemp_ / 100.0F, tempFet_, tempFET);
            tempBec_ = calcAverage(alphaTemp_ / 100.0F, tempBec_, tempBEC);
            if (cellCount_ == 255)
                if (millis() > 15000 && voltage_ > 1)
                    cellCount_ = setCellCount(voltage_);
            cellVoltage_ = voltage_ / cellCount_;
#ifdef ESC_SIGNATURE
            signature_[10] = data[14];
            signature_[11] = data[13];
#endif
#ifdef DEBUG_HW4
            uint32_t pn = (uint32_t)data[1] << 16 | (uint16_t)data[2] << 8 | data[3];
            DEBUG_PRINT(pn);
            DEBUG_PRINT(" T:");
            DEBUG_PRINT(thr_);
            DEBUG_PRINT(" R:");
            DEBUG_PRINT(rpm);
            DEBUG_PRINT(" V:");
            DEBUG_PRINT(voltage);
            DEBUG_PRINT(" Cu:");
            DEBUG_PRINT(current);
            DEBUG_PRINT(" T:");
            DEBUG_PRINT(tempFET);
            DEBUG_PRINT(" T:");
            DEBUG_PRINT(tempBEC);
            DEBUG_PRINT(" Co:");
            DEBUG_PRINT(consumption_);
            DEBUG_PRINT(" CV:");
            DEBUG_PRINT(cellVoltage_);
            DEBUG_PRINT("\n");
#endif
        }
    }
#ifdef ESC_SIGNATURE
    if (serial_.availableTimeout() == ESCHWV4_SIGNATURE_LENGHT)
    {
        uint8_t data[ESCHWV4_SIGNATURE_LENGHT];
        serial_.readBytes(data, ESCHWV4_SIGNATURE_LENGHT);
        if (data[0] == 0x9B && data[1] == 0x9B)
        {
            memcpy(&signature_[0], &data[2], 10);
#ifdef DEBUG_HW4
            DEBUG_PRINT("S:");
            for (uint8_t i = 0; i < 12; i++)
            {
                DEBUG_PRINT_HEX(signature_[i]);
                DEBUG_PRINT(" ");
            }
            DEBUG_PRINTLN();
#endif
        }
    }
#endif
#ifdef SIM_SENSORS
    rpm_ = 12345.67;
    consumption_ = 123.4;
    voltage_ = 12.34;
    current_ = 5.678;
    tempFet_ = 12.34;
    tempBec_ = 12.34;
    cellVoltage_ = voltage_ / cellCount_;
#endif
}

float EscHW4::calcVolt(uint16_t voltRaw)
{
    return ESCHW4_V_REF * voltRaw / ESCHW4_ADC_RES * ESCHW4_DIVISOR;
}

float EscHW4::calcTemp(uint16_t tempRaw)
{
    float voltage = tempRaw * ESCHW4_V_REF / ESCHW4_ADC_RES;
    float ntcR_Rref = (voltage * ESCHW4_NTC_R1 / (ESCHW4_V_REF - voltage)) / ESCHW4_NTC_R_REF;
    if (ntcR_Rref < 0.001)
        return 0;
    float temperature = 1 / (log(ntcR_Rref) / ESCHW4_NTC_BETA + 1 / 298.15) - 273.15;
    if (temperature < 0)
        return 0;
    return temperature;
}

float EscHW4::calcCurr(uint16_t currentRaw)
{
    if (currentRaw - rawCurrentOffset_ < 0)
        return 0;
    return (currentRaw - rawCurrentOffset_) * ESCHW4_V_REF / (ESCHW4_AMPGAIN * ESCHW4_DIFFAMP_SHUNT * ESCHW4_ADC_RES);
}

float *EscHW4::rpmP()
{
    return &rpm_;
}

float *EscHW4::consumptionP()
{
    return &consumption_;
}

float *EscHW4::voltageP()
{
    return &voltage_;
}

float *EscHW4::currentP()
{
    return &current_;
}

float *EscHW4::tempFetP()
{
    return &tempFet_;
}

float *EscHW4::tempBecP()
{
    return &tempBec_;
}

float *EscHW4::cellVoltageP()
{
    return &cellVoltage_;
}

#ifdef ESC_SIGNATURE
float *EscHW4::signatureP()
{
    return (float *)&signature_[0];
}
#endif