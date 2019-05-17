#include <stdint.h>
#include <stdbool.h>
#include "rotors.h"

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

void initReference()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    // Configure pins
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    // Configure pins as inputs
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);
}

void sweepBooty()
{
    // turn on tail rotor at 40% duty
    enableTailRotor();
    setTailRotorSpeed(40);
    // wait for the reference signal to turn high
    
    while (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4) == GPIO_PIN_4);
    setTailRotorSpeed(0);
}