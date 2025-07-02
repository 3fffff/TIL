
#include "uart.h" // initialize PL011 UART3 on GPIO4 and 5
#include "gpio.h"
#include "mailbox.h"
static struct uart_t *uart3 = (struct uart_t *)UART3_ADDRESS;
void uart_init(void)
{
    // turn off UART3
    uart3->CR = 0;

    mbox_call(0x08);
    // map UART3 to GPIO pins

    RPI_SetGpioPinFunction(RPI_GPIO4, FS_ALT4);
    RPI_SetGpioPinFunction(RPI_GPIO5, FS_ALT4);

    // remove pullup or pulldown
    RPI_SetPullResistor(RPI_GPIO4, PULL_NONE);
    RPI_SetPullResistor(RPI_GPIO5, PULL_NONE);


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

/*void uart_writeText(char *buffer)
{
    while (*buffer)
    {
        if (*buffer == '\n')
            uart_writeByteBlocking('\r');
        uart_writeByteBlocking(*buffer++);
    }
}*/

void uart_send(unsigned int c)
{
    /* wait until we can send */
    do
    {
        asm volatile("nop");
    } while (uart3->FR & 0x20);
    /* write the character to the buffer */
    uart3->DR = c;
}

/*void uart_read()
{
    do
    {
        ;
    } while (uart3->FR & 0x10);
    return uart3->DR;
}*/