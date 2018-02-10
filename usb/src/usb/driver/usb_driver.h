

/*
    S_USB_PARA implement communication between driver and app.
*/


void     driver_usb0_init(void);
uint32_t driver_usb0_send(uint8_t ep, uint8_t *buf, uint32_t buf_len);
void     driver_usb0_send_continous(uint8_t ep);
void     driver_usb0_set_addr(uint8_t addr);
void     driver_usb0_set_toggle_data0(void);
void     driver_usb0_set_toggle_data1(void);
void     driver_usb0_rx_next(uint8_t ep);



