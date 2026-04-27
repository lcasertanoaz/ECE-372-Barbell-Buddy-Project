#ifndef UART_H
#define UART_H

void initUART(void);
void uartTransmit(unsigned char data);
void uartPrint(const char* str);
void uartPrintInt(int val);

#endif