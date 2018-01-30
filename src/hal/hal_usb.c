
#include <stdbool.h>
#include <stdint.h>
#include "MKL26Z4.h"
#include "hal_nvic.h"

#define USB_IRQ 0x18

static void usb_allow_sie_rx(void)
{
    // 0 = allow SIE work, when get a setup, SIE set it to be 1
    USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
}
static void usb_enable(void)
{
    USB0->CTL |= USB_CTL_USBENSOFEN_MASK;
}
static void usb_pull_up(bool enable)
{
    if(enable)
        USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
    else
        USB0->CONTROL &= ~USB_CONTROL_DPPULLUPNONOTG_MASK;
}
void hal_usb_init(void)
{
    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
    // USB clock src = PLL
    SIM->SOPT2  = 0x04050000;

    // Reset USB Module
    USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
    while(USB0->USBTRC0 & USB_USBTRC0_USBRESET_MASK);

    // BDT poinnter
    USB0->BDTPAGE1 = 0;
    USB0->BDTPAGE2 = 0;
    USB0->BDTPAGE3 = 0;

    // Clear USB Reset flag
    USB0->ISTAT |= USB_ISTAT_USBRST_MASK;
    // Enable USB Reset Interrupt
    USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;

    usb_allow_sie_rx();
    hal_nvic_enable_irq(USB_IRQ);

    // run
    USB0->USBCTRL  = 0;
    USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;

    usb_enable();
}



