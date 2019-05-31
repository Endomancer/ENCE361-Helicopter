#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <stdbool.h>
#include <stdint.h>

// Initialise the Yaw Reference with an falling edge interrupt
void initReference();

// Return true if the Yaw reference has been found
bool referenceFound();

// Return true if the heicopter is at the yaw reference
bool atReference();

// Find the minimum duty cycle that the helicopter hovers at
bool findThreshold(int16_t* threshold);

// Reset the quadrature encoder upon finding the yaw reference, then disable itself
void RefIntHandler();

#endif