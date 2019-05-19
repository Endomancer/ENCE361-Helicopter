#include "yaw.h"
#include "controller.h"
#include "config.h"
#include "rotors.h"
#include "quad.h"
#include "calibration.h"

static pid_t pidYaw;
static control_states_t state = LANDED;
static int32_t reference;
static uint32_t prevTime;

// Initialse yaw controller
void initYaw()
{
    initController(&pidYaw, 0, 0, 0);
    changeYawMode(LANDED);
    reference = 0;
    prevTime = 0;
}

// Update yaw controller
void updateYaw(uint32_t time)
{
    uint16_t control = 0;
    // Calculate change in time
    uint32_t deltaTime = time - prevTime;
    prevTime = time;

    // Controller state machine
    switch (state)
    {
    case LANDED:
        control = 0;
        break;
    
    case SWEEPING:
        // TODO: Sweeping booty
        if (referenceFound())
        {
        if (pidYaw.reference != 0)
        {
            pidYaw.reference = 0;
        }
        if (referenceFound())
        control = controlUpdate(&pidYaw, getQuadDiff(pidYaw.reference), deltaTime, MAIN_OFFSET);
        }
        break;
    
    case LANDING: // Keep yaw controller running while landing
        pidYaw.reference = 0;
        control = controlUpdate(&pidYaw, getQuadDiff(pidYaw.reference), deltaTime, getMainRotorDuty()*YAW_OFFSET_MULTIPLIER);
    case FLYING:
        control = controlUpdate(&pidYaw, getQuadDiff(pidYaw.reference), deltaTime, getMainRotorDuty()*YAW_OFFSET_MULTIPLIER);
        if (control < MIN_FLYING_DUTY)
            control = MIN_FLYING_DUTY;
        break;
    }

    // Update PWM output
    setTailRotorSpeed(control);
}

// Change yaw controller mode
void changeYawMode(control_states_t newState)
{
    state = newState;

    // TODO: Determine gains
    switch (state)
    {
    case SWEEPING:
        updateGains(&pidYaw,100, 10, 0);
    case LANDED:
        pidYaw.reference = 0;
        break;
    
    case FLYING:
        updateGains(&pidYaw, 100, 10, 0);
        break;
    
    case LANDING:
        updateGains(&pidYaw, 100, 10, 0);
        break;
    }
}

// Increase yaw by 15 degrees (clockwise)
// Only used while flying
void increaseYaw()
{
    if (state == FLYING)
    {
        reference += YAW_INCREMENT;
        if (reference > DEGREES / 2)
            reference -= DEGREES;
        
        pidYaw.reference = ROT_COUNT * reference / DEGREES;
    }
}

// Decrease yaw by 15 degrees (counterclockwise)
// Only used while flying
void decreaseYaw()
{
    if (state == FLYING)
    {
        reference -= YAW_INCREMENT;
        if (reference < -DEGREES / 2)
            reference += DEGREES;
        
        pidYaw.reference = ROT_COUNT * reference / DEGREES;
    }
}