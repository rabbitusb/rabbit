



enum ENUM_DIRECTION
{
    direction_rx,
    direction_tx
};
typedef struct
{
    uint8_t   ep;
    uint8_t   odd;
    uint8_t   index;
    uint8_t   pid;
    uint8_t * buf;
    uint16_t  len;
}S_USB_PARA;

void    driver_usb_init(void);
void    driver_usb_send(uint8_t ep, uint8_t *buf, uint32_t buf_len);
void    driver_usb_send_continous(uint8_t ep);
uint8_t driver_usb_receive(uint8_t, uint8_t*);
void    driver_set_addr(uint8_t addr);
void    driver_set_toggle_data0(void);
void    driver_set_toggle_data1(void);
void    driver_usb_notify_get_data(uint8_t ep);
void    driver_usb_notify_next_rx(uint8_t ep);
void    driver_usb_set_interface(void);
void    driver_usb_set_ep_in(uint8_t ep);
void    driver_usb_set_owner_usb(S_USB_PARA * usb_para);



