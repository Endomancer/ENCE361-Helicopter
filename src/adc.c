// *******************************************************
// adc.c
//
// Support for ADC usage on the Tiva TM4C123 Development Board.
// Specifically using channel 9 to read values from an analog sensor.
// 
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

#include "adc.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"
#include "circBufT.h"
#include "config.h"

#define ADC_BASE ADC0_BASE
#define ADC_SEQUENCE 3
#define ADC_STEP 0
#define ADC_CHANNEL ADC_CTL_CH9

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t circBuf; // Buffer of size BUF_SIZE integers (sample values)
uint32_t calibrationReference;

//*****************************************************************************
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//*****************************************************************************
void ADCIntHandler(void)
{
    uint32_t ulValue;

    // Get the single sample from ADC0. ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC_BASE, ADC_SEQUENCE, &ulValue);

    // Place it in the circular buffer (advancing write index)
    writeCircBuf(&circBuf, ulValue);

    // Clean up, clearing the interrupt
    ADCIntClear(ADC_BASE, ADC_SEQUENCE);
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
    ADCSequenceConfigure(ADC_BASE, ADC_SEQUENCE, ADC_TRIGGER_PROCESSOR, 0);

    // Configure step 0 on sequence 3.  Sample channel 9 (ADC_CTL_CH9) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC_BASE, ADC_SEQUENCE, ADC_STEP, ADC_CHANNEL | ADC_CTL_IE | ADC_CTL_END);

    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC_BASE, ADC_SEQUENCE);

    // Register the interrupt handler
    ADCIntRegister(ADC_BASE, ADC_SEQUENCE, ADCIntHandler);

    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC_BASE, ADC_SEQUENCE);
}

// Trigger ADC sampling process
void triggerADC()
{
    ADCProcessorTrigger(ADC_BASE, ADC_SEQUENCE);
}

// Return the average value in the circular buffer
uint32_t averageADCVal()
{
    return averageCircBuf(&circBuf);
}

// Return the average ADC value relative to the calibrated reference
uint32_t getAltitude()
{
    return (((int32_t)calibrationReference) - averageADCVal());
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