

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

/*
    const char msg[] =
"0123456789\r\n\
1123456789\r\n\
2123456789\r\n\
3123456789\r\n\
4123456789\r\n\
5123456789\r\n\
6123456789\r\n\
7123456789\r\n\
8123456789\r\n\
9123456789\r\n\
0123456789\r\n\
1123456789\r\n\
2123456789\r\n\
3123456789\r\n\
4123456789\r\n";
*/

    //const char msg[] = "0123";
    const char msg[] = "0123456789";
    rabbit_usb_cdc_dev_tx(CDC0, (uint8_t *)msg, sizeof(msg) - 1);
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




