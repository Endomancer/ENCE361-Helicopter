/**
 * This quadrature decoder implementation concatenates the previous and
 * current input pin values and uses them as the address for a lookup table.
 * The lookup table contains direction values which are accumulated to
 * obtain an angle measurement.
 * Further details can be found at:
 * https://www.circuitsathome.com/mcu/reading-rotary-encoder-on-arduino/
 **/

#ifndef QUAD_H
#define QUAD_H

#include <stdint.h>

// Initialise the quadrotor GPIO and interrupt handler
// Interrupts occur when either PB0 or PB1 change state
void initQaud(void);

// Reset the quadrotor slit position to zero
void resetQuad(void);

// Returns the current angle in degrees
// Values fall between -180 and 179 degrees
int32_t getQuadAngle(void);

// Update the position counter when an interrupt occurs
// Rotating clockwise increments the position
// Rotating counterclockwise decrements the position
void QuadIntHandler(void);

#endif