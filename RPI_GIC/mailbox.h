#ifndef MAILBOX_H
#define MAILBOX_H
#include "gpio.h"
// GPU messagebox
#define MBOX_READ (PERIPHERAL_BASE + 0x0000B880)
#define MBOX_STATUS (PERIPHERAL_BASE + 0x0000B898)
#define MBOX_WRITE (PERIPHERAL_BASE + 0x0000B8A0)
#define MBOX_RESPONSE 0x80000000
#define MBOX_FULL 0x80000000
#define MBOX_EMPTY 0x40000000

unsigned int mbox_call(unsigned char ch);
#endif
