





void cdc_init(void (call_back_get_data)(unsigned char * buf, int len));
void cdc_entry(unsigned char ep, enum ENUM_DIRECTION dir, unsigned char * buf, int size);
void cdc_wait_enumerate(void);
void cdc_send(unsigned char * buf, int len);

