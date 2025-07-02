#include "armtimer.h"
static rpi_arm_timer_t *rpiArmTimer = (rpi_arm_timer_t *)RPI_ARMTIMER_BASE;

void init_armtimer(unsigned int time)
{
    rpiArmTimer->Load = time;
    rpiArmTimer->Control =
        RPI_ARMTIMER_CTRL_32BIT |
        RPI_ARMTIMER_CTRL_ENABLE |
        RPI_ARMTIMER_CTRL_INT_ENABLE |
        RPI_ARMTIMER_CTRL_PRESCALE_16;
}

rpi_arm_timer_t *get_armtimer()
{
    return rpiArmTimer;
}