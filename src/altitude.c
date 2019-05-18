#include "altitude.h"
#include "adc.h"
#include "controller.h"
#include "config.h"
#include "rotors.h"
#include "yaw.h"

static pid_t pidMain;
static control_states_t state = LANDED;
static int16_t reference = 0;
static uint32_t prevTime = 0;

void initAltitude()
{
    initController(&pidMain, 0, 0, 0);
    changeMode(LANDED);
    reference = 0;
    prevTime = 0;
}

void updateAltitude(uint32_t time)
{
    uint16_t control = 0;

    switch (state)
    {
    case SWEEPING:
    case LANDED:
        control = 0;
        break;
    
    case FLYING:
        control = controlUpdate(&pidMain, getAltitude(), time - prevTime);
        if (control < MIN_FLYING_DUTY)
            control = MIN_FLYING_DUTY;
        break;
    
    case LANDING:
        pidMain.reference -= 1; // TODO : Find appropriate landing speed

        if (pidMain.reference <= 0)
        {
            changeMode(LANDED);
            control = 0;
        }
        else
        {
            control = controlUpdate(&pidMain, getAltitude(), time - prevTime);
        }
        
        break;
    }

    setMainRotorSpeed(control);
    prevTime = time;
}

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
        updateGains(&pidMain, 10, 0, 0);
        break;
    
    case LANDING:
        updateGains(&pidMain, 5, 0, 0);
        break;
    }
}

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