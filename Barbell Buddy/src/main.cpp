// ------------------------------------------------------------
// Barbell Buddy - Main Program
// ------------------------------------------------------------
// This version initializes the IMU, LEDs, LCD, UART, and rep counter.
// The LCD displays rep count in real time, while the LEDs show tilt.
// ------------------------------------------------------------

#include <avr/io.h>

#include "config.h"
#include "i2c.h"
#include "imu.h"
#include "indicators.h"
#include "timer.h"
#include "uart.h"
#include "reps.h"
#include "lcd.h"

int main(void) {
    float tiltAngleDeg;
    float zG;
    int currentReps;
    int lastDisplayedRep = -1;
    int debugLoopCounter = 0;

    // -----------------------------
    // Initialization
    // -----------------------------
    initTimer1();
    initIndicators();

    InitI2C();
    initIMU();
    calibrateIMU();

    initUART();
    initRepCounter();

    initLCD();
    clearLCD();

    moveCursor(0, 0);
    writeString("Barbell Buddy");

    moveCursor(1, 0);
    writeString("Reps: 0");

    uartPrint("Barbell Buddy Starting...\r\n");

    // -----------------------------
    // Main loop
    // -----------------------------
    while (1) {
        // Update IMU values
        updateTiltEstimate();
        tiltAngleDeg = getTiltAngleDeg();

        // Show tilt using LEDs only
        updateIndicators(tiltAngleDeg, 0);

        // Update rep counter using Z acceleration
        zG = getZAccelG();
        updateReps(zG);

        // Read current rep count
        currentReps = getRepCount();

        // Update LCD only when rep count changes
        if (currentReps != lastDisplayedRep) {
            moveCursor(1, 0);
            writeString("Reps:      ");
            moveCursor(1, 6);
            writeIntLCD(currentReps);

            uartPrint("Rep Count: ");
            uartPrintInt(currentReps);
            uartPrint("\r\n");

            lastDisplayedRep = currentReps;
        }

        // Optional UART debug every ~500 ms if LOOP_DELAY_MS = 10
        debugLoopCounter++;
        if (debugLoopCounter >= 50) {
            uartPrint("Tilt (x10): ");
            uartPrintInt((int)(tiltAngleDeg * 10));
            uartPrint("  Z-Accel (x100): ");
            uartPrintInt((int)(zG * 100));
            uartPrint("\r\n");
            debugLoopCounter = 0;
        }

        delayMs(LOOP_DELAY_MS);
    }

    return 0;
}