
#include <stdint.h>
#include "usb_spec.h"
#include "usb_driver.h"
#include "usb_cdc.h"


enum E_EP_LIST
{
    EP0,
    EP1,
    EP2,
    EP3,
    EP4,
    EP5,
    EP6,
    EP7
};

static void s_ep_ctrl_setup(unsigned char * buf, int len)
{
    S_SETUP * setup;
    driver_set_toggle_data1();

    setup = (S_SETUP*)buf;

    switch(setup->req_type & 0x1F)
    {
        case REQ_STANDARD_DEVICE:
            // cdc_request_device();
            break;

        default:
            driver_usb_send(EP0,0,0);
            break;
    }
}
static void s_handler_ep_ctrl(int pid, unsigned char * buf, int len)
{
    switch(pid)
    {
        case PID_SETUP:
            s_ep_ctrl_setup(buf, len);
            break;
        default:
            driver_usb_send(0,0,0);
    }
}
/*
    When got data from host, this function is called.
*/
void usb_stack_device_entry_rx(uint8_t ep, uint8_t pid, uint8_t * buf, uint32_t len)
{
    if(ep == EP_CTRL)
    {
        s_handler_ep_ctrl(pid, buf, len);
    }
    else
    {
        // non - ctrl ep process
    }
}

/*
    when the data is sent out, this function is called.
*/
void usb_stack_device_entry_tx(uint8_t ep)
{
    if(ep == EP_CTRL)
    {

    }
    else
    {

    }
}
