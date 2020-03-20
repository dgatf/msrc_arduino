/*
 *
 * License https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * Arduino library to communicate with the Frsky Smartport protocol
 * 
 * 
 */

#ifndef SMARTPORT_H
#define SMARTPORT_H

#define LED_SMARTPORT LED_BUILTIN
#define SMARTPORT_TIMEOUT 2

//#define DEBUG
//#define SIM_POLL
//#define SIM_SENSORS

#define SENSOR_ID_1 0x00 // VARIO 0x100 (0 in opentx lua: id - 1)
#define SENSOR_ID_2 0xA1 // FLVSS 0x300
#define SENSOR_ID_3 0x22 // FAS40-S 0x200
#define SENSOR_ID_4 0x83 // GPS 0x800
#define SENSOR_ID_5 0xE4 // RPM 0x500
#define SENSOR_ID_6 0x45 // UART
#define SENSOR_ID_7 0xC6 // UART
#define SENSOR_ID_8 0x67
#define SENSOR_ID_9 0x48
#define SENSOR_ID_10 0xE9
#define SENSOR_ID_11 0x6A
#define SENSOR_ID_12 0xCB
#define SENSOR_ID_13 0xAC
#define SENSOR_ID_14 0xD
#define SENSOR_ID_15 0x8E
#define SENSOR_ID_16 0x2F
#define SENSOR_ID_17 0xD0
#define SENSOR_ID_18 0x71
#define SENSOR_ID_19 0xF2
#define SENSOR_ID_20 0x53
#define SENSOR_ID_21 0x34
#define SENSOR_ID_22 0x95
#define SENSOR_ID_23 0x16
#define SENSOR_ID_24 0xB7 // Accel 0x700
#define SENSOR_ID_25 0x98
#define SENSOR_ID_26 0x39 // Power box
#define SENSOR_ID_27 0xBA // Temp
#define SENSOR_ID_28 0x1B // Fuel 0x600

#define ALT_FIRST_ID 0x0100 // 100 m
#define ALT_LAST_ID 0x010f
#define VARIO_FIRST_ID 0x0110 // 100 m/s
#define VARIO_LAST_ID 0x011f
#define CURR_FIRST_ID 0x0200 // 10 A
#define CURR_LAST_ID 0x020f
#define VFAS_FIRST_ID 0x0210 // 100 v
#define VFAS_LAST_ID 0x021f
#define CELLS_FIRST_ID 0x0300 //
#define CELLS_LAST_ID 0x030f
#define T1_FIRST_ID 0x0400 // 1 C
#define T1_LAST_ID 0x040f
#define T2_FIRST_ID 0x0410 // 1 C
#define T2_LAST_ID 0x041f
#define RPM_FIRST_ID 0x0500 // 1 rpm
#define RPM_LAST_ID 0x050f
#define FUEL_FIRST_ID 0x0600 // 1 %
#define FUEL_LAST_ID 0x060f
#define ACCX_FIRST_ID 0x0700 // 100 g
#define ACCX_LAST_ID 0x070f
#define ACCY_FIRST_ID 0x0710 // 100 g
#define ACCY_LAST_ID 0x071f
#define ACCZ_FIRST_ID 0x0720 // 100 g
#define ACCZ_LAST_ID 0x072f
#define GPS_LONG_LATI_FIRST_ID 0x0800 // 100
#define GPS_LONG_LATI_LAST_ID 0x080f
#define GPS_ALT_FIRST_ID 0x0820 // 100 m
#define GPS_ALT_LAST_ID 0x082f
#define GPS_SPEED_FIRST_ID 0x0830 // 1000 kts
#define GPS_SPEED_LAST_ID 0x083f
#define GPS_COURS_FIRST_ID 0x0840 // 100 º
#define GPS_COURS_LAST_ID 0x084f
#define GPS_TIME_DATE_FIRST_ID 0x0850 // ?
#define GPS_TIME_DATE_LAST_ID 0x085f
#define A3_FIRST_ID 0x0900 // 100 v
#define A3_LAST_ID 0x090f
#define A4_FIRST_ID 0x0910 // 100 v
#define A4_LAST_ID 0x091f
#define AIR_SPEED_FIRST_ID 0x0a00 // 10 kts
#define AIR_SPEED_LAST_ID 0x0a0f
#define RBOX_BATT1_FIRST_ID 0x0b00 // 1000 v
#define RBOX_BATT1_LAST_ID 0x0b0f
#define RBOX_BATT2_FIRST_ID 0x0b10 // 1000 v
#define RBOX_BATT2_LAST_ID 0x0b1f
#define RBOX_STATE_FIRST_ID 0x0b20 // 1
#define RBOX_STATE_LAST_ID 0x0b2f
#define RBOX_CNSP_FIRST_ID 0x0b30 // 1 mAh
#define RBOX_CNSP_LAST_ID 0x0b3f
#define SD1_FIRST_ID 0x0b40
#define SD1_LAST_ID 0x0b4f
#define ESC_POWER_FIRST_ID 0x0b50 // bytes 1,2: 100 V,  bytes 3,4: 100 A
#define ESC_POWER_LAST_ID 0x0b5f
#define ESC_RPM_CONS_FIRST_ID 0x0b60 // bytes 1,2: 0.01 rpm,  bytes 3,4: 1 mah
#define ESC_RPM_CONS_LAST_ID 0x0b6f
#define ESC_TEMPERATURE_FIRST_ID 0x0b70 // 1 C
#define ESC_TEMPERATURE_LAST_ID 0x0b7f
#define X8R_FIRST_ID 0x0c20
#define X8R_LAST_ID 0x0c2f
#define S6R_FIRST_ID 0x0c30
#define S6R_LAST_ID 0x0c3f
#define GASSUIT_TEMP1_FIRST_ID 0x0d00 // 1 C
#define GASSUIT_TEMP1_LAST_ID 0x0d0f
#define GASSUIT_TEMP2_FIRST_ID 0x0d10 // 1 C
#define GASSUIT_TEMP2_LAST_ID 0x0d1f
#define GASSUIT_SPEED_FIRST_ID 0x0d20 // 1 rpm
#define GASSUIT_SPEED_LAST_ID 0x0d2f
#define GASSUIT_RES_VOL_FIRST_ID 0x0d30 // 1 ml
#define GASSUIT_RES_VOL_LAST_ID 0x0d3f
#define GASSUIT_RES_PERC_FIRST_ID 0x0d40 // 1 %
#define GASSUIT_RES_PERC_LAST_ID 0x0d4f
#define GASSUIT_FLOW_FIRST_ID 0x0d50 // 1 ml
#define GASSUIT_FLOW_LAST_ID 0x0d5f
#define GASSUIT_MAX_FLOW_FIRST_ID 0x0d60 // 1 ml
#define GASSUIT_MAX_FLOW_LAST_ID 0x0d6f
#define GASSUIT_AVG_FLOW_FIRST_ID 0x0d70 // 1 ml
#define GASSUIT_AVG_FLOW_LAST_ID 0x0d7f
#define SBEC_POWER_FIRST_ID 0x0e50 // bug opentx
#define SBEC_POWER_LAST_ID 0x0e5f
#define DIY_FIRST_ID 0x5100
#define DIY_LAST_ID 0x52ff
#define DIY_STREAM_FIRST_ID 0x5000
#define DIY_STREAM_LAST_ID 0x50ff
#define FACT_TEST_ID 0xf000
#define RSSI_ID 0xf101
#define A1_ID 0xf102 // 10 v
#define A2_ID 0xf103 // 10 v
#define SP2UART_A_ID 0xfd00
#define SP2UART_B_ID 0xfd01
#define RXBT_ID 0xf104 // 10 v
#define RAS_ID 0xf105
#define XJT_VERSION_ID 0xf106
#define FUEL_QTY_FIRST_ID 0x0a10 // 100 ml
#define FUEL_QTY_LAST_ID 0x0a1f

#define RECEIVED_NONE 0
#define RECEIVED_POLL 1
#define RECEIVED_PACKET 2
#define SENT_TELEMETRY 3
#define SENT_VOID 4
#define SENT_PACKET 5
#define SENT_NONE 6
#define MAINTENANCE_ON 7
#define MAINTENANCE_OFF 8
#define SENT_SENSOR_ID 9
#define CHANGED_SENSOR_ID 10

#include <Arduino.h>

class AbstractDevice
{
private:
protected:
    float calcAverage(float alpha, float value, float newValue);

public:
    AbstractDevice();
    virtual float read(uint8_t index) = 0;
};

class Sensor
{
protected:
    uint16_t timestamp_ = 0, dataId_, frameId_ = 0x10;
    float valueL_ = 0, valueM_ = 0;
    uint8_t indexL_ = 0, indexM_ = 255;
    uint8_t refresh_;
    AbstractDevice *device_;

public:
    Sensor(uint16_t dataId, uint8_t indexM, uint8_t indexL, uint8_t refresh, AbstractDevice *device);
    Sensor(uint16_t dataId, uint8_t indexL, uint8_t refresh, AbstractDevice *device);
    Sensor(uint16_t dataId, uint8_t refresh, AbstractDevice *device);
    Sensor *nextP = NULL;
    uint16_t timestamp();
    void setTimestamp(uint16_t dataId);
    uint16_t dataId();
    uint16_t frameId();
    uint8_t refresh();
    uint8_t indexL();
    uint8_t indexM();
    float valueL();
    void setValueL(float valueM);
    float valueM();
    void setValueM(float valueM);
    float read(uint8_t index);
};

class Smartport
{
private:
    struct Packet
    {
        uint8_t frameId;
        uint16_t dataId;
        uint32_t value;
    };
    Stream &serial_;
    Sensor *sensorP = NULL;
    Packet *packetP = NULL;
    uint8_t sensorId_, sensorIdTx_;
    uint16_t dataId_;
    bool maintenanceMode_ = false;
    void sendByte(uint8_t c, uint16_t *crcp);

public:
    //Smartport(Stream &serial, uint8_t dataId, uint8_t sensorId, uint8_t sensorIdTx);
    Smartport(Stream &serial);
    ~Smartport();
    uint8_t idToCrc(uint8_t sensorId);
    uint8_t crcToId(uint8_t sensorIdCrc);
    uint8_t available();
    uint8_t read(uint8_t *data);
    void sendData(uint16_t dataId, uint32_t val);
    void sendData(uint8_t frameId, uint16_t dataId, uint32_t val);
    void sendVoid();
    uint32_t formatData(uint16_t dataId, float valueM, float valueL);
    uint32_t formatData(uint16_t dataId, float valueL);

    void setDataId(uint16_t dataId);
    uint8_t sensorId();
    void setSensorId(uint8_t sensorId);
    void setSensorIdTx(uint8_t sensorIdTx);
    uint8_t maintenanceMode();
    void setMaintenanceMode(uint8_t maintenanceMode);
    void addSensor(Sensor *newSensorP);
    bool addPacket(uint16_t dataId, uint32_t value);
    bool addPacket(uint8_t frameId, uint16_t dataId, uint32_t value);
    void deleteSensors();
    uint8_t update(uint8_t &frameId, uint16_t &dataId, uint32_t &value);
    uint8_t update();
    bool sendPacketReady();
};

#endif
