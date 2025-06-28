#ifndef __THEKERNEL
#define __THEKERNEL 1

extern unsigned long _regs[38];
extern void dispatch(void);

// implemented in startup.S
extern void PUT32(unsigned int, unsigned int);
extern unsigned int GET32(unsigned int);

unsigned int mbox_call(unsigned char ch);

// Pi4 B, BCM2711, 4MB Base Addresses
#define MMIO_BASE 0xFE000000

// System Timer (and Comparator 3)
#define PIT_STATUS (MMIO_BASE + 0x00003000)
#define PIT_LOW (MMIO_BASE + 0x00003004)
#define PIT_Compare3 (MMIO_BASE + 0x00003018)
#define SYSTIMER_SPI 99
#define ARMTIMER_SPI 64
#define PIT_MASKBIT 3

// IRQ and GIC-400
#define GIC_IRQS 192
#define GIC_SPURIOUS 1023
#define GIC_BASE 0xFF840000
#define GICD_BASE 0xFF841000
#define GICD_CTLR (GICD_BASE + 0x000)
#define GICD_ENABLE (GICD_BASE + 0x100)
#define GICD_DISABLE (GICD_BASE + 0x180)
#define GICD_PEND_CLR (GICD_BASE + 0x280)
#define GICD_ACTIVE_CLR (GICD_BASE + 0x380)
#define GICD_PRIO (GICD_BASE + 0x400)
#define GICD_TARGET (GICD_BASE + 0x800)
#define GICD_CFG (GICD_BASE + 0xc00)
// GIC-400 CPU Interface Controller
#define GICC_BASE 0xFF842000
#define GICC_CTLR (GICC_BASE + 0x000)
#define GICC_PRIO (GICC_BASE + 0x004)
#define GICC_ACK (GICC_BASE + 0x00c)
#define GICC_EOI (GICC_BASE + 0x010)

// GPIOs
#define GPFSEL0 (MMIO_BASE + 0x00200000)
#define GPPUPPDN0 (MMIO_BASE + 0x002000E4)

// GPU messagebox
#define MBOX_READ (MMIO_BASE + 0x0000B880)
#define MBOX_STATUS (MMIO_BASE + 0x0000B898)
#define MBOX_WRITE (MMIO_BASE + 0x0000B8A0)
#define MBOX_RESPONSE 0x80000000
#define MBOX_FULL 0x80000000
#define MBOX_EMPTY 0x40000000
#define RPI_INTERRUPT_CONTROLLER_BASE MMIO_BASE + 0xB200UL
struct uart_t
{
    volatile unsigned int DR; // data register
    volatile unsigned int RSRECR;
    volatile unsigned int RESERV0[4];
    volatile unsigned int FR; // Flag register
    volatile unsigned int RESERV1;
    volatile unsigned int ILPR;
    volatile unsigned int IBRD;  // Integer Baud rate divisor
    volatile unsigned int FBRD;  // Fractional Baud rate divisor
    volatile unsigned int LCRH;  // Line Control register
    volatile unsigned int CR;    // Control register
    volatile unsigned int IFLS;  // Interrupt FIFO Level Select Register
    volatile unsigned int IMCS;  // Interrupt Mask Set Clear Register
    volatile unsigned int RIS;   // Raw Interrupt Status Register
    volatile unsigned int MIS;   // Masked Interrupt Status Register
    volatile unsigned int ICR;   // Interrupt Clear Register
    volatile unsigned int DMACR; // DMA Control Register
    volatile unsigned int ITCR;  // Test Control register
    volatile unsigned int ITIP;  // Integration test input reg
    volatile unsigned int ITOP;  // Integration test output reg
    volatile unsigned int TDR;   // Test Data reg
};
#define UART3_ADDRESS (MMIO_BASE + 0x201600)


/* CoreLink GIC-400 Generic Interrupt Controller */

/* Register bit definitions */
#define GIC400_CTL_ENABLE (1 << 0)
#define GIC400_CTL_DISABLE (0 << 0)

/** @brief Max interrupts that the GIC supports

    Indicates the maximum number of interrupts that the GIC supports. If ITLinesNumber=N, the
    maximum number of interrupts is 32(N+1). The interrupt ID range is from 0 to
    (number of IDs – 1). For example: 0b00011

    Up to 128 interrupt lines, interrupt IDs 0-127.

    The maximum number of interrupts is 1020 ( 0b11111 ). See the text in this section for more
    information.

    Regardless of the range of interrupt IDs defined by this field, interrupt IDs 1020-1023 are
    reserved for special purposes, see Special interrupt numbers on page 3-43 and Interrupt IDs
    on page 2-24.
*/
#define GIC400_TYPE_ITLINESNUMBER_GET(x) (x & 0xF)

/*
    If the GIC implements the Security Extensions, the value of this field is the maximum number
    of implemented lockable SPIs, from 0 ( 0b00000 ) to 31 ( 0b11111 ), see Configuration lockdown
    on page 4-82. If this field is 0b00000 then the GIC does not implement configuration lockdown.

    If the GIC does not implement the Security Extensions, this field is reserved.
*/
#define GIC400_TYPE_LSPI_GET(x) ((x >> 11) & 0x1F)

/*
    Indicates whether the GIC implements the Security Extensions.
    0 - Security Extensions not implemented.
    1 - Security Extensions implemented.
*/
#define GIC400_TYPE_SECURITY_EXTENSIONS_GET(x) ((x >> 10) & 0x1)

/*
    Indicates the number of implemented CPU interfaces. The number of implemented CPU interfaces
    is one more than the value of this field, for example if this field is 0b011 , there are four
    CPU interfaces. If the GIC implements the Virtualization Extensions, this is also the number
    of virtual CPU interfaces.
*/
#define GIC400_TYPE_CPU_NUMBER_GET(x) ((x >> 5) & 0x7)

#define GIC400_TARGET_CPU0 (1 << 0)
#define GIC400_TARGET_CPU1 (1 << 1)
#define GIC400_TARGET_CPU2 (1 << 2)
#define GIC400_TARGET_CPU3 (1 << 3)
#define GIC400_TARGET_CPU4 (1 << 4)
#define GIC400_TARGET_CPU5 (1 << 5)
#define GIC400_TARGET_CPU6 (1 << 6)
#define GIC400_TARGET_CPU7 (1 << 7)

#define GIC400_ICFG_LEVEL_SENSITIVE (0 << 1)
#define GIC400_ICFG_EDGE_TRIGGERED (1 << 1)

typedef struct
{
    volatile unsigned int ctl;
    volatile const unsigned int type;
    volatile const unsigned int iid;
    volatile unsigned int _res0[((0x80 - 0xC) / (sizeof(unsigned int)))];
    volatile unsigned int igroup[((0x100 - 0x80) / (sizeof(unsigned int)))];
    volatile unsigned int isenable[((0x180 - 0x100) / (sizeof(unsigned int)))];
    volatile unsigned int icenable[((0x200 - 0x180) / (sizeof(unsigned int)))];
    volatile unsigned int ispend[((0x280 - 0x200) / (sizeof(unsigned int)))];
    volatile unsigned int icpend[((0x300 - 0x280) / (sizeof(unsigned int)))];
    volatile unsigned int isactive[((0x380 - 0x300) / (sizeof(unsigned int)))];
    volatile unsigned int icactive[((0x400 - 0x380) / (sizeof(unsigned int)))];
    volatile unsigned char ipriority[((0x800 - 0x400) / (sizeof(unsigned char)))];
    volatile unsigned char istargets[((0xC00 - 0x800) / (sizeof(unsigned char)))];
    volatile unsigned int icfg[((0xD00 - 0xC08) / (sizeof(unsigned int)))];
    volatile const unsigned int ppis;
    volatile unsigned int spis[((0xF00 - 0xD04) / (sizeof(unsigned int)))];
    volatile unsigned int sgi;
    volatile unsigned int _res1[((0xF10 - 0xF04) / (sizeof(unsigned int)))];
    volatile unsigned int cpendsgi[((0xF20 - 0xF10) / (sizeof(unsigned int)))];
    volatile unsigned int spendsgi[((0xF30 - 0xF20) / (sizeof(unsigned int)))];
    volatile unsigned int _res2[((0xFD0 - 0xF30) / (sizeof(unsigned int)))];
    volatile const unsigned int pid[((0xFF0 - 0xFD0) / (sizeof(unsigned int)))];
    volatile const unsigned int cid[((0x1000 - 0xFF0) / (sizeof(unsigned int)))];
} gic400_gicd_t;

typedef struct
{
    volatile unsigned int ctl;
    volatile unsigned int pm;
    volatile unsigned int bp;
    volatile const unsigned int ia;
    volatile unsigned int eoi;
    volatile const unsigned int rp;
    volatile const unsigned int hppi;
    volatile unsigned int abp;
    volatile const unsigned int aia;
    volatile unsigned int aeoi;
    volatile const unsigned int ahppi;
    volatile unsigned int _res0[(0xD0 - 0x2C) / sizeof(unsigned int)];
    volatile unsigned int ap;
    volatile unsigned int _res1[(0xE0 - 0xD4) / sizeof(unsigned int)];
    volatile unsigned int nasp;
    volatile unsigned int _res2[(0xFC - 0xE4) / sizeof(unsigned int)];
    volatile const unsigned int iid;
    volatile unsigned int _res3[(0x1000 - 0x100) / sizeof(unsigned int)];
    volatile unsigned int dir;
} gic400_gicc_t;

typedef struct
{
    gic400_gicd_t *gicd;
    gic400_gicc_t *gicc;
} gic400_t;

#define ARMTIMER_IRQ_SPI 64
/** @brief See the documentation for the ARM side timer (Section 14 of the
    BCM2835 Peripherals PDF) */
#define RPI_ARMTIMER_BASE (MMIO_BASE + 0xB400UL)

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

#define RPI_SYSTIMER_BASE (MMIO_BASE + 0x3000UL)

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

typedef struct
{
    volatile unsigned int IRQ_basic_pending;
    volatile unsigned int IRQ_pending_1;
    volatile unsigned int IRQ_pending_2;
    volatile unsigned int FIQ_control;
    volatile unsigned int Enable_IRQs_1;
    volatile unsigned int Enable_IRQs_2;
    volatile unsigned int Enable_Basic_IRQs;
    volatile unsigned int Disable_IRQs_1;
    volatile unsigned int Disable_IRQs_2;
    volatile unsigned int Disable_Basic_IRQs;
} rpi_irq_controller_t;

/** @brief Bits in the Enable_Basic_IRQs register to enable various interrupts.
See the BCM2835 ARM Peripherals manual, section 7.5 */
#define RPI_BASIC_ARM_TIMER_IRQ (1 << 0)
#define RPI_BASIC_ARM_MAILBOX_IRQ (1 << 1)
#define RPI_BASIC_ARM_DOORBELL_0_IRQ (1 << 2)
#define RPI_BASIC_ARM_DOORBELL_1_IRQ (1 << 3)
#define RPI_BASIC_GPU_0_HALTED_IRQ (1 << 4)
#define RPI_BASIC_GPU_1_HALTED_IRQ (1 << 5)
#define RPI_BASIC_ACCESS_ERROR_1_IRQ (1 << 6)
#define RPI_BASIC_ACCESS_ERROR_0_IRQ (1 << 7)

#endif
