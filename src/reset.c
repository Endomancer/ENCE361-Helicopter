#include "reset.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

void SoftResetIntHandler();

void initSoftReset()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_6);

    GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_FALLING_EDGE);

    GPIOIntRegister(GPIO_PORTA_BASE, SoftResetIntHandler);

    GPIOIntEnable(GPIO_PORTA_BASE, GPIO_INT_PIN_6);
}

void SoftResetIntHandler()
{
    uint32_t intStatus = GPIOIntStatus(GPIO_PORTA_BASE, true);
    GPIOIntClear(GPIO_PORTA_BASE, intStatus);

    // Reset system if PA6 is pulled low
    if (intStatus & GPIO_INT_PIN_6)
        SysCtlReset();
}