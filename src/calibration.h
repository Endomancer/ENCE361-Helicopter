#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <stdbool.h>
#include <stdint.h>

void initReference();

bool referenceFound();

bool atReference();

bool findThreshold(int16_t* threshold);

void RefIntHandler();

#endif