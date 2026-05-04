#include "timer.h"

// Configures Timer1 for 1ms timing interval
void initTimer1() {
    // Timer1 in CTC mode
    TCCR1A &= ~(1 << WGM10);
    TCCR1A &= ~(1 << WGM11);
    TCCR1B |=  (1 << WGM12);
    TCCR1B &= ~(1 << WGM13);

    // Prescaler = 64
    TCCR1B |=  (1 << CS10);
    TCCR1B |=  (1 << CS11);
    TCCR1B &= ~(1 << CS12);

    // 16 MHz / 64 = 250 kHz
    // 1 ms = 250 counts -> OCR1A = 249
    OCR1A = 249;
}

// Delays for specified number of milliseconds using Timer1
void delayMs(unsigned int delay) {
    unsigned int delayCnt = 0;

    TCNT1 = 0;
    TIFR1 |= (1 << OCF1A);

    while (delayCnt < delay) {
        if (TIFR1 & (1 << OCF1A)) {
            delayCnt++;
            TIFR1 |= (1 << OCF1A);
        }
    }
}
