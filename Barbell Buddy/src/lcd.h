#ifndef LCD_H
#define LCD_H

void initLCDPins(void);
void fourBitCommandWithDelay(unsigned char data, unsigned int delay);
void eightBitCommandWithDelay(unsigned char command, unsigned int delay);
void writeCharacter(unsigned char character);
void writeString(const char *str);
void moveCursor(unsigned char row, unsigned char col);
void clearLCD(void);
void initLCDProcedure(void);
void initLCD(void);
void writeIntLCD(int value);

#endif