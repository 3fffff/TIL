#ifndef ARMTIMER_H
#define ARMTIMER_H
#include "gpio.h"

#define ARMTIMER_IRQ_SPI 64
/** @brief See the documentation for the ARM side timer (Section 14 of the
    BCM2835 Peripherals PDF) */
#define RPI_ARMTIMER_BASE (PERIPHERAL_BASE + 0xB400UL)

/** @brief 0 : 16-bit counters - 1 : 32-bit counter */
#define RPI_ARMTIMER_CTRL_32BIT (1 << 1)

#define RPI_ARMTIMER_CTRL_PRESCALE_1 (0 << 2)
#define RPI_ARMTIMER_CTRL_PRESCALE_16 (1 << 2)
#define RPI_ARMTIMER_CTRL_PRESCALE_256 (2 << 2)

/** @brief 0 : Timer interrupt disabled - 1 : Timer interrupt enabled */
#define RPI_ARMTIMER_CTRL_INT_ENABLE (1 << 5)
#define RPI_ARMTIMER_CTRL_INT_DISABLE (0 << 5)

/** @brief 0 : Timer disabled - 1 : Timer enabled */
#define RPI_ARMTIMER_CTRL_ENABLE (1 << 7)
#define RPI_ARMTIMER_CTRL_DISABLE (0 << 7)

/** @brief Section 14.2 of the BCM2835 Peripherals documentation details
    the register layout for the ARM side timer */
typedef struct
{

    /** The timer load register sets the time for the timer to count down.
        This value is loaded into the timer value register after the load
        register has been written or if the timer-value register has counted
        down to 0. */
    volatile unsigned int Load;

    /** This register holds the current timer value and is counted down when
        the counter is running. It is counted down each timer clock until the
        value 0 is reached. Then the value register is re-loaded from the
        timer load register and the interrupt pending bit is set. The timer
        count down speed is set by the timer pre-divide register. */
    volatile unsigned int Value;

    /** The standard SP804 timer control register consist of 8 bits but in the
        BCM implementation there are more control bits for the extra features.
        Control bits 0-7 are identical to the SP804 bits, albeit some
        functionality of the SP804 is not implemented. All new control bits
        start from bit 8 upwards. */
    volatile unsigned int Control;

    /** The timer IRQ clear register is write only. When writing this register
        the interrupt-pending bit is cleared. When reading this register it
        returns 0x544D5241 which is the ASCII reversed value for "ARMT". */
    volatile unsigned int IRQClear;

    /** The raw IRQ register is a read-only register. It shows the status of
        the interrupt pending bit. 0 : The interrupt pending bits is clear.
        1 : The interrupt pending bit is set.

        The interrupt pending bits is set each time the value register is
        counted down to zero. The interrupt pending bit can not by itself
        generates interrupts. Interrupts can only be generated if the
        interrupt enable bit is set. */
    volatile unsigned int RAWIRQ;

    /** The masked IRQ register is a read-only register. It shows the status
        of the interrupt signal. It is simply a logical AND of the interrupt
        pending bit and the interrupt enable bit. 0 : Interrupt line not
        asserted. 1 :Interrupt line is asserted, (the interrupt pending and
        the interrupt enable bit are set.)  */
    volatile unsigned int MaskedIRQ;

    /** This register is a copy of the timer load register. The difference is
        that a write to this register does not trigger an immediate reload of
        the timer value register. Instead the timer load register value is
        only accessed if the value register has finished counting down to
        zero. */
    volatile unsigned int Reload;

    /** The Pre-divider register is not present in the SP804. The pre-divider
        register is 10 bits wide and can be written or read from. This
        register has been added as the SP804 expects a 1MHz clock which we do
        not have. Instead the pre-divider takes the APB clock and divides it
        down according to:

        timer_clock = apb_clock/(pre_divider+1)

        The reset value of this register is 0x7D so gives a divide by 126. */
    volatile unsigned int PreDivider;

    /** The free running counter is not present in the SP804. The free running
        counter is a 32 bits wide read only register. The register is enabled
        by setting bit 9 of the Timer control register. The free running
        counter is incremented immediately after it is enabled. The timer can
        not be reset but when enabled, will always increment and roll-over.

        The free running counter is also running from the APB clock and has
        its own clock pre-divider controlled by bits 16-23 of the timer
        control register.

        This register will be halted too if bit 8 of the control register is
        set and the ARM is in Debug Halt mode. */
    volatile unsigned int FreeRunningCounter;

} rpi_arm_timer_t;

rpi_arm_timer_t *get_armtimer();
void init_armtimer(unsigned int time);
#endif