#include "imu.h"
#include "i2c.h"
#include "config.h"
#include "timer.h"

#include <stdint.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float tiltAngleDeg = 0.0f;
static float accelOffsetDeg = 0.0f;

// -------------------------------------------------
// Private helper functions
// -------------------------------------------------
static void writeADXLRegister(uint8_t reg, uint8_t value) {
    StartI2C_Trans(ADXL345_ADDR);
    Write(reg);
    Write(value);
    StopI2C_Trans();
}

static int16_t readWordLittleEndian(uint8_t regLow, uint8_t regHigh) {
    uint8_t lowByte  = Read_from(ADXL345_ADDR, regLow);
    uint8_t highByte = Read_from(ADXL345_ADDR, regHigh);

    return (int16_t)(((uint16_t)highByte << 8) | lowByte);
}

static float computeAccelTiltDeg(int16_t yRaw, int16_t zRaw) {
    // For ADXL345, a simple first-pass tilt estimate is:
    // roll = atan2(Y, Z)
    return (float)(atan2((float)yRaw, (float)zRaw) * 180.0 / M_PI);
}

// -------------------------------------------------
// Public functions
// -------------------------------------------------
void initIMU(void) {
    // Put ADXL345 into measurement mode
    writeADXLRegister(REG_POWER_CTL, 0x08);

    // Optional but recommended:
    // 100 Hz output data rate
    writeADXLRegister(REG_BW_RATE, 0x0A);

    // Optional but recommended:
    // full-resolution, +/-2g range
    writeADXLRegister(REG_DATA_FORMAT, 0x08);

    delayMs(100);
}

void calibrateIMU(void) {
    float angleSum = 0.0f;
    int i;

    for (i = 0; i < CALIBRATION_SAMPLES; i++) {
        int16_t yRaw = readWordLittleEndian(REG_DATAY0, REG_DATAY1);
        int16_t zRaw = readWordLittleEndian(REG_DATAZ0, REG_DATAZ1);

        angleSum += computeAccelTiltDeg(yRaw, zRaw);

        delayMs(CALIBRATION_DELAY_MS);
    }

    accelOffsetDeg = angleSum / (float)CALIBRATION_SAMPLES;
    tiltAngleDeg = 0.0f;
}

void updateTiltEstimate(void) {
    int16_t yRaw = readWordLittleEndian(REG_DATAY0, REG_DATAY1);
    int16_t zRaw = readWordLittleEndian(REG_DATAZ0, REG_DATAZ1);

    float rawTiltDeg;
    rawTiltDeg = computeAccelTiltDeg(yRaw, zRaw) - accelOffsetDeg;

    // Simple smoothing to reduce flicker
    tiltAngleDeg = 0.85f * tiltAngleDeg + 0.15f * rawTiltDeg;
}

float getTiltAngleDeg(void) {
    return tiltAngleDeg;
}

float getZAccelG(void) {
    int16_t zRaw = readWordLittleEndian(REG_DATAZ0, REG_DATAZ1);
    // ADXL345 full resolution mode scale factor is ~3.9mg per LSB
    return (float)zRaw * 0.0039f;
}