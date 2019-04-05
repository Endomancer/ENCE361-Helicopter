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

static int32_t quadCount;

void initQaud()
//Initialise the port/pins for GPIO with the quadrotor int handler
{
    // Enable port B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);

    //Register the quadrotor int handler with the relevant GPIO pins
    GPIOIntRegister(GPIO_PORTB_BASE, QuadIntHandler); 

    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);

    //initialise quadCount at 0
    resetQuad(); 
}

void resetQuad()
//Reset quadCount to zero
{
    quadCount = 0;
}

uint32_t getQuadAngle()
//Returns quadCount translated into degrees of rotation
{
    return (uint32_t) quadCount * 360 / ROT_COUNT;
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
        value <<= 2;            // bit shifting previous value of value
        value &= 0b1100;        // masking two bits
        value |= GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
        quadCount += lookup[value];      // Iterates quadCount CW or CCW according to the direction in the lookup table


        // Reset qaudCount to 0 if it gets to the end of a full rotation either way
        if (quadCount >= ROT_COUNT) { quadCount -= ROT_COUNT ; } 
        else if (quadCount < 0) { quadCount += ROT_COUNT; }
    }
}