



enum ENUM_DIRECTION
{
    direction_rx,
    direction_tx
};
void                driver_usb_init(void);
void                driver_usb_send(unsigned char ep, unsigned char *buf, unsigned int buf_len);
unsigned char       driver_usb_receive(unsigned char, unsigned char*);
void                driver_set_addr(unsigned char addr);
unsigned char       driver_get_pid(void);
void                driver_set_toggle_data0(void);
void                driver_set_toggle_data1(void);
void                driver_usb_notify_get_data(unsigned char ep);
void                driver_usb_notify_next_rx(unsigned char ep);
void                driver_usb_set_interface(void);
void                driver_usb_set_ep_in(unsigned char ep);




