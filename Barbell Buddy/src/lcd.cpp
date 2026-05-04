#include "lcd.h"
#include "config.h"
#include "timer.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

/*
 * Small helper for microsecond delays.
 * We keep this local so the LCD file stays simple and self-contained.
 */
static void lcdDelayUs(unsigned int delay) {
    unsigned int i;
    for (i = 0; i < delay; i++) {
        _delay_us(1);
    }
}

/*
 * Initializes all LCD-related pins as outputs.
 * LCD is used in 4-bit mode:
 * RS, E, D4, D5, D6, D7
 */
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

/*
 * Helper function to place the lower 4 bits of "data"
 * onto LCD D4-D7.
 *
 * bit 0 -> D4
 * bit 1 -> D5
 * bit 2 -> D6
 * bit 3 -> D7
 */
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

/*
 * Sends only 4 bits to the LCD and delays the given number
 * of MICROseconds.
 *
 * This is used during initialization and also by the 8-bit send helpers.
 */
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

/*
 * Sends a full 8-bit command by sending the top part first,
 * then the lower part.
 */
void eightBitCommandWithDelay(unsigned char command, unsigned int delay) {
    fourBitCommandWithDelay(command >> 4, 1);
    fourBitCommandWithDelay(command, delay);
}

/*
 * Writes one character to the LCD.
 * Same idea as eightBitCommandWithDelay, except RS is high.
 */
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

/*
 * Writes a C-string to the LCD.
 */
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

/*
 * Clears the LCD.
 */
void clearLCD(void) {
    eightBitCommandWithDelay(0x01, 2000);
}

/*
 * LCD initialization procedure based on the standard HD44780
 * 4-bit mode startup sequence.
 */
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

/*
 * Initializes LCD pins and then runs the LCD startup procedure.
 */
void initLCD(void) {
    initLCDPins();
    initLCDProcedure();
}

/*
 * Writes an integer to the LCD by converting it to text first.
 */
void writeIntLCD(int value) {
    char buffer[12];
    itoa(value, buffer, 10);
    writeString(buffer);
}
