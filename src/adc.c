#include "adc.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "display.h"
#include "quad.h"
#include "FreeRTOS.h"
#include "task.h"

#define MILLISECONDS 1000
#define ADC_SAMPLE_RATE_HZ 100
#define SAMPLE_RATE_MS (MILLISECONDS / ADC_SAMPLE_RATE_HZ)
#define CALIBRATION_SAMPLES 4
#define CALIBRATION_SAMPLE_RATE_MS 300
#define MAX_HEIGHT 993 // Corresponds to ~ 0.8 V
#define PERCENT 100

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t circBuf; // Buffer of size BUF_SIZE integers (sample values)
static uint32_t calibrationReference;
TaskHandle_t xCalibrationHandle = NULL; // Calibration task handler

//*****************************************************************************
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//*****************************************************************************
void ADCIntHandler(void)
{
    uint32_t ulValue;

    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);

    // Place it in the circular buffer (advancing write index)
    writeCircBuf(&circBuf, ulValue);

    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

void initADC(void)
{
    // Initialise the circular buffer
    initCircBuf(&circBuf, BUF_SIZE);

    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);

    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    // Register the interrupt handler
    ADCIntRegister(ADC0_BASE, 3, ADCIntHandler);

    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

// Calculate the average value in the circular buffer
uint32_t averageADCVal()
{
    uint32_t sum = 0;

    // Accumulate samples
    for (uint8_t i = 0; i < BUF_SIZE; i++)
    {
        sum += readCircBuf(&circBuf);
    }

    // Calculate mean and round value
    return (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;
}

// Calculate height as a percentage
int32_t getHeight()
{
    // Calculate height difference
    int32_t height = (((int32_t)calibrationReference) - averageADCVal());

    // Convert height to percentage
    height = height * PERCENT / MAX_HEIGHT;

    // Ensure height remains between 0% and 100%
    if (height < 0)
    {
        height = 0;
    }
    else if (height > PERCENT)
    {
        height = PERCENT;
    }

    return height;
}

void vADCTask(void *pvParameters)
{
    while(1)
    {
        TickType_t xLastWakeTime = xTaskGetTickCount();
        
        ADCProcessorTrigger(ADC0_BASE, 3);

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SAMPLE_RATE_MS));
    }
}

void vCalibrationTask(void *pvParameters)
{
    while(1)
    {
        // Start calibration display task
        xTaskNotify(xDisplayHandle, CALIBRATE, eSetValueWithOverwrite);

        // Wait for circular buffer to fill up
        vTaskDelay(pdMS_TO_TICKS(SAMPLE_RATE_MS * BUF_SIZE));

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