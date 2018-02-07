


void cdc_dev_init(void (call_back_get_data)(unsigned char * buf, int len));
void cdc_dev_entry(S_USB_PARA * para);
void cdc_dev_wait_enumerate(void);
void cdc_dev_send(unsigned char * buf, int len);



