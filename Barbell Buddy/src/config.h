#ifndef CONFIG_H
#define CONFIG_H

#include <avr/io.h>
#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// -------------------------------------------------
// Uno pin assignments
// D8  = PB0 -> left tilt LED
// D9  = PB1 -> level LED
// D10 = PB2 -> right tilt LED
// D11 = PB3 -> buzzer output
// D2  = PD2 -> pushbutton / INT0
// -------------------------------------------------
#define LEFT_LED_DDR    DDRB
#define LEFT_LED_PORT   PORTB
#define LEFT_LED_BIT    PB0

#define LEVEL_LED_DDR   DDRB
#define LEVEL_LED_PORT  PORTB
#define LEVEL_LED_BIT   PB1

#define RIGHT_LED_DDR   DDRB
#define RIGHT_LED_PORT  PORTB
#define RIGHT_LED_BIT   PB2

#define BUZZER_DDR      DDRB
#define BUZZER_PORT     PORTB
#define BUZZER_BIT      PB3

#define BUTTON_DDR      DDRD
#define BUTTON_PORT     PORTD
#define BUTTON_PINREG   PIND
#define BUTTON_BIT      PD2

// -------------------------------------------------
// ADXL345 I2C settings
// Use 0x53 if ALT ADDRESS / SDO is low
// -------------------------------------------------
#define ADXL345_ADDR       0x53

#define REG_DEVID          0x00
#define REG_POWER_CTL      0x2D
#define REG_DATA_FORMAT    0x31
#define REG_BW_RATE        0x2C

// ADXL345 data registers are little-endian
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
#define WARNING_THRESHOLD_DEG 5.0f
#define WARNING_LOOP_COUNT    10

#endif