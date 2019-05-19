#include "altitude.h"
#include "adc.h"
#include "controller.h"
#include "config.h"
#include "rotors.h"
#include "yaw.h"
#include "calibration.h"

static pid_t pidMain;
static control_states_t state = LANDED;
static int32_t reference;
static uint32_t prevTime;

// Initialise altitude controller
void initAltitude()
{
    initController(&pidMain, 0, 0, 0);
    changeMode(LANDED);
    reference = 0;
    prevTime = 0;
}

// Update altitude controller
void updateAltitude(uint32_t time)
{
    uint16_t control = 0;
    // Calculate error
    int32_t error = pidMain.reference - getAltitude();
    // Calculate change in time
    uint32_t deltaTime = time - prevTime;
    prevTime = time;

    // Controller state machine
    switch (state)
    {
    case SWEEPING: // TODO
        if (referenceFound())
        {
        if (pidMain.reference != 0)
        {
            pidMain.reference = 0;
            error = pidMain.reference - getAltitude();
        }
        control = controlUpdate(&pidMain, error, deltaTime, MAIN_OFFSET);
        }
    case LANDED:
        control = 0;
        break;
    
    case FLYING:
        control = controlUpdate(&pidMain, error, deltaTime, MAIN_OFFSET);
        if (control < MIN_FLYING_DUTY)
            control = MIN_FLYING_DUTY;
        break;
    
    case LANDING:
        pidMain.reference -= 5; // TODO : Find appropriate landing speed
        if (pidMain.reference <= 0)
        {
            changeMode(LANDED);
            changeYawMode(LANDED);
            control = 0;
        }
        else // Continue running altitude controller until landed
            control = controlUpdate(&pidMain, error, deltaTime, MAIN_OFFSET);
        break;
    }

    // Update PWM output
    setMainRotorSpeed(control);
}

// Change controller mode
void changeMode(control_states_t newState)
{
    state = newState;

    // TODO: Determine gains
    switch (state)
    {
    case SWEEPING:
    case LANDED:
        pidMain.reference = 0;
        break;
    
    case FLYING:
        changeYawMode(FLYING);
        updateGains(&pidMain, 10, 1, 0);
        break;
    
    case LANDING:
        changeYawMode(LANDING);
        updateGains(&pidMain, 10, 0, 0);
        break;
    }
}

// Increase altitude by 10%
// Only used while flying
void increaseAltitude()
{
    if (state == FLYING)
    {
        reference += ALTITUDE_INCREMENT;
        if (reference > PERCENT)
            reference = PERCENT;

        pidMain.reference = MAX_HEIGHT * reference / PERCENT;
    }
}

// Decrease altitude by 10%
// Only used while flying
void decreaseAltitude()
{
    if (state == FLYING)
    {
        reference -= ALTITUDE_INCREMENT;
        if (reference < 0)
            reference = 0;

        pidMain.reference = MAX_HEIGHT * reference / PERCENT;
    }
}