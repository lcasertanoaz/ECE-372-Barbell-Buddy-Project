#include "i2c.h"

void InitI2C(void) {
    // Prescaler = 1
    TWSR &= ~((1 << TWPS1) | (1 << TWPS0));

    // SCL frequency near 100 kHz for 16 MHz CPU
    TWBR = 72;

    // Enable TWI hardware
    TWCR = (1 << TWEN);
}

void StartI2C_Trans(uint8_t sla) {
    // Send start condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while ((TWCR & (1 << TWINT)) == 0) {
    }

    // Send slave address + write bit
    TWDR = (sla << 1) | 0;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while ((TWCR & (1 << TWINT)) == 0) {
    }
}

void StopI2C_Trans(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void Write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while ((TWCR & (1 << TWINT)) == 0) {
    }
}

uint8_t Read_data(void) {
    // Read one byte and return NACK because this helper reads one byte only
    TWCR = (1 << TWINT) | (1 << TWEN);
    while ((TWCR & (1 << TWINT)) == 0) {
    }
    return TWDR;
}

uint8_t Read_from(uint8_t sla, uint8_t memAddress) {
    uint8_t data;

    StartI2C_Trans(sla);
    Write(memAddress);

    // Repeated start
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while ((TWCR & (1 << TWINT)) == 0) {
    }

    // Send slave address + read bit
    TWDR = (sla << 1) | 1;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while ((TWCR & (1 << TWINT)) == 0) {
    }

    data = Read_data();
    StopI2C_Trans();

    return data;
}

void Write_to(uint8_t sla, uint8_t memAddress, uint8_t data) {
    StartI2C_Trans(sla);
    Write(memAddress);
    Write(data);
    StopI2C_Trans();
}
