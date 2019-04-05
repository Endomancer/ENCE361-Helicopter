#ifndef QUAD_H
#define QUAD_H

#include <stdint.h>

// Initialise the quadrotor GPIO and interrupt handler
// Interrupts occur when either PB0 or PB1 change state
void initQaud(void);

// Reset the quadrotor slit position to zero
void resetQuad(void);

// Converts the quadrotor slit counter to a value in degrees (of rotation of the helicopter)
uint32_t getQuadAngle(void);

// Update the position counter when an interrupt occurs
// Rotating clockwise increments the position
// Rotating counterclockwise decrements the position
void QuadIntHandler(void);

#endif