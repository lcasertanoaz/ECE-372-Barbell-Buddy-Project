// ------------------------------------------------------------
// Barbell Buddy - First Draft
// ------------------------------------------------------------
// This version is organized in a style similar to your lab code:
//   main.cpp     -> main state machine and top-level logic
//   i2c.cpp/h    -> low-level I2C helper functions
//   imu.cpp/h    -> MPU-6050 setup, calibration, and tilt estimate
//   indicators   -> LEDs and buzzer outputs
//   switch.cpp/h -> calibration pushbutton input
//   timer.cpp/h  -> millisecond delay helper
//   config.h     -> pins, constants, and register definitions
//
// Core behavior:
// 1. Initialize the IMU and calibrate a level starting position.
// 2. Continuously estimate bar tilt angle.
// 3. Show left tilt, level, or right tilt using LEDs.
// 4. Turn on the buzzer if tilt remains too large for several loops.
// 5. Allow recalibration using the pushbutton.
// ------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>

#include "config.h"
#include "i2c.h"
#include "imu.h"
#include "indicators.h"
#include "switch.h"
#include "timer.h"
#include "uart.h"
#include "reps.h"

typedef enum {
    DISPLAY_LEVEL,
    DISPLAY_WARNING,
    ALARM_SILENCED
} MainState;

typedef enum {
    SWITCH_WAIT,
    SWITCH_DEBOUNCE_PRESS,
    SWITCH_WAIT_RELEASE,
    SWITCH_DEBOUNCE_RELEASE
} SwitchState;

volatile unsigned char switchFlag = 0;
volatile unsigned char alarmLatched = 0;

MainState mainState = DISPLAY_LEVEL;
SwitchState switchState = SWITCH_WAIT;

unsigned char warningCounter = 0;

int main(void) {
    float tiltAngleDeg;

    initTimer1();
    initIndicators();
    initSwitchINT4();

    InitI2C();
    initIMU();
    calibrateIMU();

    initUART();
    initRepCounter();
    uartPrint("Barbell Buddy Starting...\r\n");

    sei();
    enableSwitchInterrupt();

    // while (1) {
    //     updateTiltEstimate();
    //     tiltAngleDeg = getTiltAngleDeg();

    //     if (tiltAngleDeg < 0.0f) {
    //         absTiltDeg = -tiltAngleDeg;
    //     } else {
    //         absTiltDeg = tiltAngleDeg;
    //     }

    //     switch (mainState) {
    //         case DISPLAY_LEVEL:
    //             if (absTiltDeg >= WARNING_THRESHOLD_DEG) {
    //                 warningCounter++;

    //                 if (warningCounter >= WARNING_LOOP_COUNT) {
    //                     alarmLatched = 1;
    //                     mainState = DISPLAY_WARNING;
    //                     warningCounter = 0;
    //                 }
    //             } else {
    //                 warningCounter = 0;
    //             }
    //             break;

    //         case DISPLAY_WARNING:
    //             if (alarmLatched == 0) {
    //                 mainState = ALARM_SILENCED;
    //             }
    //             break;

    //         case ALARM_SILENCED:
    //             if (absTiltDeg <= LEVEL_WINDOW_DEG) {
    //                 mainState = DISPLAY_LEVEL;
    //                 warningCounter = 0;
    //             }
    //             break;

    //         default:
    //             mainState = DISPLAY_LEVEL;
    //             warningCounter = 0;
    //             break;
    //     }

    //     if (mainState == DISPLAY_WARNING) {
    //         updateIndicators(tiltAngleDeg, 1);
    //     } else {
    //         updateIndicators(tiltAngleDeg, 0);
    //     }

    //     switch (switchState) {
    //         case SWITCH_WAIT:
    //             if (switchFlag) {
    //                 switchFlag = 0;
    //                 switchState = SWITCH_DEBOUNCE_PRESS;
    //             }
    //             break;

    //         case SWITCH_DEBOUNCE_PRESS:
    //             delayMs(DEBOUNCE_MS);

    //             if (switchPressed()) {
    //                 alarmLatched = 0;
    //                 disableSwitchInterrupt();
    //                 switchState = SWITCH_WAIT_RELEASE;
    //             } else {
    //                 switchState = SWITCH_WAIT;
    //             }
    //             break;

    //         case SWITCH_WAIT_RELEASE:
    //             if (!switchPressed()) {
    //                 switchState = SWITCH_DEBOUNCE_RELEASE;
    //             }
    //             break;

    //         case SWITCH_DEBOUNCE_RELEASE:
    //             delayMs(DEBOUNCE_MS);

    //             if (!switchPressed()) {
    //                 clearSwitchInterruptFlag();
    //                 enableSwitchInterrupt();
    //                 switchState = SWITCH_WAIT;
    //             } else {
    //                 switchState = SWITCH_WAIT_RELEASE;
    //             }
    //             break;

    //         default:
    //             switchState = SWITCH_WAIT;
    //             break;
    //     }

    //     delayMs(LOOP_DELAY_MS);
    // }

    // SIMPLE TESTING, REPLACE WHILE LOOP

    int debugLoopCounter = 0;

    while (1) {
        updateTiltEstimate();
        tiltAngleDeg = getTiltAngleDeg();

        // LED-only testing: always update indicators, never use buzzer
        updateIndicators(tiltAngleDeg, 0);

        float zG = getZAccelG();
        updateReps(zG);

        // Print the Z-acceleration every 50 loops (~500ms)
        debugLoopCounter++;
        if (debugLoopCounter >= 50) {
            uartPrint("Z-Accel (x100): ");
            uartPrintInt((int)(zG * 100));
            uartPrint("\r\n");
            debugLoopCounter = 0;
        }

        delayMs(LOOP_DELAY_MS);
    }

    return 0;
}

ISR(INT4_vect) {
    switchFlag = 1;
}
