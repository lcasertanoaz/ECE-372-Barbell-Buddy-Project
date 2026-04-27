#include "switch.h"
#include "config.h"
#include <avr/io.h>

void initSwitchINT0(void) {
    // D2 / PD2 input
    BUTTON_DDR &= ~(1 << BUTTON_BIT);

    // Enable pull-up resistor
    BUTTON_PORT |= (1 << BUTTON_BIT);

    // INT0 on falling edge
    EICRA &= ~(1 << ISC00);
    EICRA |=  (1 << ISC01);
}

void enableSwitchInterrupt(void) {
    EIMSK |= (1 << INT0);
}

void disableSwitchInterrupt(void) {
    EIMSK &= ~(1 << INT0);
}

unsigned char switchPressed(void) {
    return ((BUTTON_PINREG & (1 << BUTTON_BIT)) == 0);
}

void clearSwitchInterruptFlag(void) {
    EIFR |= (1 << INTF0);
}