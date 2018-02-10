
/*
    This structure is shared between HAL can up layer like core, class
*/
typedef struct
{
    E_USB     usb;
    uint8_t   ep;
    uint8_t   pid;
    uint8_t * buf;
    uint16_t  len;
}S_USB_PARA;

typedef enum
{
    enum_ep0,
    enum_ep1,
    enum_ep2,
    enum_ep3,
    enum_ep4,
    enum_ep5,
    enum_ep6,
    enum_ep7
}E_EP;
void     usb_hal_init(E_USB usb);
uint32_t usb_hal_send(E_USB usb, E_EP ep, uint8_t *buf, uint32_t buf_len);
void     usb_hal_send_continous(E_USB usb, E_EP ep);
void     usb_hal_set_addr(E_USB usb, uint8_t addr);
void     usb_hal_set_toggle_data0(E_USB usb);
void     usb_hal_set_toggle_data1(E_USB usb);
void     usb_hal_rx_next(E_USB usb, E_EP ep);



