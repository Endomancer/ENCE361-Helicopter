#include <stdint.h>
#include "adc.h"
#include "buttons4.h"
#include "clock.h"
#include "display.h"
#include "quad.h"
#include "tasks.h"

// Initialise peripherals
void init()
{
    initClock();
    initADC();
    initButtons();
    initDisplay();
    initQaud();
}

int main(void)
{
    init();

    createTasks();

    // Start scheduler
    vTaskStartScheduler();
}
