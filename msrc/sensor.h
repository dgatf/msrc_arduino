#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include "device.h"
#include "formatData.h"

class SensorSport : public FormatData
{
protected:
    uint16_t timestamp_ = 0, dataId_, frameId_ = 0x10;
    float *valueLP_;
    uint32_t value_;
    uint8_t refresh_;

public:
    SensorSport(uint16_t dataId, float *valueLP, uint8_t refresh);
    virtual ~SensorSport();
    uint16_t timestamp();
    void setTimestamp(uint16_t dataId);
    uint16_t dataId();
    uint16_t frameId();
    uint8_t refresh();
    virtual uint32_t valueFormatted();
};

class SensorSportDouble : public SensorSport
{
protected:
    float *valueMP_;

public:
    SensorSportDouble(uint16_t dataId, float *valueLP, float *valueMP, uint8_t refresh);
    virtual uint32_t valueFormatted();
};

class SensorSportLatLon : public SensorSportDouble
{
protected:
    uint8_t type_ = TYPE_LAT;

public:
    SensorSportLatLon(uint16_t dataId, float *lonP, float *latP, uint8_t refresh);
    uint32_t valueFormatted();
};

class SensorSportDateTime : public SensorSportDouble
{
protected:
    uint8_t type_ = TYPE_DATE;

public:
    SensorSportDateTime(uint16_t dataId, float *timeP, float *dateP, uint8_t refresh);
    uint32_t valueFormatted();
};

class SensorSportCell : public SensorSportDouble
{
protected:
    uint8_t cellIndex_ = 0;

public:
    SensorSportCell(uint16_t dataId, float *indexM, float *indexL, uint8_t cellIndex, uint8_t refresh);
    uint32_t valueFormatted();
};

class SensorFrskyD : public FormatData
{
protected:
    uint16_t timestamp_ = 0, value_;
    uint8_t dataId_;
    float *valueP_;
    uint8_t refresh_;

public:
    SensorFrskyD(uint8_t dataId, float *value, uint8_t refresh);
    ~SensorFrskyD();
    uint16_t timestamp();
    void setTimestamp(uint16_t dataId);
    uint8_t dataId();
    uint8_t refresh();
    uint16_t valueFormatted();
};

class SensorIbus : public FormatData
{
protected:
    uint8_t dataId_;
    uint8_t type_;
    float *valueP_;
    int32_t valueFormatted_ = 0;

public:
    SensorIbus(uint8_t dataId, uint8_t type, float *value);
    virtual ~SensorIbus();
    uint8_t dataId();
    uint8_t type();
    virtual uint8_t *valueFormatted();
};

class SensorSbus : public FormatData
{
protected:
    uint8_t dataId_;
    float *valueP_;

public:
    SensorSbus(uint8_t dataId, float *valueP);
    ~SensorSbus();
    uint8_t dataId();
    float *valueP();
    uint16_t valueFormatted();
};

class SensorMultiplex : public FormatData
{
protected:
    uint8_t dataId_;
    float *valueP_;

public:
    SensorMultiplex(uint8_t dataId, float *value);
    ~SensorMultiplex();
    uint8_t dataId();
    float *valueP();
    uint16_t valueFormatted();
};

class SensorJetiEx
{
protected:
    uint8_t sensorId_;
    uint8_t type_;
    uint8_t format_;
    float *valueP_;
    char text_[32] = "SENSOR";
    char unit_[8] = "";

public:
    SensorJetiEx(uint8_t type, uint8_t format, float *value);
    ~SensorJetiEx();
    void setText(const char *textP);
    void setUnit(const char *textP);
    char *textP();
    char *unitP();
    void setSensorId(uint8_t sensorId);
    uint8_t type();
    uint8_t format();
    float *valueP();
};

#endif