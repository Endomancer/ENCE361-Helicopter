#include "display.h"
#include "config.h"
#include "OrbitOLEDInterface.h"
#include "utils/ustdlib.h"
#include "rotors.h"

void initDisplay()
{
    // Intialise the Orbit OLED display
    OLEDInitialise();
    
    clearDisplay();
}

// Clear the entire OLED display
void clearDisplay()
{
    OLEDStringDraw("                ", 0, 0);
    OLEDStringDraw("                ", 0, 1);
    OLEDStringDraw("                ", 0, 2);
    OLEDStringDraw("                ", 0, 3);
}

// Display a title at the top of the OLED
void displayTitle()
{
    OLEDStringDraw("Milestone 2", 0, 0);
}

// Display a given altitude
void displayAltitude(int16_t altitude)
{
    char string[17]; // Display is 16 characters wide
    usnprintf(string, sizeof(string), "Altitude = %4d%%", altitude);
    OLEDStringDraw(string, 0, 0); // Update line on display
}

// Display a given angle
void displayAngle(int16_t angle)
{
    char string[17]; // Display is 16 characters wide
    // Right align the angle (note that ` displays the ° symbol)
    usnprintf(string, sizeof(string), "Angle    = %4d`", angle);
    OLEDStringDraw(string, 0, 1); // Update line on display
}

// Display the averaged ADC value
void displayMeanVal(int16_t meanVal)
{
    char string[17]; // Display is 16 characters wide
    usnprintf(string, sizeof(string), "Mean ADC = %4d", meanVal);
    OLEDStringDraw(string, 0, 1); // Update line on display
}

// Displays loading bar while calibrating
void displayCalibrating(bool calibrating)
{
    const char calString[] = "Calibrating";
    const uint8_t dotStartPos = sizeof(calString) - 1;
    static uint8_t dotPos = dotStartPos;

    if (calibrating)
    {
        if (dotPos >= dotStartPos + CALIBRATING_NUM_DOTS) {
            // Reset dot position
            dotPos = dotStartPos;
            // Clear dots
            OLEDStringDraw("     ", dotPos, 0);
        }
        else
        {
            OLEDStringDraw(calString, 0, 0);
            OLEDStringDraw(".", dotPos++, 0);
        }
    }
    else
    {
        // Reset dot position after calibrating
        dotPos = dotStartPos;
    }
}

// Display the Tail PWM duty cycle
void displayTailPWM()
{
    char string[17]; // Display is 16 characters wide
    uint16_t duty = getTailRotorDuty();
    usnprintf(string, sizeof(string), "Tail PWM = %4d%%", duty);
    OLEDStringDraw(string, 0, 3); // Update line on display
}

// Display the Main PWM duty cycle
void displayMainPWM()
{
char string[17]; // Display is 16 characters wide
    uint16_t duty = getMainRotorDuty();
    usnprintf(string, sizeof(string), "Main PWM = %4d%%", duty);
    OLEDStringDraw(string, 0, 2); // Update line on display
}