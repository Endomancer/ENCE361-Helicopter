#include <stdint.h>
#include <stdbool.h>
#include "rotors.h"
#include "quad.h"
#include "calibration.h"
#include "adc.h"

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

static bool foundReference = false;

void initReference()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    // Configure pins
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    // Configure pins as inputs

    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);

    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
        
    GPIOIntRegister(GPIO_PORTC_BASE, RefIntHandler); 
    
    // Enable interrupts
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);

    foundReference = false;
}

bool referenceFound()
{
    return foundReference;
}

bool atReference()
{
    return !(GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4) == GPIO_PIN_4);
}

bool findThreshold(int16_t* threshold)
{
    bool found = false;

    if (getAltitude() <= 10)
    {
        *threshold++;
        found = true;
    }
    return found;
}

void RefIntHandler()
{
    resetQuad();
    foundReference = true;
    GPIOIntDisable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
}