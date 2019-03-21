#include "display.h"
#include "utils/ustdlib.h"

void initDisplay(void)
{
    // Intialise the Orbit OLED display
    OLEDInitialise();

    OLEDStringDraw("Milestone 1", 0, 0);
}

// Clear the entire OLED display
void clearDisplay(void)
{
    //OLEDStringDraw ("                ", 0, 0);
    OLEDStringDraw("                ", 0, 1);
    OLEDStringDraw("                ", 0, 2);
    OLEDStringDraw("                ", 0, 3);
}

//*****************************************************************************
// Function to display the mean ADC value (10-bit value, note) and sample count.
//*****************************************************************************
void displayMeanVal(int16_t meanVal)
{
    char string[17]; // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    // number field ensures it is displayed right justified.
    usnprintf(string, sizeof(string), "Mean ADC = %4d", meanVal);
    // Update line on display.
    OLEDStringDraw(string, 0, 1);
}

void displayAltitude(int16_t altitude)
{
    char string[17]; // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    // number field ensures it is displayed right justified.
    usnprintf(string, sizeof(string), "Altitude = %3d%%", altitude);
    // Update line on display.
    OLEDStringDraw(string, 0, 1);
}
