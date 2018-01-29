

#include <stdarg.h>
#include <stdio.h>
#include "..\\hal\\hal_uart.h"


static char buf[256];
static void send_string(char *str)
{
    for(; *str != 0; str++)
    {
        if(*str == '\n')
            hal_uart_send_char('\r');
        hal_uart_send_char(*str);
    }
}

void rabbit_printf(char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsprintf(buf, fmt, arg);
    va_end(arg);

    send_string(buf);
}
