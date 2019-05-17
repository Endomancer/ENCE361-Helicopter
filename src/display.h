#ifndef DISPLAY_H
#define DISPLAY_H

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

void displayTailPWM();

void displayMainPWM();

#endif
