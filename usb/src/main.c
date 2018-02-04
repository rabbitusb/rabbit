
#include <stdint.h>
#include <stdbool.h>
#include ".\\hal\\hal_uart.h"
#include ".\\hal\\hal_mcg.h"
#include ".\\print\\print.h"

void usb_main (void);

int main(void)
{
    int cnt;
    cnt = 0;

    hal_mcg_init();  // Set system clock to 48MHz
    hal_uart_init(); // Set uart baudrate to 115200

    rabbit_printf("hello, rabbit.\n");
    usb_main();

    while (1)
    {
        cnt++;
    }

    return 0;
}




