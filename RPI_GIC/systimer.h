#ifndef SYS_TIMER_H
#define SYS_TIMER_H

#include "gpio.h"

#define RPI_SYSTIMER_BASE (PERIPHERAL_BASE + 0x3000UL)
#define PIT_STATUS (PERIPHERAL_BASE + 0x00003000)
#define PIT_LOW (PERIPHERAL_BASE + 0x00003004)
#define PIT_Compare3 (PERIPHERAL_BASE + 0x00003018)
#define PIT_MASKBIT 3

typedef struct
{
    volatile unsigned int control_status;
    volatile unsigned int counter_lo;
    volatile unsigned int counter_hi;
    volatile unsigned int compare0;
    volatile unsigned int compare1;
    volatile unsigned int compare2;
    volatile unsigned int compare3;
} rpi_sys_timer_t;
rpi_sys_timer_t *get_systimer();
void init_systimer(unsigned int time);
#endif