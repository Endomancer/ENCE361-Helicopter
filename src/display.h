#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "OrbitOLEDInterface.h"
#include "FreeRTOS.h"
#include "task.h"

typedef enum
{
    NONE,
    NEXT_STATE,
    CALIBRATE
} notification_t;

// Display task handler
extern TaskHandle_t xDisplayHandle;

// Initialise the display
void initDisplay(void);

// OLED display task
void vDisplayTask(void *pvParameters);

void displayAngle(int16_t angle);

#endif
