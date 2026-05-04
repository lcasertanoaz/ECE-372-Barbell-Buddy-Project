#include "indicators.h"
#include "config.h"

// Turns all 3 tilt LEDS off
static void allLedsOff(void) {
    LEFT_LED_PORT  &= ~(1 << LEFT_LED_BIT);
    LEVEL_LED_PORT &= ~(1 << LEVEL_LED_BIT);
    RIGHT_LED_PORT &= ~(1 << RIGHT_LED_BIT);
}

// Initializes LED and buzzer pins as outputs and turns them off
void initIndicators(void) {
    LEFT_LED_DDR  |= (1 << LEFT_LED_BIT);
    LEVEL_LED_DDR |= (1 << LEVEL_LED_BIT);
    RIGHT_LED_DDR |= (1 << RIGHT_LED_BIT);
    BUZZER_DDR    |= (1 << BUZZER_BIT);

    allLedsOff();
    BUZZER_PORT &= ~(1 << BUZZER_BIT);
}

// Updates all LEDS based on tilt angle and turns buzzer on/off based on input parameter
void updateIndicators(float tiltAngleDeg, unsigned char buzzerOn) {
    allLedsOff();

    if ((tiltAngleDeg <= LEVEL_WINDOW_DEG) &&
        (tiltAngleDeg >= -LEVEL_WINDOW_DEG)) {
        LEVEL_LED_PORT |= (1 << LEVEL_LED_BIT);
    }
    else if (tiltAngleDeg < -LEVEL_WINDOW_DEG) {
        LEFT_LED_PORT |= (1 << LEFT_LED_BIT);
    }
    else {
        RIGHT_LED_PORT |= (1 << RIGHT_LED_BIT);
    }

    if (buzzerOn) {
        BUZZER_PORT |= (1 << BUZZER_BIT);
    } else {
        BUZZER_PORT &= ~(1 << BUZZER_BIT);
    }
}