#include "gic.h"
#include "armtimer.h"
#include "systimer.h"
#include "uart.h"
#include "aux_uart.h"
// initialize GIC-400
static gic400_t gic400;
void gic_init(void)
{
    gic400.gicd = (gic400_gicd_t *)(GIC_BASE + 0x1000);
    gic400.gicc = (gic400_gicc_t *)(GIC_BASE + 0x2000);

    /* Disable the controller so we can configure it before it passes any
       interrupts to the CPU */
    gic400.gicd->ctl = GIC400_CTL_DISABLE;

    /* Get the number of interrupt lines implemented in the GIC400 controller */
    int number_of_interrupts = GIC400_TYPE_ITLINESNUMBER_GET(gic400.gicd->type) * 32;

    /* The actual number returned by the ITLINESNUMBER is the number of registered implemented.
       The actual number of interrupt lines available is (ITLINESNUMBER * 32) */
    for (int i = 0; i < number_of_interrupts; i++)
    {
        /* Deal with register sets that have 32-interrupts per register */
        if ((i % 32) == 0)
        {
            /* Disable this block of 32 interrupts, clear the pending and active flags */
            gic400.gicd->icenable[i / 32] = 0xFFFFFFFF;
            gic400.gicd->icpend[i / 32] = 0xFFFFFFFF;
            gic400.gicd->icactive[i / 32] = 0xFFFFFFFF;
        }
        /* Deal with interrupt configuration. The configuration registers are 32-bit wide and
        have 2-bit configuration for each interrupt */
        gic400.gicd->icfg[i / 16] &= (0x3 << (i % 16));
        gic400.gicd->icfg[i / 16] |= (GIC400_ICFG_LEVEL_SENSITIVE << (i % 16));
    }

    gic400.gicd->ctl = GIC400_CTL_ENABLE;

    gic400.gicc->pm = 0xF0;
    gic400.gicc->ctl = GIC400_CTL_ENABLE;
}

void setIRQ(gic400_irq irq, enum gic_core core)
{
    unsigned int n = irq / 32;
    // enable gic distributor
    gic400.gicd->isenable[n] |= 1 << (irq % 32);
    // assign interrupt core
    /* Deal with register sets that have one interrupt per register */
    // for uint32
    // u32 byte_offset = INTID % 4;
    // u32 shift = byte_offset * 8 + core;
    gic400.gicd->ipriority[irq] |= 0xA0;
    gic400.gicd->istargets[irq] |= core;
}

void handle_irq(void)
{
    // get one of the pending "Shared Peripheral Interrupt"
    unsigned int spi = gic400.gicc->iar & 0x2FF;
    uart_send('i');

    if (spi == SYSTIMER_COMP3)
    {
        uart_send('t');
        // next in 5sec
        get_systimer()->compare3 = get_systimer()->counter_lo + 5000000;
        // clear IRQ in System Timer chip
        get_systimer()->control_status = 1 << PIT_MASKBIT;
    }
    else if (spi == AUX)
    {
        char c = mini_uart_recv();
        uart_send(c);
    }
    /*  else if (get_armtimer()->MaskedIRQ == 1)
      {
          uart_send('A');
          get_armtimer()->Load = 8388607;
          get_armtimer()->IRQClear = 1;
      }*/
    // clear the pending
    gic400.gicc->eoi = spi;
}
