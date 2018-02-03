

#include <stdio.h>
#include <string.h>
#include "..\\print\\print.h"

#define DEBUG_ENABLE 0
#define DEBUG_BUF_LEN (3*1024)
static char debug_buf[DEBUG_BUF_LEN] = {0};
static const char new_line[] = "\r\n";



#if DEBUG_ENABLE
static int cnt = 0;
void debug_record(char * buf, int size)
{
    int i;
    int len;
    if(cnt >= (DEBUG_BUF_LEN - 100))
        return; // buffer full

    for(i=0; i<size; i++)
    {
        //len = sprintf(debug_buf+cnt+i*3, "%.2x , cnt = %d", buf[i], cnt);
        len = sprintf(debug_buf+cnt+i*3, "%.2x", buf[i]);
    }
    cnt += len;

    //memcpy(debug_buf+cnt, new_line, sizeof(new_line));
    //cnt += 2;
}
void debug_record_string(char * buf)
{
    int len;
    if(cnt >= (DEBUG_BUF_LEN - 100))
        return; // buffer full
    len = sprintf(debug_buf+cnt, "%s", buf);
    cnt += len;
}
#else
void debug_record(char * buf, int size)
{

}
void debug_record_string(char * buf)
{

}
#endif


void debug_show(void)
{
    //rabbit_printf("%s", debug_buf);
    rabbit_send_string(debug_buf);
}

