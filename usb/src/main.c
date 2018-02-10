

#include ".\\hal\\hal_uart.h"
#include ".\\hal\\hal_mcg.h"
#include ".\\print\\print.h"
#include ".\\demo\\demo_cdc.h"


int main(void)
{
    hal_mcg_init();  // Set system clock to 48MHz
    hal_uart_init(); // Set uart baudrate to 115200

    rabbit_printf("hello, rabbit.\n");

    demo_cdc();

    while (1)
        ;

    return 0;
}



