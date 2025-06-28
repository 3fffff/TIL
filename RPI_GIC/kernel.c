#include "kernel.h"

// a properly aligned message buffer with: 9x4 byte long mes-
// sage setting feature PL011 UART Clock to 3MHz (and some TAGs and
// sizes of parameter in message)
volatile unsigned int __attribute__((aligned(16))) mbox[9] = {36, 0, 0x38002, 12, 8, 2, 3000000, 0, 0};

unsigned long _regs[38];
static struct uart_t *uart3 = (struct uart_t *)UART3_ADDRESS;
static rpi_sys_timer_t *rpiSystemTimer = (rpi_sys_timer_t *)RPI_SYSTIMER_BASE;
static rpi_arm_timer_t *rpiArmTimer = (rpi_arm_timer_t *)RPI_ARMTIMER_BASE;
static gic400_t gic400;

void uart_send(unsigned int c)
{
    /* wait until we can send */
    do
    {
        asm volatile("nop");
    } while (uart3->FR & 0x20); // while (GET32(UART3_FR) & 0x20);
    /* write the character to the buffer */
    uart3->DR = c;
}

void dispatch(void)
{
    // get one of the pending "Shared Peripheral Interrupt"
    unsigned int spi = gic400.gicc->ia;
    uart_send('i');

    while (spi != GIC_SPURIOUS)
    { // loop until no SPIs are pending on GIC
        if (spi == SYSTIMER_SPI)
        {
            uart_send('t');
            // next in 5sec
            rpiSystemTimer->compare3 = rpiSystemTimer->counter_lo + 5000000;
            // clear IRQ in System Timer chip
            rpiSystemTimer->control_status = 1 << PIT_MASKBIT;
        }
        else if (rpiArmTimer->MaskedIRQ == 1)
        {
            uart_send('A');
            rpiArmTimer->Load = 8388607;
            rpiArmTimer->IRQClear = 1;
        }
        // clear the pending
        gic400.gicc->eoi = spi;
        spi = gic400.gicc->ia;
    }
}

unsigned int mbox_call(unsigned char ch)
{
    // 28-bit address (MSB) and 4-bit value (LSB)
    unsigned int r = ((unsigned int)((unsigned long)&mbox) & ~0xF) | (ch & 0xF);

    // Wait until we can write
    while (GET32(MBOX_STATUS) & MBOX_FULL)
        ;

    // Write the address of our buffer to the mailbox with the channel appended
    PUT32(MBOX_WRITE, r);

    while (1)
    {
        // Is there a reply?
        while (GET32(MBOX_STATUS) & MBOX_EMPTY)
            ;

        // Is it a reply to our message?
        if (r == GET32(MBOX_READ))
            return mbox[1] == MBOX_RESPONSE; // Is it successful?
    }
    return 0;
}

// initialize PL011 UART3 on GPIO4 and 5
void uart_init(void)
{
    // turn off UART3
    uart3->CR = 0;

    mbox_call(0x08);
    // map UART3 to GPIO pins
    register unsigned int r = GET32(GPFSEL0);
    r &= ~((7 << 12) | (7 << 15)); // 000 to gpio4, gpio5
    r |= (3 << 12) | (3 << 15);    // 011 = alt4
    PUT32(GPFSEL0, r);

    // remove pullup or pulldown
    r = GET32(GPPUPPDN0);
    r &= ~((3 << 8) | (3 << 10)); // 11 to gpio4, gpio5 (mask/clear)
    r |= (0 << 8) | (0 << 10);    // 00 = no pullup or down
    PUT32(GPPUPPDN0, r);

    // clear interrupts in PL011 Chip
    uart3->ICR = 0x7FF;

    // Divider = 3000000MHz/(16 * 115200) = 1.627 = ~1.
    // Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
    // 115200 baud = 1 40
    uart3->IBRD = 1;
    uart3->FBRD = 40;

    // Enable FIFO and 8 bit data transmission (1 stop bit, no parity)
    uart3->LCRH = (1 << 6) | (1 << 5) | (1 << 4);
    // enable Tx, Rx, FIFO
    uart3->CR = (1 << 9) | (1 << 8) | (1 << 0);
}

// initialize GIC-400
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

        /* Deal with register sets that have one interrupt per register */
        gic400.gicd->ipriority[i] = 0xA0;
        gic400.gicd->istargets[i] = GIC400_TARGET_CPU0;
    }

    gic400.gicd->ctl = GIC400_CTL_ENABLE;

    gic400.gicc->pm = 0xF0;
    gic400.gicc->ctl = GIC400_CTL_ENABLE;
}

void main()
{
    // interrupts off/mask
    asm volatile("msr daifset, #2");

    gic_init();

    gic400.gicd->isenable[SYSTIMER_SPI / 32] = 1 << (SYSTIMER_SPI % 32);
    rpiSystemTimer->compare3 = 12000000;
    // inital first IRQ in 12sec
    rpiSystemTimer->control_status = 1 << PIT_MASKBIT;

    gic400.gicd->isenable[ARMTIMER_SPI / 32] |= 1 << (ARMTIMER_SPI % 32);
    rpiArmTimer->Load = 888607;
    rpiArmTimer->Control =
        RPI_ARMTIMER_CTRL_32BIT |
        RPI_ARMTIMER_CTRL_ENABLE |
        RPI_ARMTIMER_CTRL_INT_ENABLE |
        RPI_ARMTIMER_CTRL_PRESCALE_16;
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
