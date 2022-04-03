#ifndef SOFTSERIAL_H
#define SOFTSERIAL_H

#include <Arduino.h>
#include "serial.h"
#include "hardserial.h"

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__) || defined(ARDUINO_AVR_A_STAR_328PB) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega32U4__)

#if defined(__AVR_ATmega32U4__)
#define PCINTx_vect PCINT0_vect
#define PCMSKx PCMSK0
#define PCINTxn PCINT3
#define PCIEx PCIE0
#else
#define PCINTx_vect PCINT2_vect
#define PCMSKx PCMSK2
#define PCINTxn PCINT23
#define PCIEx PCIE2
#endif

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__) || defined(ARDUINO_AVR_A_STAR_328PB)
#define PORTx PORTD
#define PORTxn PORTD7
#define PINx PIND
#define PINxn PIND7
#endif

#if defined(__AVR_ATmega2560__)
#define PORTx PORTK
#define PORTxn PORTK7
#define PINx PINK
#define PINxn PINK7
#endif

#if defined(__AVR_ATmega32U4__)
#define PORTx PORTB
#define PORTxn PORTB3
#define PINx PINB
#define PINxn PINB3
#endif

#if defined(__AVR_ATmega328PB__) || defined(ARDUINO_AVR_A_STAR_328PB)
#define DDBx DDB1
#define PORTBx PORTB1
#else
#define DDBx DDB4
#define PORTBx PORTB4
#endif

#define SOFTSERIAL_IDLE 0
#define SOFTSERIAL_RECEIVING 1
#define SOFTSERIAL_SENDING 2

/*  
    RX & TX Pins:

    RX: 328P/PB: PIN 7   (PD7,PCINT23)
        2560:    PIN A15 (PK7,PCINT23)
        32U4:    PIN B3/14  (PB3,PCINT3)

    TX: 328P:    PIN 12  (PB4)
        328PB:   PIN 9   (PB1)
        2560:    PIN D10 (PB4)
        32U4:    PIN B4/8(PB4)
*/

#define setPinHigh PORTB |= _BV(PORTBx)
#define setPinLow PORTB &= ~_BV(PORTBx)

class SoftSerial : public AbstractSerial
{
private:
    volatile bool timedout = false;
    uint8_t timeout_ = 0;
    bool inverted_;
    bool half_duplex_;
    uint8_t stop_bits_;
    uint8_t parity_;
    volatile uint16_t ts = 0;
    uint16_t tx_delay_start;
    uint16_t tx_delay;
    uint16_t tx_delay_parity;
    uint16_t tx_delay_stop;
    uint16_t rx_delay;
    uint16_t rx_delay_centering;
    uint16_t rx_delay_stop;
    uint16_t subs(uint16_t val1, uint16_t val2);
#if defined(__AVR_ATmega32U4__)
    inline void _delay_loop_2(uint16_t delay);
#endif
    void initWrite();

public:
    SoftSerial();
    void begin(uint32_t baud, uint8_t format);
    void begin(uint32_t baud) { begin(baud, SERIAL__8N1); }
    uint8_t availableTimeout();
    void setTimeout(uint16_t timeout);
    uint16_t timestamp();
    void PCINT_handler();
    void TIMER_COMP_handler();
};

extern SoftSerial softSerial;

#endif

#endif