#include "sensor.h"

SensorSport::SensorSport(uint16_t dataId, float *valueLP, uint8_t refresh) : dataId_(dataId), valueLP_(valueLP), refresh_(refresh) {}

SensorSport::~SensorSport()
{
}

uint32_t SensorSport::valueFormatted()
{
#ifdef ESC_SIGNATURE
    if (dataId_ > DIY_STREAM_FIRST_ID && dataId_ < DIY_LAST_ID)
    {
        uint32_t buffer;
        memcpy(&buffer, valueLP_, 4);
        return buffer;
    }
#endif
    return formatData(dataId_, *valueLP_);
}

uint16_t SensorSport::timestamp()
{
    return timestamp_;
}

void SensorSport::setTimestamp(uint16_t timestamp)
{
    timestamp_ = timestamp;
}

uint16_t SensorSport::dataId()
{
    return dataId_;
}

uint16_t SensorSport::frameId()
{
    return frameId_;
}

uint8_t SensorSport::refresh()
{
    return refresh_;
}

SensorSportDouble::SensorSportDouble(uint16_t dataId, float *valueLP, float *valueMP, uint8_t refresh) : SensorSport(dataId, valueLP, refresh), valueMP_(valueMP) {}

uint32_t SensorSportDouble::valueFormatted()
{
    float valueLP = 0;
    float valueMP = 0;
    if (valueLP_ != NULL)
        valueLP = *valueLP_;
    if (valueMP_ != NULL)
        valueMP = *valueMP_;
    return formatData(dataId_, valueLP, valueMP);
}

SensorSportLatLon::SensorSportLatLon(uint16_t dataId, float *lonP, float *latP, uint8_t refresh) : SensorSportDouble(dataId, lonP, latP, refresh) {}

uint32_t SensorSportLatLon::valueFormatted()
{
    if (type_ == TYPE_LAT)
    {
        type_ = !type_;
        return formatLatLon(TYPE_LAT, *valueMP_);
    }
    else
    {
        type_ = !type_;
        return formatLatLon(TYPE_LON, *valueLP_);
    }
}

SensorSportDateTime::SensorSportDateTime(uint16_t dataId, float *timeP, float *dateP, uint8_t refresh) : SensorSportDouble(dataId, timeP, dateP, refresh) {}

uint32_t SensorSportDateTime::valueFormatted()
{
    if (!type_)
    {
        type_ = !type_;
        return formatDateTime(TYPE_DATE, *valueMP_);
    }
    else
    {
        type_ = !type_;
        return formatDateTime(TYPE_TIME, *valueLP_);
    }
}

SensorSportCell::SensorSportCell(uint16_t dataId, float *indexMP, float *indexLP, uint8_t cellIndex, uint8_t refresh) : SensorSportDouble(dataId, indexMP, indexLP, refresh), cellIndex_(cellIndex) {}

uint32_t SensorSportCell::valueFormatted()
{
    return formatCell(cellIndex_, *valueMP_, *valueLP_);
}

SensorFrskyD::SensorFrskyD(uint8_t dataId, float *valueP, uint8_t refresh) : dataId_(dataId), valueP_(valueP), refresh_(refresh) {}

SensorFrskyD::~SensorFrskyD()
{
}

uint16_t SensorFrskyD::valueFormatted()
{
    return formatData(dataId_, *valueP_);
}

uint16_t SensorFrskyD::timestamp()
{
    return timestamp_;
}

void SensorFrskyD::setTimestamp(uint16_t timestamp)
{
    timestamp_ = timestamp;
}

uint8_t SensorFrskyD::dataId()
{
    return dataId_;
}

uint8_t SensorFrskyD::refresh()
{
    return refresh_;
}

SensorIbus::SensorIbus(uint8_t dataId, uint8_t type, float *valueP) : dataId_(dataId), type_(type), valueP_(valueP) {}

SensorIbus::~SensorIbus()
{
}

uint8_t SensorIbus::dataId()
{
    return dataId_;
}

uint8_t SensorIbus::type()
{
    return type_;
}

uint8_t *SensorIbus::valueFormatted()
{
    if (valueP_)
    {
        valueFormatted_ = formatIbus(dataId_, *valueP_);
    }
    return (uint8_t *)&valueFormatted_;
}

SensorSbus::SensorSbus(uint8_t dataId, float *valueP) : dataId_(dataId), valueP_(valueP) {}

SensorSbus::~SensorSbus()
{
}

uint16_t SensorSbus::valueFormatted()
{
    if (valueP_)
        return formatSbus(dataId_, *valueP_);
    return formatSbus(dataId_, 0);
}

uint8_t SensorSbus::dataId()
{
    return dataId_;
}

float *SensorSbus::valueP()
{
    return valueP_;
}

SensorMultiplex::SensorMultiplex(uint8_t dataId, float *valueP) : dataId_(dataId), valueP_(valueP) {}

SensorMultiplex::~SensorMultiplex()
{
}

uint16_t SensorMultiplex::valueFormatted()
{
    return formatMultiplex(dataId_, *valueP_);
}

uint8_t SensorMultiplex::dataId()
{
    return dataId_;
}

float *SensorMultiplex::valueP()
{
    return valueP_;
}

SensorJetiEx::SensorJetiEx(uint8_t type, uint8_t format, float *valueP) : type_(type), format_(format), valueP_(valueP) {}

SensorJetiEx::~SensorJetiEx()
{
}

float *SensorJetiEx::valueP()
{
    return valueP_;
}

void SensorJetiEx::setText(const char *textP)
{
    strncpy(text_, textP, 32);
}

void SensorJetiEx::setUnit(const char *textP)
{
    strncpy(unit_, textP, 8);
}

char *SensorJetiEx::textP()
{
    return text_;
}

char *SensorJetiEx::unitP()
{
    return unit_;
}

void SensorJetiEx::setSensorId(uint8_t sensorId)
{
    sensorId_ = sensorId;
}

uint8_t SensorJetiEx::type()
{
    return type_;
}

uint8_t SensorJetiEx::format()
{
    return format_;
}
