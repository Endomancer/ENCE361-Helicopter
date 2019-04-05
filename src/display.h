#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "OrbitOLEDInterface.h"

// Initialise the display
void initDisplay(void);

// Clear the entire OLED display
void clearDisplay(void);

//*****************************************************************************
// Function to display the mean ADC value (10-bit value, note) and sample count.
//*****************************************************************************
void displayMeanVal(int16_t meanVal);

void displayAltitude(int16_t altitude);

void displayAngle(int16_t angle);

#endif
