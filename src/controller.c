// *******************************************************
//
// controller.c
//
// Support for PID controllers with various types of inputs
// and functions.
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

#include "controller.h"
#include "adc.h"
#include "calibration.h"
#include "config.h"
#include "pid.h"
#include "quad.h"
#include "rotors.h"

static controller_t pidMain;
static controller_t pidTail;
static control_states_t state = LANDED;
static int32_t referenceMain;
static int32_t referenceTail;
static uint32_t prevTime;
static int16_t offsetMain = 20;

// Internal functions used when updating the controller
void rampMain(uint16_t increment);
void rampTail(uint16_t increment);

// Initialise controller
void initController()
{
    // Initialise PID controllers
    initPID(&pidMain, 0, 0, 0);
    initPID(&pidTail, 0, 0, 0);

    // Default state is landed
    changeMode(LANDED);

    // Initialise globals
    referenceMain = 0;
    referenceTail = 0;
    prevTime = 0;
}

// Update controller
void updateController(uint32_t time)
{
    static bool foundThreshold = false;
    uint32_t currentAltitude = getAltitude();
    uint16_t controlMain = 0;
    uint16_t controlTail = 0;
    // Calculate error
    int32_t errorMain = pidMain.reference - currentAltitude;
    int32_t errorTail = getQuadDiff(pidTail.reference);
    // Calculate change in time
    uint32_t deltaTime = time - prevTime;
    prevTime = time;
    // Calculate offsets
    int32_t offsetTail = getMainRotorDuty() * YAW_OFFSET_MULTIPLIER;

    // Controller state machine
    switch (state)
    {
    case LANDED:
        // Turn off rotors
        controlMain = 0;
        controlTail = 0;
        break;

    case SWEEPING:
        // Check if the reference point has been found
        if (!referenceFound())
        {
            // Check if the main offset has been found
            if (!foundThreshold)
            {
                foundThreshold = findThreshold(&offsetMain);
                controlMain = offsetMain;
            }
            else
            {
                controlMain = updatePID(&pidMain, errorMain, deltaTime, offsetMain * SCALING_FACTOR);
            }
            // Update controllers
            controlTail = updatePID(&pidTail, 20, deltaTime, offsetTail);
            // Clamp duty cycle values
            controlMain = clamp(controlMain, MIN_FLYING_DUTY, 50);
            controlTail = clamp(controlTail, MIN_FLYING_DUTY, 50);
            break;
        }
        else if (!foundThreshold)
        {
            // Continue trying to find the height offset
            foundThreshold = findThreshold(&offsetMain);
            controlMain = offsetMain;
            // Update controllers
            controlTail = updatePID(&pidTail, errorTail, deltaTime, offsetTail);
            // Clamp duty cycle values
            controlMain = clamp(controlMain, MIN_FLYING_DUTY, 50);
            controlTail = clamp(controlTail, MIN_FLYING_DUTY, PERCENT);
            break;
        }
        else
        {
            // Change state to flying
            offsetMain *= SCALING_FACTOR;
            changeMode(FLYING);
            initPID(&pidTail, TAIL_FLYING_P_GAIN, TAIL_FLYING_I_GAIN, TAIL_FLYING_D_GAIN);
            pidTail.reference = getQuad();
        }

    case FLYING:
        // Ramp controller references
        rampMain(1);
        rampTail(3);
        // Update controllers
        controlMain = updatePID(&pidMain, errorMain, deltaTime, offsetMain + currentAltitude);
        controlTail = updatePID(&pidTail, errorTail, deltaTime, offsetTail);
        // Clamp duty cycle values
        controlMain = clamp(controlMain, MIN_FLYING_DUTY, PERCENT);
        controlTail = clamp(controlTail, MIN_FLYING_DUTY, PERCENT);
        break;

    case LANDING:
        // Ramp controller references
        rampMain(1);
        rampTail(5);

        if (pidMain.reference == 0 && errorTail <= 5 && errorTail > -5)
        {
            state = LANDED; // Overide landing lockout
            changeMode(LANDED);
            controlMain = 0;
            referenceMain = 0;
        }
        else // Continue running controllers until landed
        {
            if (pidMain.reference == 0 && pidTail.reference == 0)
            {
                updateGains(&pidTail, TAIL_LANDING_P_GAIN, TAIL_LANDING_I_GAIN, TAIL_LANDING_D_GAIN);
            }
            // Update controllers
            controlMain = updatePID(&pidMain, errorMain, deltaTime, offsetMain);
            controlTail = updatePID(&pidTail, errorTail, deltaTime, offsetTail);
            // Clamp duty cycle values
            controlMain = clamp(controlMain, MIN_FLYING_DUTY, PERCENT);
            controlTail = clamp(controlTail, MIN_FLYING_DUTY, PERCENT);
        }
        break;
    }

    // Update PWM outputs
    setMainRotorSpeed(controlMain);
    setTailRotorSpeed(controlTail);
}

// Change controller mode
void changeMode(control_states_t newState)
{
    // Do not change state while landing
    // The state must be changed manually to "unlock" from landing mode
    if (state != LANDING)
        state = newState;

    // Gain Scheduler
    switch (state)
    {
    case LANDED:
        // Reset references
        pidMain.reference = 0;
        pidTail.reference = 0;
        referenceMain = 0;
        referenceTail = 0;
        break;
    
    case SWEEPING:
        // Change the gains for sweeping
        updateGains(&pidTail, TAIL_SWEEPING_P_GAIN, TAIL_SWEEPING_I_GAIN, TAIL_SWEEPING_D_GAIN);
        break;

    case FLYING:
        // Change the gains for flying
        updateGains(&pidMain, MAIN_FLYING_P_GAIN, MAIN_FLYING_I_GAIN, MAIN_FLYING_D_GAIN);
        updateGains(&pidTail, TAIL_FLYING_P_GAIN, TAIL_FLYING_I_GAIN, TAIL_FLYING_D_GAIN);
        referenceMain = 0;
        referenceTail = 0;
        break;

    case LANDING:
        // Update gains for landing
        updateGains(&pidMain, MAIN_LANDING_P_GAIN, MAIN_LANDING_I_GAIN, MAIN_LANDING_D_GAIN);
        updateGains(&pidTail, TAIL_LANDING_P_GAIN, TAIL_LANDING_I_GAIN, TAIL_LANDING_D_GAIN);
        referenceMain = 0;
        referenceTail = 0;
        break;
    }
}

// Increase altitude by 10%
// Only used while flying
void increaseAltitude()
{
    if (state == FLYING)
    {
        referenceMain += ALTITUDE_INCREMENT;
        referenceMain = clamp(referenceMain, 0, PERCENT);
    }
}

// Decrease altitude by 10%
// Only used while flying
void decreaseAltitude()
{
    if (state == FLYING)
    {
        referenceMain -= ALTITUDE_INCREMENT;
        referenceMain = clamp(referenceMain, 0, PERCENT);
    }
}

// Increase yaw by 15 degrees (clockwise)
// Only used while flying
void increaseYaw()
{
    if (state == FLYING)
    {
        referenceTail += YAW_INCREMENT;
        referenceTail = wrap(referenceTail, DEGREES);
    }
}

// Decrease yaw by 15 degrees (counterclockwise)
// Only used while flying
void decreaseYaw()
{
    if (state == FLYING)
    {
        referenceTail -= YAW_INCREMENT;
        referenceTail = wrap(referenceTail, DEGREES);
    }
}

// Return the reference height as a percentage
int32_t getAltitudeReference()
{
    return referenceMain;
}

// Return the reference angle in degrees
int32_t getYawReference()
{
    return referenceTail;
}

// Ramp the main reference towards the desired reference
void rampMain(uint16_t increment)
{
    int32_t reference = referenceMain * MAX_HEIGHT / PERCENT;
    increment = increment * MAX_HEIGHT / PERCENT;
    ramp(&pidMain, reference, increment);
}

// Ramp the tail reference towards the desired reference
void rampTail(uint16_t increment)
{
    int32_t reference = referenceTail * ROT_COUNT / DEGREES;
    int16_t difference = wrap(reference - pidTail.reference, ROT_COUNT);
    increment = increment * ROT_COUNT / DEGREES;

    // Increment reference towards desired position
    if (difference > increment)
        pidTail.reference += increment;
    else if (-difference > increment)
        pidTail.reference -= increment;
    else
        pidTail.reference = reference;
    
    // Ensure reference is within valid range
    pidTail.reference = wrap(pidTail.reference, ROT_COUNT);
}