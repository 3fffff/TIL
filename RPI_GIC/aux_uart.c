#include "gpio.h"
#include  "aux_uart.h"

#define TXD0 14
#define RXD0 15

void mini_uart_init()
{
    /* set gpio 14 and 15 to UART1 (mini-uart) */
    RPI_SetGpioPinFunction(TXD0, FS_ALT5);
    RPI_SetGpioPinFunction(RXD0, FS_ALT5);
    /* clear the pud resistors (initialize them) */
    gpio_pin_enable(TXD0);
    gpio_pin_enable(RXD0);
    /* enable mini-uart */
    REGS_AUX->enables = 1;
    /* disable TX and RX and auto flow control */
    REGS_AUX->mu_control = 0;
    /* enable receive interrupts, check bcm errata */
    REGS_AUX->mu_ier = 0xD;
    /* set 8-bit mode */
    REGS_AUX->mu_lcr = 3;
    /* set RTS to always high */
    REGS_AUX->mu_mcr = 0;
    /* 115200 @ 500 MHz */
    REGS_AUX->mu_baud_rate = 541;
    /* enable TX and RX */
    REGS_AUX->mu_control = 3;

    mini_uart_send('\r');
    mini_uart_send('\n');
    mini_uart_send('\n');
}

void mini_uart_send(char c)
{
    /* keep looping if the 5th bit is 0 */
    while (!(REGS_AUX->mu_lsr & 0x20));

    REGS_AUX->mu_io = c;
}

char mini_uart_recv()
{
    while (!(REGS_AUX->mu_lsr & 1));

    return REGS_AUX->mu_io & 0xFF;
}

void mini_uart_send_string(char *str)
{
    while (*str) {
        if (*str == '\n')
            /* also do CR if there's a '\n' */
            mini_uart_send('\r');
        mini_uart_send(*str);
        str++;
    }
}
