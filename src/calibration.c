#include <stdint.h>
#include <stdbool.h>
#include "adc.h"
#include "calibration.h"
#include "config.h"
#include "quad.h"
#include "rotors.h"

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

static bool foundReference = false;

// Initialise the Yaw Reference pin with a falling edge interrupt
void initReference()
{
    // Enable port
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    // Configure pin
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

    // Configure pin as input
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);

    // Configure interrupt
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
    
    // Register interrupt
    GPIOIntRegister(GPIO_PORTC_BASE, RefIntHandler); 
    
    // Enable interrupts
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);

    foundReference = false;
}

// Return true if the reference point has been found
bool referenceFound()
{
    return foundReference;
}

// Return true if the yaw is at the reference point
bool atReference()
{
    return !(GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4) == GPIO_PIN_4);
}

// Find the minimum duty cycle at which the helicopter hovers
bool findThreshold(int16_t* threshold)
{
    bool foundThreshold = false;

    // Find main offset
    if (getHeight() < HEIGHT_THRESHOLD) 
    {
        // Increase the threshold 
        *threshold += 1;
    }
    else
    {
        // set the flag to true, decrease the threshold
        *threshold -= HEIGHT_REDUCTION;
        foundThreshold = true;
    }
    
    return foundThreshold;
}

// Reset the quadrature encoder upon finding the yaw reference and disable the interrupt
void RefIntHandler()
{
    // Reset the quad encoder to 0 
    resetQuad();
    // Update reference flag
    foundReference = true;
    // Disable the interrupt as it is unnecissary from this point on
    GPIOIntDisable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
}