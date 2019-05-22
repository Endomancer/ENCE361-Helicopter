#include "controller.h"
#include "config.h"
#include "adc.h"
#include "quad.h"
#include "pid.h"
#include "rotors.h"
#include "calibration.h"

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
    uint16_t controlMain = 0;
    uint16_t controlTail = 0;
    // Calculate error
    int32_t errorMain = pidMain.reference - getAltitude();
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
        controlMain = 0;
        controlTail = 0;
        break;

    case SWEEPING: // TODO
        if (!referenceFound())
        {
            if (!foundThreshold)
                foundThreshold = findThreshold(&offsetMain);
            controlMain = offsetMain;
            controlTail = updatePID(&pidTail, errorTail, deltaTime, offsetTail);
            referenceTail = wrap(referenceTail + 5, DEGREES);
            rampTail(5);
            break;
        }
        else
        {
            offsetMain *= SCALING_FACTOR;
            changeMode(FLYING);
        }

    case FLYING:
        rampMain(3);
        rampTail(3);
        // Update controllers
        controlMain = updatePID(&pidMain, errorMain, deltaTime, offsetMain);
        controlTail = updatePID(&pidTail, errorTail, deltaTime, offsetTail);
        // Clamp duty cycle values
        controlMain = clamp(controlMain, MIN_FLYING_DUTY, PERCENT);
        controlTail = clamp(controlTail, MIN_FLYING_DUTY, PERCENT);
        break;

    case LANDING:
        rampMain(2); // TODO : Find appropriate landing speed
        rampTail(5);

        if (pidMain.reference == 0 && getQuadAngle() == 0)
        {
            changeMode(LANDED);
            controlMain = 0;
        }
        else // Continue running controllers until landed
        {
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
    state = newState;

    // TODO: Determine gains
    switch (state)
    {
    case LANDED:
        referenceMain = 0;
        referenceTail = 0;
        pidMain.reference = 0;
        pidTail.reference = 0;
        break;
    
    case SWEEPING:
        updateGains(&pidTail, 1000, 10, 0);
        break;

    case FLYING:
        updateGains(&pidMain, 65, 14, 0);
        initPID(&pidTail, 800, 25, 0);
        pidMain.reference = 0;
        pidTail.reference = getQuad();
        referenceMain = 0;
        referenceTail = 0;
        break;

    case LANDING:
        updateGains(&pidMain, 50, 10, 0);
        updateGains(&pidTail, 800, 10, 0);
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

        //pidMain.reference = MAX_HEIGHT * referenceMain / PERCENT;
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

        //pidMain.reference = MAX_HEIGHT * referenceMain / PERCENT;
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

        //pidTail.reference = ROT_COUNT * referenceTail / DEGREES;
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

        //pidTail.reference = ROT_COUNT * referenceTail / DEGREES;
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