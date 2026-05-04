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
// MIddle LED -> D23 = PA1
// Right LED -> D24 = PA2
// Buzzer    -> D5  = PE3 (optional)
// Button    -> D2  = PE4 = INT4 (optional)
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
#define DEBOUNCE_MS           20            // not used
#define LOOP_DELAY_MS         10            // Main loop delay in milliseconds
#define CALIBRATION_SAMPLES   200           // Number of samples to average when calibrating IMU
#define CALIBRATION_DELAY_MS  5             // Delay between samples when calibrating IMU

// -------------------------------------------------
// Tilt / warning settings
// -------------------------------------------------
#define LEVEL_WINDOW_DEG      40.0f         // Tilt angle in degrees for which the level LED turns on
#define WARNING_THRESHOLD_DEG 12.0f         // not used
#define WARNING_LOOP_COUNT    10            // not used

// -------------------------------------------------
// LCD1602 pin assignments (4-bit mode, ATmega2560)
// RS -> PC0  (Mega D37)
// E  -> PC1  (Mega D36)
// D4 -> PC2  (Mega D35)
// D5 -> PC3  (Mega D34)
// D6 -> PL0  (Mega D49)
// D7 -> PC5  (Mega D32)
// RW is tied directly to GND, so no MCU pin is needed.
// -------------------------------------------------
#define LCD_RS_DDR     DDRC
#define LCD_RS_PORT    PORTC
#define LCD_RS_BIT     PC0

#define LCD_E_DDR      DDRC
#define LCD_E_PORT     PORTC
#define LCD_E_BIT      PC1

#define LCD_D4_DDR     DDRC
#define LCD_D4_PORT    PORTC
#define LCD_D4_BIT     PC2

#define LCD_D5_DDR     DDRC
#define LCD_D5_PORT    PORTC
#define LCD_D5_BIT     PC3

#define LCD_D6_DDR     DDRL
#define LCD_D6_PORT    PORTL
#define LCD_D6_BIT     PL0

#define LCD_D7_DDR     DDRC
#define LCD_D7_PORT    PORTC
#define LCD_D7_BIT     PC5

#endif
