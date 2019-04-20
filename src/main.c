#include <stdint.h>
#include <stdbool.h>
#include "adc.h"
#include "buttons4.h"
#include "clock.h"
#include "display.h"
#include <FreeRTOS.h>
#include <task.h>

int main(void)
{
    // Initialise peripherals
    initClock();
    initADC();
    initButtons();
    initDisplay();
    initQaud();

    // Create tasks
    xTaskCreate(vADCTask, "Sample ADC", 64, NULL, 1, NULL);
    xTaskCreate(vButtonsTask, "Poll Buttons", 64, NULL, 1, NULL);
    xTaskCreate(vCalibrationTask, "Calibrate", 64, NULL, 1, &xCalibrationHandle);
    xTaskCreate(vDisplayTask, "Display", 64, NULL, 1, &xDisplayHandle);

    // Start scheduler
    vTaskStartScheduler();
}
