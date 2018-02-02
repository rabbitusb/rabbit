

#include <stdio.h>
#include <string.h>
#include "..\\print\\print.h"

static char debug_buf[1024*2] = {0};
static int cnt = 0;
static const char new_line[] = "\r\n";
void debug_record(char * buf, int size)
{
    int i;
    for(i=0; i<size; i++)
    {
        sprintf(debug_buf+cnt+i*3, "%.2x ", buf[i]);
    }
    cnt += size*3;

    //memcpy(debug_buf+cnt, new_line, sizeof(new_line));
    //cnt += 2;
}
void debug_record_string(char * buf)
{
    int len;
    len = sprintf(debug_buf+cnt, "%s", buf);
    cnt += len;
}
void debug_show(void)
{
    //rabbit_printf("%s", debug_buf);
    rabbit_send_string(debug_buf);
}

