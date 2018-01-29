

#include "stdint.h"
#include "MKL26Z4.h"
#include "hal_nvic.h"

static void allow_sie_rx(void)
{
    // 0 = allow SIE work, when get a setup, SIE set it to be 1
    USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
}
static void usb_enable(void)
{
    USB0->CTL |= USB_CTL_USBENSOFEN_MASK;
}

void hal_usb_init(void)
{
    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
    SIM->SOPT2  = 0x04050000; // USB clock src = PLL

    USB0->ISTAT    = 0xff;                             // clear flag
    USB0->CONTROL &= ~USB_CONTROL_DPPULLUPNONOTG_MASK; // disable DP pullup

    // BDT poinnter
    USB0->BDTPAGE1 = 0;
    USB0->BDTPAGE2 = 0;
    USB0->BDTPAGE3 = 0;

    USB0->ERRSTAT  = 0xff;                 // clear flag
    USB0->CTL     |= USB_CTL_ODDRST_MASK;  // reset odd state
    USB0->ADDR     = 0;                    // device address set to 0
    USB0->CTL     &= ~USB_CTL_ODDRST_MASK; // ?
    USB0->ERREN    = 0xff;                 // enable all error

    USB0->INTEN    = 0;                    // enable interrupt
    allow_sie_rx();
    hal_nvic_enable_irq(0x18);

    // run
    USB0->USBCTRL  = 0;
    USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;

    usb_enable();
}



