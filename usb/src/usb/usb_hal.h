

typedef struct
{
    uint8_t   usb;
    uint8_t   ep;
    uint8_t   pid;
    uint8_t * buf;
    uint16_t  len;
}S_USB_PARA;
void    usb_hal_init(E_USB usb);
void    usb_hal_send(E_USB usb, uint8_t ep, uint8_t *buf, uint32_t buf_len);
void    usb_hal_send_continous(E_USB usb, uint8_t ep);
void    usb_hal_set_addr(E_USB usb, uint8_t addr);
void    usb_hal_set_toggle_data0(E_USB usb);
void    usb_hal_set_toggle_data1(E_USB usb);
void    usb_hal_rx_next(E_USB usb, uint8_t ep);



