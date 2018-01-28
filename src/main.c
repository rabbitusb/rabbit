

#include ".\\hal\\hal_uart.h"
#include ".\\hal\\hal_mcg.h"

int main(void)
{
    int cnt;
    cnt = 0;

    mcg_init();  // Set system clock to 48MHz
    uart_init(); // Set uart baudrate to 115200
    uart_send_char('a');
    uart_send_char('b');
    uart_send_char('c');
    uart_send_char('d');

    while (1)
    {
        cnt++;
    }

    return 0;
}




