

#include <stdint.h>
#include "usb_app.h"
#include "usb_hal.h"
#include "string.h"
#include ".\\app\\rabbit_usb.h"


/********************************************************************/

#define BUF_CDC_LEN 64
#define CDC0 0


/*
    Call back from rabbit stack
    When data received from CDC port
*/
void rabbit_usb_cdc_dev_rx(uint32_t cdc_index, uint8_t* buf, uint32_t len)
{
    // get data from host
    //const char msg[] = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\r\n";
    const char msg[] = "0123";
    rabbit_usb_cdc_dev_tx(CDC0, (uint8_t *)msg, sizeof(msg));
}

void usb_main (void)
{
    // init the rabbit usb stack to be a cdc device
    rabbit_usb_init(usb0, e_usb_class_cdc_dev);
    rabbit_usb_wait_enumerate();
    while(1)
    {

    }
}




