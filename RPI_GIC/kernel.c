#include "gic.h"
#include "uart.h"
#include "armtimer.h"
#include "systimer.h"
#include "framebuffer.h"

void initProgress(void)
{
    drawRect(0, 0, 301, 50, 0x0f, 0);
    drawString(309, 21, "Core 0", 0x0f);

    drawRect(0, 60, 301, 110, 0x0f, 0);
    drawString(309, 81, "Core 1", 0x0f);

    drawRect(0, 120, 301, 170, 0x0f, 0);
    drawString(309, 141, "Core 2", 0x0f);

    drawRect(0, 180, 301, 230, 0x0f, 0);
    drawString(309, 201, "Core 3", 0x0f);
}

void drawProgress(unsigned int core, unsigned int val)
{
    unsigned char col = (core + 1) + ((core + 1) << 4);

    // val should be 0-100
    if (val == 0)
        drawRect(1, (60 * core) + 1, 300, (60 * core) + 49, 0x00, 1);
    if (val > 0)
        drawRect(1, (60 * core) + 1, (val * 3), (60 * core) + 49, col, 1);
}

/*void core3_main(void)
{
    unsigned int core3_val = 0;

    clear_core3(); // Only run once

    while (1)
    {
        wait_msec(0x3FFFF);
        drawProgress(3, core3_val);
        uart_send('3');
        if (core3_val < 100)
            core3_val++;
    }
}

void core1_main(void)
{
    clear_core1(); // Only run once

    drawProgress(1, 50);

    debugstr("Playing on CPU Core #1... ");
    debugstr("done");
    debugcrlf();

    drawProgress(1, 100);
}*/

void core0_main(void)
{
    // the main loop ---------------------------------
    unsigned int core0_val = 0;

    while (1)
    {
        wait_msec(0x100000);
        drawProgress(0, core0_val);
        uart_send('0');
        if (core0_val < 100)
            core0_val++;
    }
}

void main()
{
    // interrupts off/mask
    irq_disable();

    gic_init();

    init_systimer(12000000);
    // init_armtimer(80000);
    // setIRQ(ARMTIMER, TARGET_CPU_0);
    setIRQ(SYSTIMER_COMP3, TARGET_CPU_0);
    setIRQ(AUX, TARGET_CPU_0);

    // IRQs on
    irq_barrier();
    irq_enable();

    //   fb_init();
    uart_init();

    //  initProgress();

    //   unsigned int i = 0;
    //    while (i++ < 30)
    //       debugcrlf();

    // welcome from the uart !!!
    uart_send('b');
    uart_send('o');
    uart_send('o');
    uart_send('\r');
    uart_send('\n');
    // start_core1(core1_main);
    // start_core3(core3_main);
    core0_main();
}
