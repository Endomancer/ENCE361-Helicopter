#include "quad.h"
#include <stdbool.h>

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

#define SLOT_COUNT (112) // Number of slots in disk
#define ROT_COUNT (SLOT_COUNT * 4) // State changes per rotation

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

// Reset quadPos to zero
void resetQuad()
{
    quadPos = 0;
}

// Returns quadPos translated into degrees of rotation
uint32_t getQuadAngle()
{
    return (uint32_t) quadPos * 360 / ROT_COUNT;
}

void QuadIntHandler()
{
    static uint32_t value = 0;
    uint32_t intStatus = 0;

    // Lookup table of directions (CW (+ve) or CCW (-ve)) according to the change in reference points over sample steps
    static int8_t lookup[] = {
        0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0
    };

    intStatus = GPIOIntStatus(GPIO_PORTB_BASE, true); //Trigger the interrupt when an event occurs
    GPIOIntClear(GPIO_PORTB_BASE, intStatus);

    if (intStatus & (GPIO_INT_PIN_0 | GPIO_INT_PIN_1)) {
        value &= 0b11; // Mask off two rightmost bits (previous value)
        value <<= 2; // Shift previous value left

        // Read encoder pins and concatenate with previous value to form a 4-bit pattern
        value |= GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);

        // Increment/decrement position according to the lookup table
        quadPos += lookup[value];

        // Ensure the position remains within limits [0 to ROT_COUNT]
        quadPos -= quadPos >= ROT_COUNT ? ROT_COUNT : 0;
        quadPos += quadPos < 0 ? ROT_COUNT : 0;
    }
}