#include "mailbox.h"
// a properly aligned message buffer with: 9x4 byte long mes-
// sage setting feature PL011 UART Clock to 3MHz (and some TAGs and
// sizes of parameter in message)
volatile unsigned int __attribute__((aligned(16))) mbox[9] = {36, 0, 0x38002, 12, 8, 2, 3000000, 0, 0};

unsigned long _regs[38];
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
