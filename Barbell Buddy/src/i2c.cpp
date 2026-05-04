#include "i2c.h"

// Initializes ATmega2560 TWI hardware for I2C communication with the ADXL345 IMU
void InitI2C(void) {
    // Prescaler = 1
    TWSR &= ~((1 << TWPS1) | (1 << TWPS0));

    // SCL frequency near 100 kHz for 16 MHz CPU
    TWBR = 72;

    // Enable TWI hardware
    TWCR = (1 << TWEN);
}

// Starts an I2C transmission by sending a start condition and the slave address with the write bit
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

// Ends current I2C transmission by sending a stop condition
void StopI2C_Trans(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

// Writes a byte of data to the I2C bus and waits for the transmission to complete
void Write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while ((TWCR & (1 << TWINT)) == 0) {
    }
}

// Reads 1 byte of data from the I2C bus and returns it 
uint8_t Read_data(void) {
    // Read one byte and return NACK because this helper reads one byte only
    TWCR = (1 << TWINT) | (1 << TWEN);
    while ((TWCR & (1 << TWINT)) == 0) {
    }
    return TWDR;
}

// Reads a byte of data from the specified slave address and memory address on the I2C bus
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

// Writes 1 byte of data to the specified slave address and memory address on the I2C bus
void Write_to(uint8_t sla, uint8_t memAddress, uint8_t data) {
    StartI2C_Trans(sla);
    Write(memAddress);
    Write(data);
    StopI2C_Trans();
}
