#include "yaw.h"
#include "controller.h"
#include "config.h"
#include "rotors.h"
#include "quad.h"

static pid_t pidYaw;
static control_states_t state = LANDED;
static int16_t reference;
static uint32_t prevTime;

void initYaw()
{
    initController(&pidYaw, 0, 0, 0);
    changeYawMode(LANDED);
    reference = 0;
    prevTime = 0;
}

void updateYaw(uint32_t time)
{
    uint16_t control = 0;

    switch (state)
    {
    case LANDED:
        control = 0;
        break;
    
    case SWEEPING:
        // TODO: Sweeping booty
        break;
    
    case LANDING:
    case FLYING:
        // TODO: Use getQuadDiff once branches have been merged
        control = controlUpdate(&pidYaw, getQuad(), time - prevTime);
        if (control < MIN_FLYING_DUTY)
            control = MIN_FLYING_DUTY;
        break;
        
        break;
    }

    setTailRotorSpeed(control);
    prevTime = time;
}

void changeYawMode(control_states_t newState)
{
    state = newState;

    // TODO: Determine gains
    switch (state)
    {
    case SWEEPING:
    case LANDED:
        pidYaw.reference = 0;
        break;
    
    case FLYING:
        updateGains(&pidYaw, 10, 0, 0);
        break;
    
    case LANDING:
        updateGains(&pidYaw, 5, 0, 0);
        break;
    }
}

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