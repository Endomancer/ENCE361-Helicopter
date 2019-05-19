#include "tasks.h"
#include "adc.h"
#include "buttons.h"
#include "clock.h"
#include "display.h"
#include "quad.h"
#include "config.h"
#include "uart.h"
#include "pwm.h"
#include "controller.h"
#include "altitude.h"
#include "yaw.h"

#define STACK_SIZE 64


int32_t desiredAltitude,desiredYaw = 0;
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
TaskHandle_t xUARTHandle = NULL; //UART task handler

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
                changeMode(FLYING); // Start rotors
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
    while(1)
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
                UARTCalibrating(false,false);
            }

            UARTAltitude(getHeight());
            UARTAngle(getQuadAngle());
            UARTTailPWM();
            UARTMainPWM();
            break;
        case CALIBRATING:
            UARTCalibrating(calibrating, initial);
            initial = false;
            vTaskDelay(pdMS_TO_TICKS(CALIBRATING_DOT_RATE_MS - UART_REFRESH_RATE_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(UART_REFRESH_RATE_MS));
    }
}

void vControllerTask(void *pvParameters)
{
    while(1)
    {
        TickType_t xLastWakeTime = xTaskGetTickCount();
        
        updateAltitude(xTaskGetTickCount());
        updateYaw(xTaskGetTickCount());

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CONTROLLER_UPDATE_RATE_MS));
    }
}

void createTasks()
{
    // Create tasks
    xTaskCreate(vADCTask, "Sample ADC", STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vButtonsTask, "Poll Buttons", STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vCalibrationTask, "Calibrate", STACK_SIZE, NULL, 1, &xCalibrationHandle);
    xTaskCreate(vDisplayTask, "Display", STACK_SIZE, NULL, 1, &xDisplayHandle);
    xTaskCreate(vUARTTask, "UART", STACK_SIZE, NULL,1, &xUARTHandle);
    xTaskCreate(vControllerTask, "PID Controllers", STACK_SIZE, NULL, 1, NULL);
}