#include <stdint.h>
#include <stdbool.h>
#include "adc.h"
#include "clock.h"
#include "display.h"
#include <FreeRTOS.h>
#include <task.h>

void vDisplayTask(void *pvParameters)
{
    while(1)
    {
        displayMeanVal(average());

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

int main(void)
{
    // Initialise peripherals
    initClock();
    initADC();
    initDisplay();

    // Create tasks
    xTaskCreate(vDisplayTask, "OLED Display", 64, NULL, 1, NULL);
    xTaskCreate(vADCTask, "Sample ADC", 64, NULL, 1, NULL);

    // Start scheduler
    vTaskStartScheduler();
}