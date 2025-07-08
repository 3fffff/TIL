#ifndef AUX_H
#define AUX_H

#include "gpio.h"
/* we only map the part for mini-uart */
struct AuxRegs
{
    unsigned int irq_status;
    unsigned int enables;
    unsigned int reserved[14];
    unsigned int mu_io;
    unsigned int mu_ier;
    unsigned int mu_iir;
    unsigned int mu_lcr;
    unsigned int mu_mcr;
    unsigned int mu_lsr;
    unsigned int mu_msr;
    unsigned int mu_scratch;
    unsigned int mu_control;
    unsigned int mu_status;
    unsigned int mu_baud_rate;
};

/* 0x00215000 is the offset from PBASE to AuxRegs */
#define REGS_AUX ((struct AuxRegs *)(PERIPHERAL_BASE + 0x00215000))

#endif /* AUX_H */