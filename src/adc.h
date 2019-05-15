#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>
#include "circBufT.h"
#include "driverlib/adc.h"

extern uint32_t calibrationReference;

//*****************************************************************************
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//*****************************************************************************
void ADCIntHandler(void);

// Initialise the ADC
void initADC(void);

// Return the average value in the circular buffer
uint32_t averageADCVal(void);

// Calculate height as a percentage
int32_t getHeight(void);

#endif
