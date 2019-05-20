#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <stdbool.h>

void initReference();


bool referenceFound();

bool atReference();

void RefIntHandler();
#endif