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
static int16_t mainOffset = 20;

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
            
            controlTail = updatePID(&pidTail, 10, deltaTime, offsetTail);
            if (!foundThreshold)
            {
                foundThreshold = findThreshold(&mainOffset);
            }
            controlMain = mainOffset;
            break;
        }
        else
        {
            changeMode(FLYING);
        }

    case FLYING:
        controlMain = updatePID(&pidMain, errorMain, deltaTime, MAIN_OFFSET);
        controlTail = updatePID(&pidTail, errorTail, deltaTime, offsetTail);
        if (controlMain < MIN_FLYING_DUTY)
            controlMain = MIN_FLYING_DUTY;
        if (controlTail < MIN_FLYING_DUTY)
            controlTail = MIN_FLYING_DUTY;
        break;

    case LANDING:
        pidTail.reference = 0;
        pidMain.reference -= 5; // TODO : Find appropriate landing speed
        if (pidMain.reference <= 0)
        {
            changeMode(LANDED);
            controlMain = 0;
        }
        else // Continue running altitude controller until landed
        {
            controlMain = updatePID(&pidMain, errorMain, deltaTime, MAIN_OFFSET);
            controlTail = updatePID(&pidTail, errorTail, deltaTime, offsetTail);
            if (controlMain < MIN_FLYING_DUTY)
                controlMain = MIN_FLYING_DUTY;
            if (controlTail < MIN_FLYING_DUTY)
                controlTail = MIN_FLYING_DUTY;
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
    case SWEEPING:
        updateGains(&pidTail, 1000, 10, 0);
    case LANDED:
        pidMain.reference = 0;
        pidTail.reference = 0;
        break;

    case FLYING:
        updateGains(&pidMain, 65, 14, 0);
        updateGains(&pidTail, 800, 25, 0);
        break;

    case LANDING:
        updateGains(&pidMain, 50, 10, 0);
        updateGains(&pidTail, 800, 10, 0);
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
        if (referenceMain > PERCENT)
            referenceMain = PERCENT;

        pidMain.reference = MAX_HEIGHT * referenceMain / PERCENT;
    }
}

// Decrease altitude by 10%
// Only used while flying
void decreaseAltitude()
{
    if (state == FLYING)
    {
        referenceMain -= ALTITUDE_INCREMENT;
        if (referenceMain < 0)
            referenceMain = 0;

        pidMain.reference = MAX_HEIGHT * referenceMain / PERCENT;
    }
}

// Increase yaw by 15 degrees (clockwise)
// Only used while flying
void increaseYaw()
{
    if (state == FLYING)
    {
        referenceTail += YAW_INCREMENT;
        if (referenceTail > DEGREES / 2)
            referenceTail -= DEGREES;

        pidTail.reference = ROT_COUNT * referenceTail / DEGREES;
    }
}

// Decrease yaw by 15 degrees (counterclockwise)
// Only used while flying
void decreaseYaw()
{
    if (state == FLYING)
    {
        referenceTail -= YAW_INCREMENT;
        if (referenceTail < -DEGREES / 2)
            referenceTail += DEGREES;

        pidTail.reference = ROT_COUNT * referenceTail / DEGREES;
    }
}

int32_t getAltitudeReference()
{
    return referenceMain;
}

int32_t getYawReference()
{
    return referenceTail;
}