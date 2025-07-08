#ifndef GPIO_H
#define GPIO_H

// implemented in startup.S
extern void PUT32(unsigned int, unsigned int);
extern unsigned int GET32(unsigned int);
extern void irq_disable();
extern void irq_enable();
extern void irq_barrier();

#define PERIPHERAL_BASE 0xFE000000
/** The base address of the GPIO peripheral (ARM Physical Address) */
#define RPI_GPIO_BASE (PERIPHERAL_BASE + 0x200000UL)

/* Different Raspberry pi models have the ACT LED on different GPIO pins. The RPI3 doesn't have
   access to the ACT LED through a GPIO pin can so can't be used in this tutorial, but the RPI3B+
   does have the ACT LED on a GPIO pin again and so can be used with this tutorial! */

#define LED_ON()                                        \
    do                                                  \
    {                                                   \
        RPI_GetGpio()->LED_GPSET = (1 << LED_GPIO_BIT); \
    } while (0)
#define LED_OFF()                                       \
    do                                                  \
    {                                                   \
        RPI_GetGpio()->LED_GPCLR = (1 << LED_GPIO_BIT); \
    } while (0)

typedef enum
{
    FS_INPUT = 0,
    FS_OUTPUT,
    FS_ALT5,
    FS_ALT4,
    FS_ALT0,
    FS_ALT1,
    FS_ALT2,
    FS_ALT3,
} rpi_gpio_alt_function_t;

typedef enum
{
    PULL_NONE = 0,
    PULL_UP = 1,
    PULL_DOWN = 2
} rpi_gpio_pull;

/* A mask to be able to clear the bits in the register before setting the
   value we require */
#define FS_MASK (7)
#define PULL_MASK (3)

typedef enum
{
    RPI_GPIO0 = 0,
    RPI_GPIO1,
    RPI_GPIO2,
    RPI_GPIO3,
    RPI_GPIO4,
    RPI_GPIO5,
    RPI_GPIO6,
    RPI_GPIO7,
    RPI_GPIO8,
    RPI_GPIO9,
    RPI_GPIO10 = 10,
    RPI_GPIO11,
    RPI_GPIO12,
    RPI_GPIO13,
    RPI_GPIO14,
    RPI_GPIO15,
    RPI_GPIO16,
    RPI_GPIO17,
    RPI_GPIO18,
    RPI_GPIO19,
    RPI_GPIO20 = 20,
    RPI_GPIO21,
    RPI_GPIO22,
    RPI_GPIO23,
    RPI_GPIO24,
    RPI_GPIO25,
    RPI_GPIO26,
    RPI_GPIO27,
    RPI_GPIO28,
    RPI_GPIO29,
    RPI_GPIO30 = 30,
    RPI_GPIO31,
    RPI_GPIO32,
    RPI_GPIO33,
    RPI_GPIO34,
    RPI_GPIO35,
    RPI_GPIO36,
    RPI_GPIO37,
    RPI_GPIO38,
    RPI_GPIO39,
    RPI_GPIO40 = 40,
    RPI_GPIO41,
    RPI_GPIO42,
    RPI_GPIO43,
    RPI_GPIO44,
    RPI_GPIO45,
    RPI_GPIO46,
    RPI_GPIO47,
    RPI_GPIO48,
    RPI_GPIO49,
    RPI_GPIO50 = 50,
    RPI_GPIO51,
    RPI_GPIO52,
    RPI_GPIO53,
} rpi_gpio_pin_t;

/** The GPIO Peripheral is described in section 6 of the BCM2835 Peripherals
    documentation.

    There are 54 general-purpose I/O (GPIO) lines split into two banks. All
    GPIO pins have at least two alternative functions within BCM. The
    alternate functions are usually peripheral IO and a single peripheral
    may appear in each bank to allow flexibility on the choice of IO voltage.
    Details of alternative functions are given in section 6.2. Alternative
    Function Assignments.

    The GPIO peripheral has three dedicated interrupt lines. These lines are
    triggered by the setting of bits in the event detect status register. Each
    bank has its’ own interrupt line with the third line shared between all
    bits.

    The Alternate function table also has the pull state (pull-up/pull-down)
    which is applied after a power down. */
typedef volatile unsigned int rpi_reg_rw_t;
typedef volatile const unsigned int rpi_reg_ro_t;
typedef volatile unsigned int rpi_reg_wo_t;

typedef volatile unsigned long rpi_wreg_rw_t;
typedef volatile const unsigned long rpi_wreg_ro_t;

typedef struct
{
    rpi_reg_rw_t GPFSEL0;
    rpi_reg_rw_t GPFSEL1;
    rpi_reg_rw_t GPFSEL2;
    rpi_reg_rw_t GPFSEL3;
    rpi_reg_rw_t GPFSEL4;
    rpi_reg_rw_t GPFSEL5;
    rpi_reg_ro_t Reserved0;
    rpi_reg_wo_t GPSET0;
    rpi_reg_wo_t GPSET1;
    rpi_reg_ro_t Reserved1;
    rpi_reg_wo_t GPCLR0;
    rpi_reg_wo_t GPCLR1;
    rpi_reg_ro_t Reserved2;
    rpi_reg_wo_t GPLEV0;
    rpi_reg_wo_t GPLEV1;
    rpi_reg_ro_t Reserved3;
    rpi_reg_wo_t GPEDS0;
    rpi_reg_wo_t GPEDS1;
    rpi_reg_ro_t Reserved4;
    rpi_reg_wo_t GPREN0;
    rpi_reg_wo_t GPREN1;
    rpi_reg_ro_t Reserved5;
    rpi_reg_wo_t GPFEN0;
    rpi_reg_wo_t GPFEN1;
    rpi_reg_ro_t Reserved6;
    rpi_reg_wo_t GPHEN0;
    rpi_reg_wo_t GPHEN1;
    rpi_reg_ro_t Reserved7;
    rpi_reg_wo_t GPLEN0;
    rpi_reg_wo_t GPLEN1;
    rpi_reg_ro_t Reserved8;
    rpi_reg_wo_t GPAREN0;
    rpi_reg_wo_t GPAREN1;
    rpi_reg_ro_t Reserved9;
    rpi_reg_wo_t GPAFEN0;
    rpi_reg_wo_t GPAFEN1;
    rpi_reg_ro_t Reserved10;
    rpi_reg_wo_t GPIO_PUP_PDN_CNTRL_REG0;
    rpi_reg_wo_t GPIO_PUP_PDN_CNTRL_REG1;
    rpi_reg_wo_t GPIO_PUP_PDN_CNTRL_REG2;
    rpi_reg_wo_t GPIO_PUP_PDN_CNTRL_REG3;
} rpi_gpio_t;

typedef enum
{
    RPI_IO_LO = 0,
    RPI_IO_HI,
    RPI_IO_ON,
    RPI_IO_OFF,
    RPI_IO_UNKNOWN,
} rpi_gpio_value_t;

rpi_gpio_t *RPI_GetGpio(void);
void RPI_SetGpioPinFunction(rpi_gpio_pin_t gpio, rpi_gpio_alt_function_t func);
void RPI_SetGpioOutput(rpi_gpio_pin_t gpio);
void RPI_SetGpioInput(rpi_gpio_pin_t gpio);
rpi_gpio_value_t RPI_GetGpioValue(rpi_gpio_pin_t gpio);
void RPI_SetGpioHi(rpi_gpio_pin_t gpio);
void RPI_SetGpioLo(rpi_gpio_pin_t gpio);
void RPI_SetGpioValue(rpi_gpio_pin_t gpio, rpi_gpio_value_t value);
void RPI_ToggleGpio(rpi_gpio_pin_t gpio);
void RPI_SetPullResistor(rpi_gpio_pin_t gpio, rpi_gpio_pull pull);

#endif
