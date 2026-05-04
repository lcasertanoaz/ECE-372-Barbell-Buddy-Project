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

// Filtered acceleration and simple velocity estimate
static float filteredZ = 1.0f;
static float velocityProxy = 0.0f;

// State tracking
static int cooldownTimer = 0;
static int loweringConfirmCount = 0;
static int liftingConfirmCount = 0;
static float peakLiftVelocity = 0.0f;

// Resets the rep counter state machine and clears all rep-tracking variables
void initRepCounter(void) {
    currentState = REP_IDLE;    // Current state of the rep counting state machine
    repCount = 0;               // Total number of reps counted
    filteredZ = 1.0f;           // Smoothed Z acceleration value to reduce noise
    velocityProxy = 0.0f;       // Motion estimate used to tell if the bar is moving up or down
    cooldownTimer = 0;          // Wait time after rep so small bounce doesn't count as new rep
    loweringConfirmCount = 0;   // Counts how many loops in a row look like lowering phase
    liftingConfirmCount = 0;    // Counts how many loops in a row look like lifting phase
    peakLiftVelocity = 0.0f;    // Largest upward motion seen during current rep
}

// Processes new Z acceleration data to update state machine, counts reps, prints updates over UART
void updateReps(float zAccelG) {
    // Keep gravity positive even if sensor orientation flips
    float absZ = fabsf(zAccelG);

    // Stronger smoothing to reduce jitter
    filteredZ = 0.90f * filteredZ + 0.10f * absZ;

    // Remove resting gravity
    float netG = filteredZ - 1.0f;

    // Simple velocity proxy by integrating net acceleration
    velocityProxy = (velocityProxy + netG) * 0.85f;

    // Cooldown timer to prevent false triggers right after counting a rep
    if (cooldownTimer > 0) {
        cooldownTimer--;
    }

    switch (currentState) {
        case REP_IDLE:
            // Require some negative motion before it's "lowering"
            if (cooldownTimer == 0 && velocityProxy < -0.10f) {
                loweringConfirmCount++;
                if (loweringConfirmCount >= 3) {
                    currentState = REP_LOWERING;
                    loweringConfirmCount = 0;
                    peakLiftVelocity = 0.0f;
                }
            } else {
                loweringConfirmCount = 0;
            }
            break;

        case REP_LOWERING:
            // Require a stronger positive reversal before it's "lifting"
            if (velocityProxy > 0.10f) {
                liftingConfirmCount++;
                if (liftingConfirmCount >= 3) {
                    currentState = REP_LIFTING;
                    liftingConfirmCount = 0;
                }
            } else {
                liftingConfirmCount = 0;
            }
            break;

        case REP_LIFTING:
            // Track how strong the upward phase got
            if (velocityProxy > peakLiftVelocity) {
                peakLiftVelocity = velocityProxy;
            }

            // Only count a rep if:
            // 1. motion has settled back near zero
            // 2. the upward phase was actually strong enough
            if (velocityProxy < 0.02f && velocityProxy > -0.02f &&
                peakLiftVelocity > 0.15f) {

                repCount++;
                uartPrint("Rep Completed! Total Reps: ");
                uartPrintInt(repCount);
                uartPrint("\r\n");

                currentState = REP_IDLE;
                cooldownTimer = 75;   // stricter cooldown
                peakLiftVelocity = 0.0f;
            }
            break;
    }
}

int getRepCount(void) {
    return repCount;
}