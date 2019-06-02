// *********************************************************
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
// Task refresh rates and priorities are defined in config.h
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *********************************************************

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
#include "FreeRTOS.h"
#include "task.h"

// Display states
typedef enum
{
    NORMAL,
    ADC,
    OFF,
    CALIBRATING
} displayState_t;

// Task notification values
typedef enum
{
    NONE,
    NEXT_STATE,
    CALIBRATE
} notification_t;

// FreeRTOS task handles
TaskHandle_t xCalibrationHandle = NULL; // Calibration task handler
TaskHandle_t xDisplayHandle = NULL;     // Display task handler
TaskHandle_t xUARTHandle = NULL;        // UART task handler

// Trigger ADC conversions
void vADCTask(void *pvParameters)
{
    while (1)
    {
        TickType_t xLastWakeTime = xTaskGetTickCount();

        triggerADC(); // Begin ADC conversion

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(ADC_SAMPLE_RATE_MS));
    }
}

// Respond to button events
void vButtonsTask(void *pvParameters)
{
    while (1)
    {
        // Poll buttons
        updateButtons();

        // Save mode switch state
        uint8_t switch_state = checkButton(SWITCH);

        // Mode switch (Switch 1 on the Orbit BoosterPack)
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

        // Left button
        if (checkButton(LEFT) == PUSHED)
        {
            decreaseYaw(); // Rotate 15 degrees counterclockwise
        }

        // Right button
        if (checkButton(RIGHT) == PUSHED)
        {
            increaseYaw(); // Rotate 15 degrees clockwise
        }

        // Up button (Orbit BoosterPack)
        if (checkButton(UP) == PUSHED)
        {
            increaseAltitude(); // Increase altitude by 10%
        }

        // Down button (Orbit BoosterPack)
        if (checkButton(DOWN) == PUSHED)
        {
            decreaseAltitude(); // Decrease altitude by 10%
        }

        vTaskDelay(pdMS_TO_TICKS(BUTTONS_POLL_RATE_MS));
    }
}

// Calibrate the ADC
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

        // Accumulate cirular buffer
        for (uint8_t i = 0; i < CALIBRATION_SAMPLES; i++)
        {
            calibrationReference += averageADCVal();
            vTaskDelay(pdMS_TO_TICKS(CALIBRATION_SAMPLE_RATE_MS));
        }

        // Average buffered ADC values
        calibrationReference /= CALIBRATION_SAMPLES;

        resetQuad();

        // Finish calibration display task
        xTaskNotify(xDisplayHandle, CALIBRATE, eSetValueWithOverwrite);
        xTaskNotify(xUARTHandle, CALIBRATE, eSetValueWithOverwrite);

        // Block task indefinitely until another calibration is requested
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

// Update the PID controller
void vControllerTask(void *pvParameters)
{
    while (1)
    {
        TickType_t xLastWakeTime = xTaskGetTickCount();

        updateController(xTaskGetTickCount()); // Update rotor PID controllers

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CONTROLLER_UPDATE_RATE_MS));
    }
}

// Update the OLED display - displays current helicopter status
void vDisplayTask(void *pvParameters)
{
    static displayState_t state = NORMAL;
    static displayState_t prevState = NORMAL;
    static bool calibrating = false;

    while (1)
    {
        notification_t notification = ulTaskNotifyTake(pdTRUE, 0);

        // Respond to task notification
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

        // Display information on OLED
        switch (state)
        {
        case NORMAL:
            // Display current helicopter status
            displayAltitude(getHeight());
            displayAngle(getQuadAngle());
            displayMainPWM();
            displayTailPWM();
            break;

        case ADC:
            // Display averaged ADC value
            displayTitle();
            displayMeanVal(averageADCVal());
            break;

        case CALIBRATING:
            // Display calibration message
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

// Transmit current helicopter information over UART
void vUARTTask(void *pvParameters)
{
    static displayState_t state = NORMAL;
    static displayState_t prevState = NORMAL;
    static bool calibrating = false;
    static bool initial = true;

    while (1)
    {
        notification_t notification = ulTaskNotifyTake(pdTRUE, 0);

        // Respond to task notification
        switch (notification)
        {
        case CALIBRATE:
            calibrating = !calibrating;

            if (calibrating)
            {
                // Save UART state
                prevState = state;
                state = CALIBRATING;
                // Used to send initial calibration message
                initial = true;
            }
            else
            {
                // Send final calibration message
                UARTCalibrating(false, false);
                // Restore display state
                state = prevState;
            }
            break;

        default: // No action
            break;
        }

        // Send information over UART
        switch (state)
        {
        default: // Select normal mode unless calibrating
        case NORMAL:
            // Transmit helicopter status over UART
            UARTAltitude(getHeight(), getAltitudeReference());
            UARTAngle(getQuadAngle(), getYawReference());
            UARTMainPWM(getMainRotorDuty());
            UARTTailPWM(getTailRotorDuty());
            break;
        
        case CALIBRATING:
            // Display calibration message
            UARTCalibrating(calibrating, initial);
            initial = false;
            // Slow down task
            vTaskDelay(pdMS_TO_TICKS(CALIBRATING_DOT_RATE_MS - UART_REFRESH_RATE_MS));
        }

        vTaskDelay(pdMS_TO_TICKS(UART_REFRESH_RATE_MS));
    }
}

// Create FreeRTOS tasks
void createTasks()
{
    xTaskCreate(vADCTask, "Sample ADC", configMINIMAL_STACK_SIZE, NULL, ADC_TASK_PRIORITY, NULL);
    xTaskCreate(vButtonsTask, "Poll Buttons", configMINIMAL_STACK_SIZE, NULL, BUTTONS_TASK_PRIORITY, NULL);
    xTaskCreate(vCalibrationTask, "Calibrate", configMINIMAL_STACK_SIZE, NULL, CALIBRATION_TASK_PRIORITY, &xCalibrationHandle);
    xTaskCreate(vControllerTask, "PID Controllers", configMINIMAL_STACK_SIZE, NULL, CONTROLLER_TASK_PRIORITY, NULL);
    xTaskCreate(vDisplayTask, "Display", configMINIMAL_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, &xDisplayHandle);
    xTaskCreate(vUARTTask, "UART", configMINIMAL_STACK_SIZE, NULL, UART_TASK_PRIORITY, &xUARTHandle);
}

// Start the FreeRTOS task scheduler
void startScheduler()
{
    vTaskStartScheduler();
}