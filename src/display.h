#ifndef DISPLAY_H
#define DISPLAY_H

// *******************************************************
//
// display.h
//
// Support the Tiva/orbit OLED display with functions for
// different states.
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

#include <stdint.h>
#include <stdbool.h>

// Initialise the display
void initDisplay(void);

// Clear the entire OLED display
void clearDisplay(void);

// Display a title at the top of the OLED
void displayTitle(void);

// Display a given altitude
void displayAltitude(int16_t altitude);

// Display a given angle
void displayAngle(int16_t angle);

// Display the averaged ADC value
void displayMeanVal(int16_t meanVal);

// Displays loading bar while calibrating
void displayCalibrating(bool calibrating);

// Display the Tail PWM duty cycle
void displayTailPWM();

// Display the Main PWM duty cycle
void displayMainPWM();

#endif
