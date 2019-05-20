#include <stdint.h>
#include "adc.h"
#include "buttons.h"
#include "clock.h"
#include "display.h"
#include "quad.h"
#include "rotors.h"
#include "tasks.h"
#include "uart.h"
#include "calibration.h"
#include "controller.h"

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
    initReference();
    initController();
}

int main(void)
{
    init();

    createTasks();

    // Start scheduler
    vTaskStartScheduler();
}
