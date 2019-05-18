#include <stdint.h>
#include "adc.h"
#include "buttons.h"
#include "clock.h"
#include "display.h"
#include "quad.h"
#include "rotors.h"
#include "tasks.h"
#include "uart.h"
#include "controller.h"
#include "altitude.h"
#include "yaw.h"

// Initialise peripherals
void init()
{
    initClock();
    initADC();
    initButtons();
    initDisplay();
    initQaud();
    initRotors();
    initUART();
    initAltitude();
    initYaw();
}

int main(void)
{
    init();

    createTasks();

    // Start scheduler
    vTaskStartScheduler();
}
