#include "mailbox.h"
unsigned long _regs[38];
unsigned int mbox_call(volatile unsigned int *data, unsigned char ch)
{
    // 28-bit address (MSB) and 4-bit value (LSB)
    unsigned int r = ((unsigned int)((unsigned long)data) & ~0xF) | (ch & 0xF);

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
            return data[1] == MBOX_RESPONSE; // Is it successful?
    }
    return 0;
}
