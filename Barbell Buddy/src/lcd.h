#ifndef LCD_H
#define LCD_H

#include <stdint.h>

void initLCD(void);
void lcdCommand(uint8_t command);
void lcdData(uint8_t data);
void lcdClear(void);
void lcdSetCursor(uint8_t row, uint8_t col);
void lcdPrint(const char* str);
void lcdPrintInt(int value);

#endif
