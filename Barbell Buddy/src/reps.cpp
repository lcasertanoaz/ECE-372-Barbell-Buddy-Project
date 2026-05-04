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

void initRepCounter(void) {
    currentState = REP_IDLE;
    repCount = 0;
    filteredZ = 1.0f;
    velocityProxy = 0.0f;
    cooldownTimer = 0;
    loweringConfirmCount = 0;
    liftingConfirmCount = 0;
    peakLiftVelocity = 0.0f;
}

void updateReps(float zAccelG) {
    // Keep gravity positive even if sensor orientation flips
    float absZ = fabsf(zAccelG);

    // Stronger smoothing to reduce jitter
    filteredZ = 0.90f * filteredZ + 0.10f * absZ;

    // Remove resting gravity
    float netG = filteredZ - 1.0f;

    // Simple velocity proxy by integrating net acceleration
    velocityProxy = (velocityProxy + netG) * 0.85f;

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