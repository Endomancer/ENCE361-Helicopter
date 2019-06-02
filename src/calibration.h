// *******************************************************
// calibration.h
//
// Utilising the reference indicator of the rotaty encoder to calibrate
// the encoder start position and altitude offset of the helicopter.
// 
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <stdbool.h>
#include <stdint.h>

// Initialise the Yaw Reference pin with a falling edge interrupt
void initReference();

// Return true if the Yaw reference has been found
bool referenceFound();

// Return true if the heicopter is at the yaw reference
bool atReference();

// Find the minimum duty cycle at which the helicopter hovers
bool findThreshold(int16_t* threshold);

// Reset the quadrature encoder upon finding the yaw reference and disable the interrupt
void RefIntHandler();

#endif