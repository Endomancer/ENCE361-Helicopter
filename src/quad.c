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
{
    // Enable port B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);

    GPIOIntRegister(GPIO_PORTB_BASE, PortBIntHandler);

    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);

    resetQuad();
}

void resetQuad()
{
    quadCount = 0;
}

uint32_t getQuadAngle()
{
    return (uint32_t) quadCount * 360 / ROT_COUNT;
}

void PortBIntHandler()
{
    static uint32_t value = 0;
    static int8_t lookup[] = {
        0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0
    };
    uint32_t intStatus = 0;

    intStatus = GPIOIntStatus(GPIO_PORTB_BASE, true);
    GPIOIntClear(GPIO_PORTB_BASE, intStatus);

    if (intStatus & (GPIO_INT_PIN_0 | GPIO_INT_PIN_1)) {
        value <<= 2;
        value &= 0b1100;
        value |= GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
        quadCount += lookup[value];

        if (quadCount >= ROT_COUNT) {
            quadCount -= ROT_COUNT;
        } else if (quadCount < 0) {
            quadCount += ROT_COUNT;
        }
    }
}