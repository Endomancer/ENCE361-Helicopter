// *************************************************************************
//
// quad.c
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

#include "quad.h"
#include <stdbool.h>
#include "config.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

static int16_t quadPos;

// Initialise the port/pins for GPIO with the quadrotor int handler
void initQaud()
{
    // Enable port B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    // Configure pins
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    // Configure pins as inputs
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Setup interrupt type
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);
    // Register the quadrotor interrupt handler
    GPIOIntRegister(GPIO_PORTB_BASE, QuadIntHandler); 
    
    // Enable interrupts
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);

    // Initialise quadPos
    resetQuad(); 
}

// Reset position to zero
void resetQuad()
{
    quadPos = 0;
}

// Returns the current angle converted into degrees
int16_t getQuadAngle()
{
    // Convert angle into degrees (between -180 and 179 degrees)
    return (quadPos * DEGREES / ROT_COUNT);
}

// Return the full/unscaled quadrature position
int16_t getQuad()
{
    return quadPos;
}

// Returns and difference between a reference angle and the current position
int16_t getQuadDiff(int16_t reference)
{
    return wrap(reference - quadPos, ROT_COUNT);
}

// Wrap position between -(range / 2) and (range / 2 - 1)
int16_t wrap(int16_t position, uint16_t range)
{
    position = position % range;
    // Ensure position falls within the desired range
    if (position >= range / 2)
        position -= range;
    else if (-position > range / 2)
        position += range;
    
    return position;
}

void QuadIntHandler()
{
    static uint16_t value = 0;
    uint16_t intStatus = 0;

    // Lookup table of directions (CW (+ve) or CCW (-ve)) according to the change in reference points over sample steps
    static int8_t lookup[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

    // Store interrupt status and clear interrupts
    intStatus = GPIOIntStatus(GPIO_PORTB_BASE, true);
    GPIOIntClear(GPIO_PORTB_BASE, intStatus);

    if (intStatus & (GPIO_INT_PIN_0 | GPIO_INT_PIN_1)) {
        value &= 3; // Masks off two rightmost bits (previous value)
        value <<= 2; // Shift previous value left

        // Read encoder pins and concatenate with previous value to form a 4-bit pattern
        value |= GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);

        // Increment/decrement position according to the lookup table
        quadPos += lookup[value];
        quadPos = wrap(quadPos, ROT_COUNT);
    }
}