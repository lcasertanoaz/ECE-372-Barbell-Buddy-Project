#include "switch.h"
#include "config.h"
#include <avr/io.h>

void initSwitchINT4(void) {
    // D2 / PE4 input
    BUTTON_DDR &= ~(1 << BUTTON_BIT);

    // Enable pull-up resistor
    BUTTON_PORT |= (1 << BUTTON_BIT);

    // INT4 on falling edge
    EICRB &= ~(1 << ISC40);
    EICRB |=  (1 << ISC41);
}

void enableSwitchInterrupt(void) {
    EIMSK |= (1 << INT4);
}

void disableSwitchInterrupt(void) {
    EIMSK &= ~(1 << INT4);
}

unsigned char switchPressed(void) {
    return ((BUTTON_PINREG & (1 << BUTTON_BIT)) == 0);
}

void clearSwitchInterruptFlag(void) {
    EIFR |= (1 << INTF4);
}
