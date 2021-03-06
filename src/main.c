// *******************************************************
//
// main.c
//
// The main function of the helicopter project.
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

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
#include "reset.h"

// Initialise peripherals
void init()
{
    initClock();
    initSoftReset();
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
    // Initialise peripherals
    init();

    // Create the required FreeRTOS tasks
    createTasks();

    // Start the FreeRTOS task scheduler
    startScheduler();
}
