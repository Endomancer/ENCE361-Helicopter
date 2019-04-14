#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>
#include "circBufT.h"
#include "driverlib/adc.h"

//*****************************************************************************
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//*****************************************************************************
void ADCIntHandler(void);

// Initialise the ADC
void initADC(void);

// Calculate the average value in the circular buffer
int32_t average();

void vADCTask(void *pvParameters);

#endif
