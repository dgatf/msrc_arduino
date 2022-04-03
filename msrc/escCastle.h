#ifndef ESCCASTLE_H
#define ESCCASTLE_H

/* castleTelemetry:
    index   element                          scaler
    0       calib 1 (1000us)                 0
    1       Volt (V)                         20
    2       rippleVolt (V)                   4
    3       Curr (A)                         50
    4       Thr (0.1-0.2 ms esc pulse)       1
    5       Output power (%)                 0.2502
    6       rpm                              20416.7
    7       becVolt (V)                      4
    8       becCurr (A)                      4
    9       temp (C) or calib 2 (500us)      30
    10      temp ntc (C) or calib 2 (500us)  63.8125
*/

#define RX_MAX_CYCLES 2 // minimum is 2

#define CASTLE_VOLTAGE 1
#define CASTLE_RIPPLE_VOLTAGE 2
#define CASTLE_CURRENT 3
#define CASTLE_RPM 6
#define CASTLE_BEC_VOLTAGE 7
#define CASTLE_BEC_CURRENT 8
#define CASTLE_TEMP 9
#define CASTLE_TEMP_NTC 10
#define CASTLE_CELL_VOLTAGE 11

#define CASTLE_R0 10000.0F
#define CASTLE_R2 10200.0F
#define CASTLE_B 3455.0F

#include <Arduino.h>
#include "device.h"

#if defined(__AVR_ATmega328P__) && !defined(ARDUINO_AVR_A_STAR_328PB)
extern void (*TIMER1_CAPT_handlerP)();
extern void (*TIMER1_COMPB_handlerP)();
extern void (*INT0_handlerP)();
extern void (*TIMER2_COMPA_handlerP)();
#endif
#if defined(__AVR_ATmega328PB__) || defined(ARDUINO_AVR_A_STAR_328PB)
extern void (*TIMER1_CAPT_handlerP)();
extern void (*TIMER1_OVF_handlerP)();
extern void (*TIMER3_COMPA_handlerP)();
extern void (*TIMER4_COMPB_handlerP)();
extern void (*TIMER4_CAPT_handlerP)();
#endif
#if defined(__AVR_ATmega2560__)
extern void (*TIMER4_CAPT_handlerP)();
extern void (*TIMER4_OVF_handlerP)();
extern void (*TIMER5_COMPB_handlerP)();
extern void (*TIMER5_COMPC_handlerP)();
extern void (*TIMER5_CAPT_handlerP)();
#endif
#if defined(__AVR_ATmega32U4__)
extern void (*TIMER3_CAPT_handlerP)();
extern void (*TIMER3_OVF_handlerP)();
extern void (*TIMER1_COMPB_handlerP)();
extern void (*TIMER1_CAPT_handlerP)();
extern void (*TIMER1_COMPC_handlerP)();
#endif
#if defined(__MKL26Z64__) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
extern void (*FTM0_IRQ_handlerP)();
extern void (*FTM1_IRQ_handlerP)();

#endif

class EscCastle : public Device, Consumption
{
private:
    static volatile bool castleTelemetryReceived;
    static volatile uint16_t castleTelemetry[12], castleCompsPerMilli, castlePwmRx;
    static volatile uint8_t castleCont;
    static volatile bool castleUpdated;
    const float scaler[11] = {0, 20, 4, 50, 1, 0.2502, 20416.7, 4, 4, 30, 63.8125};
    uint8_t alphaRpm_, alphaVolt_, alphaCurr_, alphaTemp_, cellCount_ = 255;
    float voltage_ = 0, rippleVoltage_ = 0, current_ = 0, thr_ = 0, output_ = 0, rpm_ = 0, consumption_ = 0, becVoltage_ = 0, becCurrent_ = 0, temperature_ = 0, cellVoltage_ = 0;
#if defined(__AVR_ATmega328P__) && !defined(ARDUINO_AVR_A_STAR_328PB)
    static volatile uint8_t castleRxLastReceived;
    static void TIMER1_CAPT_handler();
    static void TIMER1_COMPB_handler();
    static void INT0_handler();
    static void TIMER2_COMPA_handler();
#endif
#if defined(__AVR_ATmega328PB__) || defined(ARDUINO_AVR_A_STAR_328PB)
    static void TIMER1_CAPT_handler();
    static void TIMER1_OVF_handler();
    static void TIMER3_COMPA_handler();
    static void TIMER4_COMPB_handler();
    static void TIMER4_CAPT_handler();
#endif
#if defined(__AVR_ATmega2560__)
    static void TIMER4_CAPT_handler();
    static void TIMER4_OVF_handler();
    static void TIMER5_COMPB_handler();
    static void TIMER5_COMPC_handler();
    static void TIMER5_CAPT_handler();
#endif
#if defined(__AVR_ATmega32U4__)
    static void TIMER3_CAPT_handler();
    static void TIMER3_OVF_handler();
    static void TIMER1_COMPC_handler();
    static void TIMER1_COMPB_handler();
    static void TIMER1_CAPT_handler();
#endif
#if defined(__MKL26Z64__) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
    static void FTM0_IRQ_handler();
    static void FTM1_IRQ_handler();
#endif
    float getValue(uint8_t index);
    
protected:
public:
    EscCastle(uint8_t alphaRpm, uint8_t alphaVolt, uint8_t alphaCurr, uint8_t alphaTemp);
    void begin();
    virtual void update();
    float *voltageP();
    float *rippleVoltageP();
    float *currentP();
    float *thrP();
    float *outputP();
    float *rpmP();
    float *consumptionP();
    float *becVoltageP();
    float *becCurrentP();
    float *temperatureP();
    float *cellVoltageP();
};

#endif