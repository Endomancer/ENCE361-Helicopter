// *******************************************************
//
// tasks.c
//
// Support for all tasks required for the project, these are:
//      - ADC
//      - Buttons
//      - Calibration
//      - Controller
//      - OLEDDisplay
//      - UART
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

#include "tasks.h"
#include "adc.h"
#include "buttons.h"
#include "calibration.h"
#include "circBufT.h"
#include "clock.h"
#include "config.h"
#include "controller.h"
#include "display.h"
#include "quad.h"
#include "rotors.h"
#include "uart.h"

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
TaskHandle_t xDisplayHandle = NULL;     // Display task handler
TaskHandle_t xUARTHandle = NULL;        // UART task handler

void vADCTask(void *pvParameters)
{
    while (1)
    {
        TickType_t xLastWakeTime = xTaskGetTickCount();

        triggerADC(); // Begin ADC conversion

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
                if (referenceFound())
                {
                    changeMode(FLYING); // Start flying!
                }
                else
                {
                    changeMode(SWEEPING); // Sweep and find reference
                }
            }
            else if (switch_state == RELEASED)
            {
                changeMode(LANDING); // Land helicopter
            }
        }
        else if (checkButton(LEFT) == PUSHED)
        {
            decreaseYaw(); // Rotate 15 degrees counterclockwise
        }
        else if (checkButton(RIGHT) == PUSHED)
        {
            increaseYaw(); // Rotate 15 degrees clockwise
        }
        else if (checkButton(UP) == PUSHED)
        {
            increaseAltitude(); // Increase altitude by 10%
        }
        else if (checkButton(DOWN) == PUSHED)
        {
            decreaseAltitude(); // Decrease altitude by 10%
        }

        vTaskDelay(pdMS_TO_TICKS(BUTTON_POLL_RATE_MS));
    }
}

void vCalibrationTask(void *pvParameters)
{
    while (1)
    {
        // Start calibration display task
        xTaskNotify(xDisplayHandle, CALIBRATE, eSetValueWithOverwrite);
        xTaskNotify(xUARTHandle, CALIBRATE, eSetValueWithOverwrite);
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
        xTaskNotify(xUARTHandle, CALIBRATE, eSetValueWithOverwrite);
        // Block task indefinitely until another calibration is requested
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

void vControllerTask(void *pvParameters)
{
    while (1)
    {
        TickType_t xLastWakeTime = xTaskGetTickCount();

        updateController(xTaskGetTickCount());

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CONTROLLER_UPDATE_RATE_MS));
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
            displayAltitude(getHeight());
            displayAngle(getQuadAngle());
            displayMainPWM();
            displayTailPWM();
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

void vUARTTask(void *pvParameters)
{
    static displayState_t state = NORMAL;
    static displayState_t prevState = NORMAL;
    static bool calibrating = false;
    bool initial = true;
    while (1)
    {
        // TODO Additional states
        notification_t notification = ulTaskNotifyTake(pdTRUE, 0);

        switch (notification)
        {
        case CALIBRATE:
            calibrating = !calibrating;

            if (calibrating)
            {
                // Save UART state
                prevState = state;
                state = CALIBRATING;
            }
            else
            {
                // Restore display state
                state = prevState;
                // Reset calibration display function
            }
            break;

        default: // No action
            break;
        }
        switch (state)
        {
        default: // makes it work.jpeg
        case NORMAL:
            if (!initial)
            {
                initial = true;
                UARTCalibrating(false, false);
            }

            UARTAltitude(getHeight(), getAltitudeReference());
            UARTAngle(getQuadAngle(), getYawReference());
            UARTMainPWM(getMainRotorDuty());
            UARTTailPWM(getTailRotorDuty());
            break;
        case CALIBRATING:
            UARTCalibrating(calibrating, initial);
            initial = false;
            vTaskDelay(pdMS_TO_TICKS(CALIBRATING_DOT_RATE_MS - UART_REFRESH_RATE_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(UART_REFRESH_RATE_MS));
    }
}

void createTasks()
{
    // Create tasks
    xTaskCreate(vADCTask, "Sample ADC", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    xTaskCreate(vButtonsTask, "Poll Buttons", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(vCalibrationTask, "Calibrate", configMINIMAL_STACK_SIZE, NULL, 2, &xCalibrationHandle);
    xTaskCreate(vControllerTask, "PID Controllers", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(vDisplayTask, "Display", configMINIMAL_STACK_SIZE, NULL, 1, &xDisplayHandle);
    xTaskCreate(vUARTTask, "UART", configMINIMAL_STACK_SIZE, NULL, 1, &xUARTHandle);
}