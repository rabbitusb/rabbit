
#include <stdint.h>
#include <stdbool.h>
#include ".\\hal\\hal_uart.h"
#include ".\\hal\\hal_mcg.h"
#include ".\\print\\print.h"
#include ".\\usb\\app\\rabbit_usb.h"

void usb_main (void);

int main(void)
{
    hal_mcg_init();  // Set system clock to 48MHz
    hal_uart_init(); // Set uart baudrate to 115200

    rabbit_printf("hello, rabbit.\n");
    usb_main();

    while (1)
        ;

    return 0;
}

/*
    Call back from rabbit stack
    When data received from CDC port

    cdc: If more than 1 cdc are implemented,
         use this parameter to know from which cdc
         the data comes.
*/
void rabbit_usb_cdc_dev_rx(E_CDC cdc, uint8_t* buf, uint32_t len)
{
    uint32_t send_len;
    const char msg[] = "0123456789";
    if(cdc == e_cdc0)
    {
        // Got data from cdc0
    }

    // Send data to cdc0
    send_len = rabbit_usb_cdc_dev_tx(e_cdc0, (uint8_t *)msg, sizeof(msg) - 1);
    if(send_len > 0)
        ; // send done
}

void usb_main (void)
{
    // Init usb0 to be a cdc device
    rabbit_usb_init(usb0, e_usb_class_cdc_dev);
    rabbit_usb_wait_enumerate();
    while(1)
        ;
}



