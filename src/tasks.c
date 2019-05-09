#include "tasks.h"
#include "adc.h"
#include "buttons.h"
#include "clock.h"
#include "display.h"
#include "quad.h"
#include "config.h"

#define STACK_SIZE 64

typedef enum
{
    NORMAL,
    ADC,
    OFF,
    CALIBRATING
} displayState_t;

typedef enum
{
    NONE,
    NEXT_STATE,
    CALIBRATE
} notification_t;

TaskHandle_t xCalibrationHandle = NULL; // Calibration task handler
TaskHandle_t xDisplayHandle = NULL; // Display task handler

void vADCTask(void *pvParameters)
{
    while(1)
    {
        TickType_t xLastWakeTime = xTaskGetTickCount();
        
        ADCProcessorTrigger(ADC_BASE, ADC_SEQUENCE);

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(ADC_SAMPLE_RATE_MS));
    }
}

void vButtonsTask(void *pvParameters)
{
    while (1)
    {
        // Poll buttons
        updateButtons();

        uint8_t switch_state = checkButton(SWITCH);
        
        if (switch_state)
        {
            if (switch_state == PUSHED)
            {
                // TODO set mode to flying/taking off
        
            }
            else if (switch_state == RELEASED)
            {
                // TODO set mode to landing

            }
        }
        else if (checkButton(LEFT) == PUSHED)
        {
            // TODO Rotate 15deg CCW
            
        }
        else if (checkButton(RIGHT) == PUSHED)
        {
            // TODO Rotate 15deg CW

        }
        else if (checkButton(UP) == PUSHED)
        {
            // TODO Increase altitude by 10%
            
        }
        else if (checkButton(DOWN) == PUSHED)
        {
            // TODO Decrease altitude by 10%

        }    

        vTaskDelay(pdMS_TO_TICKS(BUTTON_POLL_RATE_MS));
    }
}

void vCalibrationTask(void *pvParameters)
{
    while(1)
    {
        // Start calibration display task
        xTaskNotify(xDisplayHandle, CALIBRATE, eSetValueWithOverwrite);

        // Wait for circular buffer to fill up
        vTaskDelay(pdMS_TO_TICKS(ADC_SAMPLE_RATE_MS * BUF_SIZE));

        calibrationReference = 0;

        for (uint8_t i = 0; i < CALIBRATION_SAMPLES; i++)
        {
            calibrationReference += averageADCVal();
            vTaskDelay(pdMS_TO_TICKS(CALIBRATION_SAMPLE_RATE_MS));
        }

        calibrationReference /= CALIBRATION_SAMPLES;

        resetQuad();

        // Finish calibration display task
        xTaskNotify(xDisplayHandle, CALIBRATE, eSetValueWithOverwrite);

        // Block task indefinitely until another calibration is requested
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
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
            displayAngle(getQuadAngle());
            break;

        case ADC:
            displayTitle();
            displayMeanVal(averageADCVal());
            break;

        case CALIBRATING:
            displayCalibrating(calibrating);
            // Slow down display refresh rate: only need to display dots 
            vTaskDelay(pdMS_TO_TICKS(CALIBRATING_DOT_RATE_MS - DISPLAY_REFRESH_RATE_MS));
            break;

        default: // Display off
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(DISPLAY_REFRESH_RATE_MS));
    }
}

void createTasks()
{
    // Create tasks
    xTaskCreate(vADCTask, "Sample ADC", STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vButtonsTask, "Poll Buttons", STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vCalibrationTask, "Calibrate", STACK_SIZE, NULL, 1, &xCalibrationHandle);
    xTaskCreate(vDisplayTask, "Display", STACK_SIZE, NULL, 1, &xDisplayHandle);
}