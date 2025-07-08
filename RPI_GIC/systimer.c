#include "systimer.h"
static rpi_sys_timer_t *rpiSystemTimer = (rpi_sys_timer_t *)RPI_SYSTIMER_BASE;

void init_systimer(unsigned int time)
{
    rpiSystemTimer->compare3 = time;
    rpiSystemTimer->control_status = 1 << PIT_MASKBIT;
}

rpi_sys_timer_t *get_systimer()
{
    return rpiSystemTimer;
}