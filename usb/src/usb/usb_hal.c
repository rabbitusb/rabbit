
#include <stdint.h>
#include "driver\\usb_driver.h"
#include "usb_app.h"
void usb_hal_init(E_USB usb)
{
    if(usb == usb0)
        driver_usb0_init();
}
uint32_t usb_hal_send(E_USB usb, uint8_t ep, uint8_t *buf, uint32_t buf_len)
{
    if(usb == usb0)
        return driver_usb0_send(ep, buf, buf_len);
    else
        return 0;
}
void usb_hal_send_continous(E_USB usb, uint8_t ep)
{
    if(usb == usb0)
        driver_usb0_send_continous(ep);
}
void usb_hal_set_addr(E_USB usb, uint8_t addr)
{
    if(usb == usb0)
        driver_usb0_set_addr(addr);
}
void usb_hal_set_toggle_data0(E_USB usb)
{
    if(usb == usb0)
        driver_usb0_set_toggle_data0();
}
void usb_hal_set_toggle_data1(E_USB usb)
{
    if(usb == usb0)
        driver_usb0_set_toggle_data1();
}
void usb_hal_rx_next(E_USB usb, uint8_t ep)
{
    if(usb == usb0)
        driver_usb0_rx_next(ep);
}

