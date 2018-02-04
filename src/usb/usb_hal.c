
#include <stdint.h>
#include "usb_driver.h"
void usb_hal_init(uint8_t usb_index)
{
    if(usb_index == 0)
        driver_usb0_init();
}
void usb_hal_send(uint8_t usb_index, uint8_t ep, uint8_t *buf, uint32_t buf_len)
{
    if(usb_index == 0)
        driver_usb0_send(ep, buf, buf_len);
}
void usb_hal_send_continous(uint8_t usb_index, uint8_t ep)
{
    if(usb_index == 0)
        driver_usb0_send_continous(ep);
}
void usb_hal_set_addr(uint8_t usb_index, uint8_t addr)
{
    if(usb_index == 0)
        driver_usb0_set_addr(addr);
}
void usb_hal_set_toggle_data0(uint8_t usb_index)
{
    if(usb_index == 0)
        driver_usb0_set_toggle_data0();
}
void usb_hal_set_toggle_data1(uint8_t usb_index)
{
    if(usb_index == 0)
        driver_usb0_set_toggle_data1();
}
void usb_hal_rx_next(uint8_t usb_index, uint8_t ep)
{
    if(usb_index == 0)
        driver_usb0_rx_next(ep);
}

