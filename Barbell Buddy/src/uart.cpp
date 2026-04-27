#include "uart.h"
#include "config.h"
#include <avr/io.h>
#include <stdlib.h>

#define BAUD 9600
#define BRC ((F_CPU/16/BAUD) - 1)

void initUART(void) {
    UBRR0H = (BRC >> 8);
    UBRR0L = BRC;
    UCSR0B = (1 << TXEN0); // Enable Transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data frame
}

void uartTransmit(unsigned char data) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = data;
}

void uartPrint(const char* str) {
    while (*str) {
        uartTransmit(*str++);
    }
}

void uartPrintInt(int val) {
    char buffer[10];
    itoa(val, buffer, 10);
    uartPrint(buffer);
}