#include "kernel.h"
#include "gic.h"
#include "uart.h"
#include "armtimer.h"
#include "systimer.h"

void main()
{
    // interrupts off/mask
    asm volatile("msr daifset, #2");

    gic_init();

    init_systimer(12000000);
    init_armtimer(80000);
    setIRQ(ARMTIMER);
    setIRQ(SYSTIMER_COMP3);

    // IRQs on
    asm volatile("msr daifclr, #2");
    uart_init();

    // welcome from the uart !!!
    uart_send('b');
    uart_send('o');
    uart_send('o');
    uart_send('\r');
    uart_send('\n');

    // the main loop ---------------------------------
    unsigned long long x = 0;
    while (1)
    {
        if (x % 500000 == 0)
        {
            // we are in the main loop
            uart_send('m');
        }
        x++;
    }
}
