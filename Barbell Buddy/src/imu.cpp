#include "imu.h"
#include "i2c.h"
#include "config.h"
#include "timer.h"

#include <stdint.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Latest filtered tilt estimate
static float tiltAngleDeg = 0.0f;

// Calibration values
static float accelOffsetDeg = 0.0f;
static float gyroYBiasDegPerSec = 0.0f;

// -------------------------------------------------
// Private helper functions
// -------------------------------------------------
static void writeMPURegister(uint8_t reg, uint8_t value) {
    StartI2C_Trans(MPU_ADDR);
    Write(reg);
    Write(value);
    StopI2C_Trans();
}

static int16_t readWord(uint8_t regHigh, uint8_t regLow) {
    uint8_t highByte = Read_from(MPU_ADDR, regHigh);
    uint8_t lowByte  = Read_from(MPU_ADDR, regLow);

    return (int16_t)(((uint16_t)highByte << 8) | lowByte);
}

static float computeAccelAngleDeg(int16_t ax, int16_t az) {
    // MPU-6050 default accel scale = +/-2g => 16384 LSB/g
    float axG = (float)ax / 16384.0f;
    float azG = (float)az / 16384.0f;

    // Mounting assumption:
    // X axis roughly along the "tilt direction"
    // Z axis roughly upward when bar is level
    return (float)(atan2(axG, azG) * 180.0 / M_PI);
}

// -------------------------------------------------
// Public IMU functions
// -------------------------------------------------
void initIMU(void) {
    // Wake up MPU-6050
    writeMPURegister(PWR_MGMT_1, 0x00);
    delayMs(100);
}

void calibrateIMU(void) {
    float angleSum = 0.0f;
    float gyroSum = 0.0f;
    int i;

    for (i = 0; i < CALIBRATION_SAMPLES; i++) {
        int16_t ax = readWord(ACCEL_XOUT_H, ACCEL_XOUT_L);
        int16_t az = readWord(ACCEL_ZOUT_H, ACCEL_ZOUT_L);
        int16_t gy = readWord(GYRO_YOUT_H, GYRO_YOUT_L);

        angleSum += computeAccelAngleDeg(ax, az);

        // MPU-6050 default gyro scale = +/-250 deg/s => 131 LSB/(deg/s)
        gyroSum += ((float)gy / 131.0f);

        delayMs(CALIBRATION_DELAY_MS);
    }

    accelOffsetDeg = angleSum / (float)CALIBRATION_SAMPLES;
    gyroYBiasDegPerSec = gyroSum / (float)CALIBRATION_SAMPLES;

    // Current position becomes 0 deg
    tiltAngleDeg = 0.0f;
}

void updateTiltEstimate(void) {
    int16_t ax = readWord(ACCEL_XOUT_H, ACCEL_XOUT_L);
    int16_t az = readWord(ACCEL_ZOUT_H, ACCEL_ZOUT_L);
    int16_t gy = readWord(GYRO_YOUT_H, GYRO_YOUT_L);

    float accelAngleDeg;
    float gyroRateDegPerSec;
    float dt;
    float gyroPredictedAngleDeg;

    accelAngleDeg = computeAccelAngleDeg(ax, az) - accelOffsetDeg;
    gyroRateDegPerSec = ((float)gy / 131.0f) - gyroYBiasDegPerSec;

    dt = ((float)LOOP_DELAY_MS) / 1000.0f;
    gyroPredictedAngleDeg = tiltAngleDeg + (gyroRateDegPerSec * dt);

    tiltAngleDeg =
        (COMPLEMENTARY_ALPHA * gyroPredictedAngleDeg) +
        ((1.0f - COMPLEMENTARY_ALPHA) * accelAngleDeg);
}

float getTiltAngleDeg(void) {
    return tiltAngleDeg;
}