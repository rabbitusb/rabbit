

#include ".\\hal\\hal_uart.h"
#include ".\\hal\\hal_mcg.h"

int main(void)
{
    int cnt;
    cnt = 0;

    mcg_init();  // Set system clock to 48MHz
    uart_init(); // Set uart baudrate to 115200

    while (1)
    {
        cnt++;
    }

    return 0;
}




