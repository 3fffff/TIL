#ifndef UART_H
#define UART_H
#include "gpio.h"

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
void uart_init(void);
void uart_send(unsigned int c);
#define UART3_ADDRESS (PERIPHERAL_BASE + 0x201600)
#endif