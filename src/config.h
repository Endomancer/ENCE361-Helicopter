#ifndef COMMON_H
#define COMMON_H

#include "inc/hw_memmap.h" // Contains ADC defines

#define MILLISECONDS 1000
#define PERCENT 100

#define ADC_BASE ADC0_BASE
#define ADC_SEQUENCE 3
#define ADC_STEP 0
#define ADC_CHANNEL ADC_CTL_CH9

#define ADC_SAMPLE_RATE_HZ 100
#define ADC_SAMPLE_RATE_MS (MILLISECONDS / ADC_SAMPLE_RATE_HZ)
#define ADC_RANGE 4096
#define MAX_HEIGHT (ADC_RANGE * 8 / 33) // Corresponds to 0.8 V

#define DEGREES 360
#define SLOT_COUNT 112 // Number of slots in disk
#define ROT_COUNT (SLOT_COUNT * 4) // State changes per rotation

#define UART_REFRESH_RATE_MS 100

#define BUTTON_POLL_RATE_MS 10

#define DISPLAY_REFRESH_RATE_MS 10

#define CALIBRATION_SAMPLES 4
#define CALIBRATION_SAMPLE_RATE_MS 300
#define CALIBRATING_DOT_RATE_MS 300
#define CALIBRATING_NUM_DOTS 5

#define PWM_FREQUENCY 200       // Frequency of main and tail PWM modules
#define ROTOR_DUTY_LIMIT 90     // Maximum allowable duty cycle

#define CONTROLLER_UPDATE_RATE_MS 100
#define CPU_CLOCK_SPEED 20000000

#define ALTITUDE_INCREMENT 10
#define YAW_INCREMENT 15
#define MIN_FLYING_DUTY 2

#define HEIGHT_OF_OFFSET 2
#define MAIN_REDUCTION 12

#define SCALING_FACTOR 1000
// #define MAIN_OFFSET (40 * SCALING_FACTOR)
#define YAW_OFFSET_MULTIPLIER (SCALING_FACTOR * 9 / 10)

#endif