
#include "usb_driver.h"
#include "usb_cdc.h"
#include "string.h"



/********************************************************************/

#define BUF_CDC_LEN 64

static int data_arrive = 0;
static unsigned char buf_cdc_rx[BUF_CDC_LEN];
static unsigned int len_rx;

void call_back_get_data(unsigned char * buf, int len)
{
    if(len > BUF_CDC_LEN)
        len = BUF_CDC_LEN;

    memcpy(buf_cdc_rx, buf, len);
    len_rx = len;

    data_arrive = 1;
}

void wait_data_arrive(void)
{
    while(data_arrive == 0);
    data_arrive = 0;
}
char msg[] = "0000000000111111111122222222220123456789\r\n";
void process_data_received(void)
{
    //const char msg[] = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\r\n";
    //cdc_send(buf_cdc_rx, len_rx);
    cdc_send((unsigned char *)msg, sizeof(msg));
}

void debug_show(void);

void usb_main (void)
{
    driver_usb_init();
    cdc_init(call_back_get_data);
    cdc_wait_enumerate();
    debug_show();
    while(1)
    {
        wait_data_arrive();
        process_data_received();
    }
}




