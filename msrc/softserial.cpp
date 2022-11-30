#include "softserial.h"

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__) || defined(ARDUINO_AVR_A_STAR_328PB) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega32U4__)

ISR(PCINTx_vect)
{
    softSerial.PCINT_handler();
}

SoftSerial::SoftSerial() {}

void SoftSerial::initWrite()
{
    DDRB |= _BV(DDBx);

    uint8_t outgoingByte = readTx();
    uint8_t oldSREG = SREG;
    bool inv = inverted_;
    uint16_t delay_start = tx_delay_start;
    uint16_t delay = tx_delay;
    uint16_t delay_parity = tx_delay_parity;
    uint16_t delay_stop = tx_delay_stop;
    uint8_t parity = parity_;
    uint8_t parity_value = 0;

    if (inv)
        outgoingByte = ~outgoingByte;

    cli();

    // start bit
    if (inv)
        setPinHigh;
    else
        setPinLow;
    _delay_loop_2(delay_start);

    // data
    for (uint8_t i = 8; i > 0; i--)
    {
        if (outgoingByte & 1)
            setPinHigh;
        else
            setPinLow;
        parity_value ^= outgoingByte & 1;
        outgoingByte >>= 1;
        _delay_loop_2(delay);
    }

    // parity
    if (parity)
    {
        parity_value ^= parity & 1;
        if (inv)
            parity_value = !parity_value;
        if (parity_value)
            setPinHigh;
        else
            setPinLow;
        _delay_loop_2(delay_parity);
    }

    // stop bit
    if (inv)
        setPinLow;
    else
        setPinHigh;

    _delay_loop_2(delay_stop);

    if (half_duplex_)
        DDRB &= ~_BV(DDBx);

    SREG = oldSREG;
}

uint8_t SoftSerial::availableTimeout()
{
    uint8_t availableCopy = available();
    if (timedout)
        return availableCopy;
    return 0;
}

uint16_t SoftSerial::subs(uint16_t val1, uint16_t val2)
{
    if (val1 > val2)
        return val1 - val2;
    return 1;
}

SoftSerial softSerial;

#endif

#if defined(__AVR_ATmega32U4__)

ISR(TIMER4_COMPA_vect)
{
    softSerial.TIMER_COMP_handler();
}

void SoftSerial::TIMER_COMP_handler()
{
    TIMSK4 &= ~_BV(OCIE4A);
    if (timeout_)
        timedout = true;
    ts = micros();
}

inline void SoftSerial::_delay_loop_2(uint16_t delay)
{
    asm volatile(
        "1: sbiw %0,1"
        "\n\t"
        "brne 1b"
        : "=w"(delay)
        : "0"(delay));
}

void SoftSerial::PCINT_handler()
{
    if (inverted_ ? bit_is_set(PINx, PINxn) : !bit_is_set(PINx, PINxn))
    {
        PCMSKx &= ~_BV(PCINTxn);
        uint8_t incomingByte = 0;

        // start bit
        _delay_loop_2(rx_delay_centering);

        // data
        for (uint8_t i = 8; i > 0; --i)
        {
            _delay_loop_2(rx_delay);
            incomingByte >>= 1;
            if (bit_is_set(PINx, PINxn))
                incomingByte |= 0x80;
        }

        if (inverted_)
            incomingByte = ~incomingByte;

        // parity
        if (parity_)
            _delay_loop_2(rx_delay);
            
        // stop bits
        //_delay_loop_2(rx_delay_stop);

        if (timedout)
            reset();
        writeRx(incomingByte);
        timedout = false;
        PCIFR = B111;
        PCMSKx |= _BV(PCINTxn);
        OCR4A = TCNT4 + timeout_;
        TIFR4 |= _BV(OCF4A);
        TIMSK4 |= _BV(OCIE4A);
    }
}

void SoftSerial::setTimeout(uint16_t timeout)
{
    timeout_ = timeout * US_TO_COMP(8);
}

uint16_t SoftSerial::timestamp()
{
    return (uint16_t)(micros() - ts) + timeout_ / US_TO_COMP(8);
}

void SoftSerial::begin(uint32_t baud, uint8_t format)
{

    PCICR |= _BV(PCIEx);    // ENABLE PCINT
    PCMSKx |= _BV(PCINTxn); // PCINT MASK

    inverted_ = format & 0x40;
    half_duplex_ = format & 0x80;

    if (!half_duplex_)
    {
        DDRB |= _BV(DDBx);
        if (inverted_)
            setPinLow;
        else
            setPinHigh;
    }
    else if (!inverted_)
        PORTx |= _BV(PORTxn); // RX PULLUP for half duplex for non inverted signal. For inverted signal, an external pull down resistor maybe needed. Not needed if the other side has one

    // 1 bit delay in 4 clock cycles
    uint16_t delay = (F_CPU / baud) / 4;
    // substract overheads
    tx_delay_start = subs(delay, 14 / 4);
    tx_delay = subs(delay, 19 / 4);
    tx_delay_parity = subs(delay, 14 / 4);
    tx_delay_stop = subs(delay * stop_bits_, (71 + 10) / 4);
    rx_delay = subs(delay, 15 / 4);
    rx_delay_centering = subs(delay / 2, (4 + 4 + 75 + 17 - 15) / 4);
    rx_delay_stop = subs(delay * 3 * stop_bits_ / 4, (37 + 11 + 12) / 4);

    TCCR4A = 0;
    TCCR4B |= _BV(CS42); // SCALER 8
}

#endif

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__) || defined(ARDUINO_AVR_A_STAR_328PB) || defined(__AVR_ATmega2560__)

ISR(TIMER2_COMPB_vect)
{
    softSerial.TIMER_COMP_handler();
}

void SoftSerial::TIMER_COMP_handler()
{
    TIMSK2 &= ~_BV(OCIE2B);
    if (timeout_)
        timedout = true;
    ts = micros();
}

void SoftSerial::PCINT_handler()
{
    if (inverted_ ? bit_is_set(PINx, PINxn) : !bit_is_set(PINx, PINxn))
    {
        PCMSKx &= ~_BV(PCINTxn);
        uint8_t incomingByte = 0;

        // start bit
        _delay_loop_2(rx_delay_centering);

        for (uint8_t i = 8; i > 0; --i)
        {
            _delay_loop_2(rx_delay);
            incomingByte >>= 1;
            if (bit_is_set(PINx, PINxn))
                incomingByte |= 0x80;
        }

        if (inverted_)
            incomingByte = ~incomingByte;

        // parity
        if (parity_)
            _delay_loop_2(rx_delay);

        // stop bits
        //_delay_loop_2(rx_delay_stop);

        if (timedout)
            reset();
        writeRx(incomingByte);
        timedout = false;
        PCIFR = B111;
        PCMSKx |= _BV(PCINTxn);
        OCR2B = TCNT2 + timeout_;
        TIFR2 |= _BV(OCF2B);
        TIMSK2 |= _BV(OCIE2B);
    }
}

void SoftSerial::setTimeout(uint16_t timeout)
{
    timeout_ = timeout * US_TO_COMP(256);
}

uint16_t SoftSerial::timestamp()
{
    return (uint16_t)(micros() - ts) + timeout_ / US_TO_COMP(256);
}

void SoftSerial::begin(uint32_t baud, uint8_t format)
{
    PCICR |= _BV(PCIEx);    // ENABLE PCINT
    PCMSKx |= _BV(PCINTxn); // PCINT MASK

    inverted_ = format & 0x40;
    half_duplex_ = format & 0x80;
    stop_bits_ = ((format & 0x8) >> 3) + 1;
    parity_ = (format & 0x30) >> 4;

    if (!half_duplex_)
    {
        DDRB |= _BV(DDBx);
        if (inverted_)
            setPinLow;
        else
            setPinHigh;
    }
    else if (!inverted_)
        PORTx |= _BV(PORTxn); // RX PULLUP for half duplex for non inverted signal. For inverted signal, an external pull down resistor maybe needed. Not needed if the other side has one

    // 1 bit delay in 4 clock cycles
    uint16_t delay = (F_CPU / baud) / 4;
    // substract overheads
    tx_delay_start = subs(delay, 14 / 4);
    tx_delay = subs(delay, 19 / 4);
    tx_delay_parity = subs(delay, 14 / 4);
    tx_delay_stop = subs(delay * stop_bits_, (71 + 10) / 4);
    rx_delay = subs(delay, 15 / 4);
    rx_delay_centering = subs(delay / 2, (4 + 4 + 75 + 17 - 15) / 4);
    rx_delay_stop = subs(delay * 3 * stop_bits_ / 4, (37 + 11 + 12) / 4);

    // Set TMR2 to measure ms (max 16Mhz 4ms, 8Mhz 8ms) - shared with sbus
    TCCR2B = _BV(CS22) | _BV(CS21); // SCALER 256
    TCCR2A = 0;
}

#endif