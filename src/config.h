// *******************************************************
//
// config.h
//
// This file provides easy access to commonly used and
// modified values
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

#ifndef COMMON_H
#define COMMON_H

// Common constants
#define MILLISECONDS 1000
#define PERCENT 100
#define DEGREES 360

// ADC
#define ADC_SAMPLE_RATE_HZ 100
#define ADC_RANGE 4096
#define MAX_HEIGHT (ADC_RANGE * 8 / 33) // Corresponds to 0.8 V

// Quadrature Decoder
#define SLOT_COUNT 112 // Number of slots in disk
#define ROT_COUNT (SLOT_COUNT * 4) // State changes per rotation

// Calibration
#define CALIBRATION_SAMPLES 4
#define CALIBRATING_DOT_RATE_MS 300
#define CALIBRATING_NUM_DOTS 5

// PWM
#define PWM_FREQUENCY 200       // Frequency of main and tail PWM modules
#define ROTOR_DUTY_LIMIT 90     // Maximum allowable duty cycle

// Controller
#define MIN_FLYING_DUTY 2       // Minimum duty cycle while flying
#define SCALING_FACTOR 1000     // Used for integer arithmetic
#define ALTITUDE_INCREMENT 10   // Adjust altitude by 10%
#define YAW_INCREMENT 15        // Adjust angle by 15 degrees
#define YAW_OFFSET_MULTIPLIER (SCALING_FACTOR * 9 / 10)

// Calibration
#define HEIGHT_THRESHOLD 2
#define HEIGHT_REDUCTION 12

// Task Update Rates
#define ADC_SAMPLE_RATE_MS (MILLISECONDS / ADC_SAMPLE_RATE_HZ)
#define BUTTON_POLL_RATE_MS 10
#define CALIBRATION_SAMPLE_RATE_MS 300
#define CONTROLLER_UPDATE_RATE_MS 100
#define DISPLAY_REFRESH_RATE_MS 100
#define UART_REFRESH_RATE_MS 250

#endif