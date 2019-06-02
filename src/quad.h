// *************************************************************************
//
// quad.h
//
// This quadrature decoder implementation concatenates the previous and
// current input pin values and uses them as the address for a lookup table.
// The lookup table contains direction values which are accumulated to
// obtain an angle measurement.
// Further details can be found at:
// https://www.circuitsathome.com/mcu/reading-rotary-encoder-on-arduino/
// Clockwise corresponds to a positive rotation
// Anticlockwise corresponds to a negative rotation
// The quadrature decoder implementation provided in the TivaWare library
// is not used.
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *************************************************************************

#ifndef QUAD_H
#define QUAD_H

#include <stdint.h>

// Initialise the quadrotor GPIO and interrupt handler
// Interrupts occur when either PB0 or PB1 change state
void initQaud();

// Reset the quadrotor slit position to zero
void resetQuad();

// Returns the current angle in degrees
// Values fall between -180 and 179 degrees
int16_t getQuadAngle();

// Return the full/unscaled quadrature position
// Values fall within -224 and 223
int16_t getQuad();

// Returns and difference between a reference angle and the current position
// A current pos clockwise from the reference returns a positve value
// A current pos counter-clockwise from the reference returns a negative value
int16_t getQuadDiff(int16_t reference);

// Wrap position between -(range / 2) and (range / 2 - 1)
int16_t wrap(int16_t position, uint16_t range);

// Update the position counter when an interrupt occurs
// Rotating clockwise increments the position
// Rotating counterclockwise decrements the position
void QuadIntHandler();

#endif