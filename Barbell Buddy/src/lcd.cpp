#include "lcd.h"
#include "config.h"
#include "timer.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

// Delays for specified number of microseconds for LCD timing
static void lcdDelayUs(unsigned int delay) {
    unsigned int i;
    for (i = 0; i < delay; i++) {
        _delay_us(1);
    }
}

// Sets all LCD control and data pins as outputs and drives them low
void initLCDPins(void) {
    LCD_RS_DDR |= (1 << LCD_RS_BIT);
    LCD_E_DDR  |= (1 << LCD_E_BIT);
    LCD_D4_DDR |= (1 << LCD_D4_BIT);
    LCD_D5_DDR |= (1 << LCD_D5_BIT);
    LCD_D6_DDR |= (1 << LCD_D6_BIT);
    LCD_D7_DDR |= (1 << LCD_D7_BIT);

    LCD_RS_PORT &= ~(1 << LCD_RS_BIT);
    LCD_E_PORT  &= ~(1 << LCD_E_BIT);
    LCD_D4_PORT &= ~(1 << LCD_D4_BIT);
    LCD_D5_PORT &= ~(1 << LCD_D5_BIT);
    LCD_D6_PORT &= ~(1 << LCD_D6_BIT);
    LCD_D7_PORT &= ~(1 << LCD_D7_BIT);
}

// Places low 4 bits of data on LCD data pins D4-D7
static void setLCDDataPins(unsigned char data) {
    // Clear current data bits
    LCD_D4_PORT &= ~(1 << LCD_D4_BIT);
    LCD_D5_PORT &= ~(1 << LCD_D5_BIT);
    LCD_D6_PORT &= ~(1 << LCD_D6_BIT);
    LCD_D7_PORT &= ~(1 << LCD_D7_BIT);

    // Set bits from low part of data
    if (data & 0x01) {
        LCD_D4_PORT |= (1 << LCD_D4_BIT);
    }
    if (data & 0x02) {
        LCD_D5_PORT |= (1 << LCD_D5_BIT);
    }
    if (data & 0x04) {
        LCD_D6_PORT |= (1 << LCD_D6_BIT);
    }
    if (data & 0x08) {
        LCD_D7_PORT |= (1 << LCD_D7_BIT);
    }
}

// Sends a 4-bit command to the LCD with specified delay after execution for timing purposes
void fourBitCommandWithDelay(unsigned char data, unsigned int delay) {
    setLCDDataPins(data & 0x0F);

    // RS low for command
    LCD_RS_PORT &= ~(1 << LCD_RS_BIT);

    // Pulse Enable
    LCD_E_PORT |= (1 << LCD_E_BIT);
    lcdDelayUs(5);
    LCD_E_PORT &= ~(1 << LCD_E_BIT);
    lcdDelayUs(5);

    lcdDelayUs(delay);
}

// Sends an 8-bit command to the LCD by splitting it into two 4-bit parts and sending each part with appropriate timing
void eightBitCommandWithDelay(unsigned char command, unsigned int delay) {
    fourBitCommandWithDelay(command >> 4, 1);
    fourBitCommandWithDelay(command, delay);
}

// Writes a single character to the LCD at the current cursor position
void writeCharacter(unsigned char character) {
    // RS high for data
    LCD_RS_PORT |= (1 << LCD_RS_BIT);

    // Send top 4 bits
    setLCDDataPins((character >> 4) & 0x0F);
    LCD_E_PORT |= (1 << LCD_E_BIT);
    lcdDelayUs(5);
    LCD_E_PORT &= ~(1 << LCD_E_BIT);
    lcdDelayUs(5);

    // Send bottom 4 bits
    setLCDDataPins(character & 0x0F);
    LCD_E_PORT |= (1 << LCD_E_BIT);
    lcdDelayUs(5);
    LCD_E_PORT &= ~(1 << LCD_E_BIT);
    lcdDelayUs(5);

    lcdDelayUs(53);
}

// Writes a null-terminated string to the LCD
void writeString(const char *str) {
    while (*str != '\0') {
        writeCharacter(*str);
        str++;
    }
}

/*
 * Moves the cursor to row/column.
 * row = 0 or 1
 * col = 0 to 15
 */
void moveCursor(unsigned char row, unsigned char col) {
    unsigned char address;

    if (row == 0) {
        address = col;
    }
    else {
        address = 0x40 + col;
    }

    eightBitCommandWithDelay((0x80 | address), 53);
}

// Clears the LCD display
void clearLCD(void) {
    eightBitCommandWithDelay(0x01, 2000);
}

// Initializes LCD using the startup procedure defined in the datasheet
void initLCDProcedure(void) {
    delayMs(100);

    fourBitCommandWithDelay(0x03, 4100);
    fourBitCommandWithDelay(0x03, 500);
    fourBitCommandWithDelay(0x03, 500);
    fourBitCommandWithDelay(0x02, 500);

    // 4-bit mode, 2-line display
    eightBitCommandWithDelay(0x28, 53);

    // Display off
    eightBitCommandWithDelay(0x08, 53);

    // Clear display
    eightBitCommandWithDelay(0x01, 2000);

    // Entry mode set
    eightBitCommandWithDelay(0x06, 53);

    // Display on, cursor off
    eightBitCommandWithDelay(0x0C, 53);
}

// Initializes LCD by setting up pins and running startup procedure
void initLCD(void) {
    initLCDPins();
    initLCDProcedure();
}

// Writes an integer value to the LCD by converting it to a string first
void writeIntLCD(int value) {
    char buffer[12];
    itoa(value, buffer, 10);
    writeString(buffer);
}
