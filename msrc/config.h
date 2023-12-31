#ifndef CONFIG_H
#define CONFIG_H

/* Receiver protocol */
#define RX_PROTOCOL RX_SMARTPORT // RX_SMARTPORT, RX_XBUS, RX_SRXL, RX_FRSKY, RX_IBUS, RX_SBUS, RX_MULTIPLEX, RX_JETIEX, RX_HITEC

/* Sensors */
#define CONFIG_ESC_PROTOCOL PROTOCOL_NONE // PROTOCOL_NONE, PROTOCOL_HW_V3, PROTOCOL_HW_V4, PROTOCOL_PWM, PROTOCOL_CASTLE, PROTOCOL_KONTRONIK, PROTOCOL_APD_F, PROTOCOL_APD_HV
#define CONFIG_GPS false
#define GPS_BAUD_RATE 9600
#define CONFIG_VOLTAGE1 false
#define CONFIG_VOLTAGE2 false
#define CONFIG_NTC1 false
#define CONFIG_NTC2 false
#define CONFIG_CURRENT false
#define CONFIG_AIRSPEED false
#define CONFIG_I2C1_TYPE I2C_NONE // I2C_NONE, I2C_BMP280, I2C_MS5611
#define CONFIG_I2C1_ADDRESS 0x76  // 0x76, 0x77
/* Refresh rate in 0.1s (1 = 100ms) */
#define CONFIG_REFRESH_RPM 1
#define CONFIG_REFRESH_VOLT 1
#define CONFIG_REFRESH_CURR 1
#define CONFIG_REFRESH_TEMP 1
#define CONFIG_REFRESH_GPS 1
#define CONFIG_REFRESH_DEF 1
/* Averaging elements (1 = no averaging) */
#define CONFIG_AVERAGING_ELEMENTS_RPM 1
#define CONFIG_AVERAGING_ELEMENTS_VOLT 1
#define CONFIG_AVERAGING_ELEMENTS_CURR 1
#define CONFIG_AVERAGING_ELEMENTS_TEMP 1
#define CONFIG_AVERAGING_ELEMENTS_VARIO 1
#define CONFIG_AVERAGING_ELEMENTS_DEF 1
/* Analog voltage sensors */
#define VOLTAGE1_MULTIPLIER 7.8  // 7.8 if using 68k and 10k as proposed in the docs
#define VOLTAGE2_MULTIPLIER 7.8
/* Analog current sensor */
#define CURRENT_MULTIPLIER 1 // current_multiplier = 1000 / sensitivity(mV/A)
/* Zero current output voltage offset or quiescent voltage (voltage offset when I = 0, Viout)
 - All hall effect core sensors (Amploc) are bidirectional. Viout=Vs/2
 - Hall effect coreless sensors (IC) (ACS758) can be bidirectional or directional. Recommended to use directional for higher sensitivity. Viout defined in datasheet
 - If CURRENT_AUTO_OFFSET is true, then after 5 seconds, voltage read is set as offset. It is recommended to use auto offset
*/
#define CURRENT_OFFSET 0
#define CURRENT_AUTO_OFFSET true
/* RPM multipliers (optional, this may be done in transmitter*/
#define RPM_PAIR_OF_POLES 1
#define RPM_PINION_TEETH 1  // For helis
#define RPM_MAIN_TEETH 1    // For helis
/* BMP Filter. Higher filter = lower noise: 1 - low, 2 - medium, 3 - high */
#define BMP280_FILTER 3
/* Pwm out */
#define CONFIG_PWMOUT false
#define PWMOUT_DUTY 0.5 // 0.5 = 50%

/* Only smartport and opentx */
#define SENSOR_ID 10 // Sensor Id 
#define DATA_ID 0x5000 // DataId (sensor type)
//#define ESC_SIGNATURE // HW V4 signature (only smartport). This outputs esc signature and raw current to sensors 5100, 5101 and 5102
//#define CONFIG_LUA // Comment if not using lua script for configuration (only smartport)

/* XBus */
//#define XBUS_CLOCK_STRECH_SWITCH

/* Jeti Ex */
#define JETI_GPS_SPEED_UNITS_KMH

/* Use library I2C_T3 for Teensy LC/3.X */
#define I2C_T3_TEENSY

/* Add init delay for FlyFun ESC. Uncomment if the ESC doesn't arm */
//#define ESC_INIT_DELAY 10000

/* HW V4/V5 parameters */
#define CURRENT_THRESHOLD 25
#define ESCHW4_DIVISOR 11
#define ESCHW4_AMPGAIN 10
#define ESCHW4_CURRENT_MAX 250

/* Force eeprom write */
//#define FORCE_EEPROM_WRITE // Uncomment to force write eeprom as defined in config.h. Useful when using lua and eeprom is messed up. Reflash againg with line commented or config will be reset at power up 

/* Debug
   Disconnect Vcc from the RC model to the Arduino
   Do not connect at the same time Vcc from the model and usb (TTL)
   Telemetry may not work properly in debug mode
   Connect arduino Rx to TTL Tx for flashing, then if applicabe connect arduino Rx to esc or gps
*/

//#define DEBUG
//#define DEBUG_PACKET
//#define DEBUG_SBUS_MS
//#define DEBUG_EEPROM_WRITE
//#define DEBUG_EEPROM_READ

//#define DEBUG_HW3
//#define DEBUG_HW4
//#define DEBUG_KONTRONIK
//#define DEBUG_APDF
//#define DEBUG_APDF_PACKET
//#define DEBUG_APDHV
//#define DEBUG_GPS
//#define DEBUG_PWM
//#define DEBUG_CASTLE
//#define DEBUG_CASTLE_RX
//#define DEBUG_MS5611

//#define SIM_RX
//#define SIM_SENSORS
//#define SIM_LUA_SEND
//#define SIM_LUA_RECEIVE

#endif