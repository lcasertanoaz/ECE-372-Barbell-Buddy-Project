#ifndef CONFIG_H
#define CONFIG_H

#include <avr/io.h>
#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// -------------------------------------------------
// ATmega2560 / Mega 2560 pin assignments
// -------------------------------------------------
// Left LED  -> D22 = PA0
// Level LED -> D23 = PA1
// Right LED -> D24 = PA2
// Buzzer    -> D5  = PE3 (optional)
// Button    -> D2  = PE4 = INT4
// -------------------------------------------------
#define LEFT_LED_DDR    DDRA
#define LEFT_LED_PORT   PORTA
#define LEFT_LED_BIT    PA0

#define LEVEL_LED_DDR   DDRA
#define LEVEL_LED_PORT  PORTA
#define LEVEL_LED_BIT   PA1

#define RIGHT_LED_DDR   DDRA
#define RIGHT_LED_PORT  PORTA
#define RIGHT_LED_BIT   PA2

#define BUZZER_DDR      DDRE
#define BUZZER_PORT     PORTE
#define BUZZER_BIT      PE3

#define BUTTON_DDR      DDRE
#define BUTTON_PORT     PORTE
#define BUTTON_PINREG   PINE
#define BUTTON_BIT      PE4

// -------------------------------------------------
// ADXL345 I2C settings
// -------------------------------------------------
#define ADXL345_ADDR       0x53

#define REG_DEVID          0x00
#define REG_POWER_CTL      0x2D
#define REG_DATA_FORMAT    0x31
#define REG_BW_RATE        0x2C

#define REG_DATAX0         0x32
#define REG_DATAX1         0x33
#define REG_DATAY0         0x34
#define REG_DATAY1         0x35
#define REG_DATAZ0         0x36
#define REG_DATAZ1         0x37

// -------------------------------------------------
// Timing / debounce
// -------------------------------------------------
#define DEBOUNCE_MS           20
#define LOOP_DELAY_MS         10
#define CALIBRATION_SAMPLES   200
#define CALIBRATION_DELAY_MS  5

// -------------------------------------------------
// Tilt / warning settings
// -------------------------------------------------
#define LEVEL_WINDOW_DEG      10.0f
#define WARNING_THRESHOLD_DEG 12.0f
#define WARNING_LOOP_COUNT    10

#endif
