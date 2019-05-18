#include "quad.h"
#include <stdbool.h>
#include "config.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

static int32_t quadPos;

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
int32_t getQuadAngle()
{
    // Convert angle into degrees (between -180 and 179 degrees)
    return (quadPos * DEGREES / ROT_COUNT);
}

void QuadIntHandler()
{
    static uint32_t value = 0;
    uint32_t intStatus = 0;

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

        // Ensure position remains between (-ROT_COUNT / 2) to (ROT_COUNT / 2 - 1)
        // The position wraps if limits are exceeded
        quadPos -= quadPos >= (ROT_COUNT / 2) ? ROT_COUNT : 0;
        quadPos += quadPos < (-ROT_COUNT / 2) ? ROT_COUNT : 0;
    }
}