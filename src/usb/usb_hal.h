

typedef struct
{
    uint8_t   ep;
    uint8_t   pid;
    uint8_t * buf;
    uint16_t  len;
}S_USB_PARA;
void    usb_hal_init(uint8_t usb_index  );
void    usb_hal_send(uint8_t usb_index, uint8_t ep, uint8_t *buf, uint32_t buf_len);
void    usb_hal_send_continous(uint8_t usb_index, uint8_t ep);
void    usb_hal_set_addr(uint8_t usb_index, uint8_t addr);
void    usb_hal_set_toggle_data0(uint8_t usb_index);
void    usb_hal_set_toggle_data1(uint8_t usb_index);
void    usb_hal_rx_next(uint8_t usb_index, uint8_t ep);



