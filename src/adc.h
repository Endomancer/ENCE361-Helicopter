#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>
#include "circBufT.h"
#include "driverlib/adc.h"
#include "FreeRTOS.h"
#include "task.h"

// Calibration task handler
extern TaskHandle_t xCalibrationHandle;

//*****************************************************************************
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//*****************************************************************************
void ADCIntHandler(void);

// Initialise the ADC
void initADC(void);

// Calculate the average value in the circular buffer
uint32_t averageADCVal(void);

// Calculate height as a percentage
int32_t getHeight(void);

// Sample ADC
void vADCTask(void *pvParameters);

void vCalibrationTask(void *pvParameters);

#endif
