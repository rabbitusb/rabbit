
#include <stdint.h>
#include <stdbool.h>
#include ".\\hal\\hal_uart.h"
#include ".\\hal\\hal_mcg.h"
#include ".\\print\\print.h"
#include ".\\component\\fifo.h"

#define FIFO_LEN 512
void usb_main (void);
uint8_t buf_fifo[FIFO_LEN];
FIFO fifo;


void print_fifo(void)
{
    uint8_t c;
    if(fifo_pop(&fifo, &c))
    {
        rabbit_printf("%c", c);
    }
}
int main(void)
{
    int cnt;
    cnt = 0;

    hal_mcg_init();  // Set system clock to 48MHz
    hal_uart_init(); // Set uart baudrate to 115200

    rabbit_printf("hello, rabbit.\n");

    fifo_init(&fifo, buf_fifo, FIFO_LEN);

    usb_main();

    while (1)
    {
        cnt++;
    }

    return 0;
}




