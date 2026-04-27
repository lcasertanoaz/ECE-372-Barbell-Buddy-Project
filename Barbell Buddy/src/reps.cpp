#include "reps.h"
#include "uart.h"
#include <math.h>

typedef enum {
    REP_IDLE,
    REP_LOWERING,
    REP_LIFTING
} RepState;

static RepState currentState = REP_IDLE;
static int repCount = 0;
static float filteredZ = 1.0f; 
static float velocityProxy = 0.0f;
static int cooldownTimer = 0;

void initRepCounter(void) {
    currentState = REP_IDLE;
    repCount = 0;
    filteredZ = 1.0f;
    velocityProxy = 0.0f;
    cooldownTimer = 0;
}

void updateReps(float zAccelG) {
    // Use absolute value so gravity is always positive ~1.0g regardless of sensor flip
    float absZ = fabs(zAccelG);

    // Stronger low-pass filter to smooth out jitter
    filteredZ = 0.85f * filteredZ + 0.15f * absZ;

    // Net acceleration (remove 1G resting gravity)
    float netG = filteredZ - 1.0f;

    // Leaky integrator to estimate velocity. 
    // Accumulates small sustained accelerations, but decays to 0 to prevent drift.
    velocityProxy = (velocityProxy + netG) * 0.90f;

    if (cooldownTimer > 0) {
        cooldownTimer--;
    }

    switch (currentState) {
        case REP_IDLE:
            // Velocity goes negative as bar starts dropping
            if (cooldownTimer == 0 && velocityProxy < -0.05f) { 
                currentState = REP_LOWERING;
            }
            break;

        case REP_LOWERING:
            // Velocity goes positive as bar turns around at the bottom and goes up
            if (velocityProxy > 0.05f) { 
                currentState = REP_LIFTING;
            }
            break;

        case REP_LIFTING:
            // Velocity settles back near zero as bar stops at the top
            if (velocityProxy < 0.02f && velocityProxy > -0.02f) { 
                repCount++;
                uartPrint("Rep Completed! Total Reps: ");
                uartPrintInt(repCount);
                uartPrint("\r\n");
                currentState = REP_IDLE;
                cooldownTimer = 50; // ~500ms cooldown to prevent double counting bounce
            }
            break;
    }
}

int getRepCount(void) {
    return repCount;
}