

#include "stdint.h"
#include "MKL26Z4.h"





void uart_pin_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    // PTA1, PTA2 -> ALT2
    PORTA->PCR[1] &= ~PORT_PCR_MUX_MASK;
    PORTA->PCR[2] &= ~PORT_PCR_MUX_MASK;
    PORTA->PCR[1] |= PORT_PCR_MUX(2);
    PORTA->PCR[2] |= PORT_PCR_MUX(2);

    // UART SRC
    SIM->SOPT5 &= ~(SIM_SOPT5_UART0TXSRC_MASK | SIM_SOPT5_UART0RXSRC_MASK);
    SIM->SOPT5 |= SIM_SOPT5_UART0TXSRC(0)|
                  SIM_SOPT5_UART0RXSRC(0);
}

void uart_ip_init(void)
{
    // the following code set UART0 baudrate to be 115200
    SIM->SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1); // 48M FLL output
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;

    UART0->C2  &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK); // disable
    UART0->C4  &= ~UART0_C4_OSR_MASK;
    UART0->C4  |= 31;
    UART0->BDH &= ~UART0_BDH_SBR_MASK;
    UART0->BDL  = 13;
    UART0->BDH &= ~UART0_BDH_SBNS_MASK;
    UART0->BDH |= UART0_BDH_SBNS(0);
    UART0->C2  |= UART0_C2_TE_MASK | UART0_C2_RE_MASK;
}
void uart_init(void)
{
    uart_pin_init();
    uart_ip_init();
}
void uart_send_char(char c)
{
    while (!(UART0->S1 & UART0_S1_TDRE_MASK));
    UART0->D = c;
}
