#include "display.h"
#include "adc.h"
#include "utils/ustdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#define REFRESH_RATE_MS 50
#define CALIBRATING_DOT_RATE_MS 250
#define DISPLAY_WIDTH 14

typedef enum
{
    NORMAL,
    ADC,
    OFF,
    CALIBRATING
} displayState_t;

// Display task handler
TaskHandle_t xDisplayHandle = NULL;

void displayTitle()
{
    OLEDStringDraw("Milestone 1", 0, 0);
}

// Clear the entire OLED display
void clearDisplay()
{
    OLEDStringDraw("                ", 0, 0);
    OLEDStringDraw("                ", 0, 1);
    OLEDStringDraw("                ", 0, 2);
    OLEDStringDraw("                ", 0, 3);
}

void initDisplay()
{
    // Intialise the Orbit OLED display
    OLEDInitialise();
    
    clearDisplay();
}

void displayAltitude(int16_t altitude)
{
    char string[17]; // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    // number field ensures it is displayed right justified.
    usnprintf(string, sizeof(string), "Altitude = %3d%%", altitude);
    // Update line on display.
    OLEDStringDraw(string, 0, 1);
}

//*****************************************************************************
// Function to display the mean ADC value (10-bit value, note) and sample count.
//*****************************************************************************
void displayMeanVal(int16_t meanVal)
{
    char string[17]; // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    // number field ensures it is displayed right justified.
    usnprintf(string, sizeof(string), "Mean ADC = %4d", meanVal);
    // Update line on display.
    OLEDStringDraw(string, 0, 1);
}

void displayCalibrating(bool calibrating)
{
    const char calString[] = "Calibrating";
    const uint8_t dotStartPos = sizeof(calString) - 1;
    const uint8_t numDots = 3; // Number of dots to display
    static uint8_t dotPos = dotStartPos;

    if (calibrating)
    {
        if (dotPos >= dotStartPos + numDots) {
            // Reset dot position
            dotPos = dotStartPos;
            // Clear dots
            OLEDStringDraw("     ", dotPos, 0);
        }
        else
        {
            OLEDStringDraw(calString, 0, 0);
            OLEDStringDraw(".", dotPos++, 0);
        }
    }
    else
    {
        // Reset dot position after calibrating
        dotPos = dotStartPos;
    }
    
    
}

void vDisplayTask(void *pvParameters)
{
    static displayState_t state = NORMAL;
    static displayState_t prevState = NORMAL;
    static bool calibrating = false;

    while (1)
    {
        notification_t notification = ulTaskNotifyTake(pdTRUE, 0);

        switch (notification)
        {
            case CALIBRATE:
                calibrating = !calibrating;
                clearDisplay();
                
                if (calibrating)
                {
                    // Save display state
                    prevState = state;
                    state = CALIBRATING;
                }
                else
                {
                    // Restore display state
                    state = prevState;
                    // Reset calibration display function
                    displayCalibrating(calibrating);
                }
                break;
            
            case NEXT_STATE:
                // Do not change display state when calibrating
                if (!calibrating)
                {
                    // Cycle through display states: NORMAL -> ADC -> OFF
                    state = state < OFF ? state + 1 : NORMAL;

                    // Clear display and redraw title
                    clearDisplay();
                }
                break;
        
            default: // No action
                break;
        }

        switch (state)
        {
        case NORMAL:
            displayTitle();
            displayAltitude(getHeight());
            break;

        case ADC:
            displayTitle();
            displayMeanVal(averageADCVal());
            break;

        case CALIBRATING:
            displayCalibrating(calibrating);
            // Slow down display refresh rate: only need to display dots 
            vTaskDelay(pdMS_TO_TICKS(CALIBRATING_DOT_RATE_MS - REFRESH_RATE_MS));
            break;

        default: // Display off
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(REFRESH_RATE_MS));
    }
}