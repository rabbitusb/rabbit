

#include "stdint.h"
#include "MKL26Z4.h"

#define MCG_S_CLKST_VAL ((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT)


void hal_mcg_init(void)
{
    SIM->CLKDIV1 = 0x10030000U;

    // enable osc0
    OSC0->CR = 0;
    OSC0->CR = 0x80;
    MCG->C2 =0xd4;
    while (!(MCG->S & MCG_S_OSCINIT0_MASK))
        ;

    // switch MCG to external clock
    MCG->C1  = 4;
    MCG->C2 &= ~MCG_C2_LP_MASK;
    MCG->C1  = 0x80;
    while ((MCG->S & (MCG_S_IREFST_MASK | MCG_S_CLKST_MASK)) !=
           (MCG_S_IREFST(0) | MCG_S_CLKST(2)))
        ;

    // switch to FLL
    MCG->C6 = 0;
    while (MCG->S & MCG_S_PLLST_MASK)
        ;

    // wait lock
    MCG->C5 = 1;
    MCG->C6 = 0;
    MCG->C5 = 0x41;
    while (!(MCG->S & MCG_S_LOCK0_MASK))
        ;

    // switch to pll
    MCG->C6 = 0x40;
    while (!(MCG->S & MCG_S_PLLST_MASK))
        ;
    MCG->C1 = 0;
    while (MCG_S_CLKST_VAL != 3)
        ;

    SIM->CLKDIV1 = 0x10010000;

    // select PLL
    SIM->SOPT2   = 0x00010000;
}
