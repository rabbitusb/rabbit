



#include <stdint.h>
#include <stdbool.h>
#include "fifo.h"

static uint8_t * s_inc(FIFO * fifo, uint8_t * p)
{
    p++;

    if(p == (fifo->buf + fifo->len))
        p = fifo->buf;

    return p;
}

void fifo_init(FIFO * fifo, uint8_t * buf, uint32_t len)
{
    fifo->buf  = buf;
    fifo->len  = len;
    fifo->head = buf;
    fifo->tail = buf;
}

bool fifo_push(FIFO * fifo, uint8_t c)
{
    uint8_t * next;
    next = s_inc(fifo, fifo->head);

    if(next != fifo->tail)
    {
        *(fifo->head) = c;
        fifo->head = next;
        return true;
    }
    else
        return false;
}

bool fifo_pop(FIFO * fifo, uint8_t *c)
{
    if(fifo->tail != fifo->head)
    {
        *c = *(fifo->tail);
        fifo->tail = s_inc(fifo, fifo->tail);
        return true;
    }
    else
        return false;
}




