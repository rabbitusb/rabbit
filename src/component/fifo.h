


typedef struct 
{
    uint8_t * buf;
    uint32_t len;
    uint8_t * head;
    uint8_t * tail;
} FIFO;


void fifo_init(FIFO * fifo, uint8_t * buf, uint32_t len);
bool fifo_push(FIFO * fifo, uint8_t c);
bool fifo_pop(FIFO * fifo, uint8_t *c);


