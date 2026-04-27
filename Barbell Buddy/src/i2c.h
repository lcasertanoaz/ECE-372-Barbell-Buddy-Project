#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stdint.h>

void InitI2C(void);
void StartI2C_Trans(unsigned char SLA);
void StopI2C_Trans(void);
void Write(unsigned char data);
unsigned char Read_from(unsigned char SLA, unsigned char MEMADDRESS);
unsigned char Read_data(void);

#endif