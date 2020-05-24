#include "escCastle.h"

volatile bool castleTelemetryReceived = false;
#ifdef SIM_SENSORS
volatile uint16_t castleTelemetry[12] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 500};
#else
volatile uint16_t castleTelemetry[12] = {0};
#endif
volatile uint16_t castleCompsPerMilli = 1 * MS_TO_COMP(8);
volatile uint8_t castleCont = 0;
volatile uint8_t castleRxLastReceived = 0;

EscCastleInterface::EscCastleInterface(uint8_t alphaRpm, uint8_t alphaVolt, uint8_t alphaCurr, uint8_t alphaTemp) : alphaRpm_(alphaRpm), alphaVolt_(alphaVolt), alphaCurr_(alphaCurr), alphaTemp_(alphaTemp) {}

void EscCastleInterface::TIMER1_CAPT_handler() // RX INPUT
{
    static uint16_t ts = 0;
    if (TCCR1B & _BV(ICES1)) // RX RISING
    {
        ts = ICR1;
    }
    else // RX FALLING
    {
        if (ts < ICR1)
        {
            OCR1B = ICR1 - ts;
        }
        else
        {
            OCR1B = ICR1 + OCR1A - ts;
        }
        castleRxLastReceived = 0;
    }
    TCCR1B ^= _BV(ICES1); // TOGGLE ICP1 DIRECTION
}

void EscCastleInterface::TIMER1_COMPB_handler() // START INPUT STATE
{
    if (OCR1B) {
        DDRB &= ~_BV(DDB2);   // PWM OUT (PB2, PIN10) INPUT
        PORTB |= _BV(PB2);    // PB2 PULLUP
        EIFR |= _BV(INTF0);   // CLEAR INT0 FLAG
        EIMSK = _BV(INT0);    // ENABLE INT0 (PD2, PIN2)
        TIFR2 |= _BV(OCF2A);  // CLEAR TIMER2 COMPA FLAG
        TIMSK2 = _BV(OCIE2A); // ENABLE TIMER2 COMPA
        TCNT2 = 0;            // RESET TIMER2
    }
}

void EscCastleInterface::INT0_handler() // READ TELEMETRY
{
    castleTelemetry[castleCont] = TCNT1 - OCR1B;
    castleCont++;
    castleTelemetryReceived = true;
}

void EscCastleInterface::TIMER2_COMPA_handler() // START OUTPUT STATE
{
    if (!castleTelemetryReceived)
    {
        castleCompsPerMilli = castleTelemetry[0] / 2 + (castleTelemetry[9] < castleTelemetry[10] ? castleTelemetry[9] : castleTelemetry[10]);
        castleCont = 0;
    }
    castleTelemetryReceived = false;
    if (castleRxLastReceived > RX_MAX_CYCLES)
    {
        OCR1B = 0;
    }
    else
    {
        castleRxLastReceived++;
    }
    EIMSK = 0;        // DISABLE INT0 (PD2, PIN2)
    TIMSK2 = 0;       // DISABLE TIMER2 INTS
    DDRB = _BV(DDB2); // PWM OUT PIN 10 OUTPUT
}

void EscCastleInterface::begin()
{
    TIMER1_CAPT_handlerP = TIMER1_CAPT_handler;
    TIMER1_COMPB_handlerP = TIMER1_COMPB_handler;
    INT0_handlerP = INT0_handler;
    TIMER2_COMPA_handlerP = TIMER2_COMPA_handler;

    // ICP1 (PB0 PIN8) -> RX THR, INPUT
    // INT0 (PD2, PIN2)-> TELEMETRY, INPUT
    // PWM OUT OC1B (PB2, PIN10) -> OUTPUT/INPUT PULL UP

    // TIMER 1, ICP1
    DDRB = _BV(DDB2);                                             // PWM OUT PIN 10
    TCCR1A = _BV(WGM11) | _BV(WGM10) | _BV(COM1B1) | _BV(COM1B0); // TOGGLE OC1A ON OCR1B
    TCCR1B = _BV(WGM13) | _BV(WGM12);                             // MODE 15
    TCCR1B |= _BV(ICES1);                                         // RISING
    TCCR1B |= _BV(CS11);                                          // SCALER 8
    TIMSK1 = _BV(OCIE1B) | _BV(ICIE1);                            // INTS: CAPT, COMPB
    OCR1A = 20 * MS_TO_COMP(8);                                   // 50Hz = 20ms

    // INT0
    EICRA = _BV(ISC01); // INT0 FALLING

    // TIMER 2
    TCCR2A = 0;                                 // NORMAL MODE
    TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS21); // SCALER 1024
    OCR2A = 12 * MS_TO_COMP(1024);              // 12ms
}

float EscCastleInterface::read(uint8_t index)
{
    float value;
    if (cellCount_ == 255)
    {
        if (millis() > 10000)
        {
            cellCount_ = setCellCount(((float)castleTelemetry[CASTLE_VOLTAGE] / castleCompsPerMilli - 0.5) * scaler[CASTLE_VOLTAGE]);
        }
    }
    switch (index)
    {
    case 0 ... 8:
        value = ((float)castleTelemetry[index] / castleCompsPerMilli - 0.5) * scaler[index];
        break;
    case 9:
        if (castleTelemetry[9] > castleTelemetry[10])
        {
            value = ((float)castleTelemetry[index] / castleCompsPerMilli - 0.5) * scaler[index];
        }
        else
        {
            value = 0;
        }
        break;
    case 10:
        if (castleTelemetry[10] > castleTelemetry[9])
        {
            float ntc_value = ((float)castleTelemetry[index] / castleCompsPerMilli - 0.5) * scaler[index];
            value = 1 / (log(ntc_value * R2 / (255.0F - ntc_value) / R0) / B + 1 / 298.0F) - 273.0F;
        }
        else
        {
            value = 0;
        }
        break;
    case 11:
        value = (((float)castleTelemetry[1] / castleCompsPerMilli - 0.5) * scaler[1]) / cellCount_;
        break;
    default:
        value = 0;
        break;
    }
#ifdef DEBUG_ESC
    Serial.print("Value [");
    Serial.print(index);
    Serial.print("]: ");
    Serial.println(value);
#endif
    if (value < 0)
        return 0;
    return value;
}