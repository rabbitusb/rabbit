
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    e_usb_class_hid_dev,
    e_usb_class_cdc_dev,
    e_usb_class_msc_dev,
    e_usb_class_hid_hid_dev,
    e_usb_class_cdc_msc_dev
}E_USB_CLASS;

typedef enum
{
    usb0,
    usb1,
    usb2,
    usb3,
    usb4,
    usb5,
    usb6,
    usb7,
    usb_end
}E_USB;

typedef enum
{
    e_cdc0,
    e_cdc1,
    e_cdc2,
    e_cdc3,
    e_cdc4,
    e_cdc5,
    e_cdc6,
    e_cdc7
}E_CDC;




// APP API
void     rabbit_usb_init(E_USB usb, E_USB_CLASS cls);
void     rabbit_usb_wait_enumerate(void);
uint32_t rabbit_usb_cdc_dev_tx(E_CDC cdc, uint8_t* buf, uint32_t len);
void     rabbit_usb_cdc_dev_rx(E_CDC cdc, uint8_t* buf, uint32_t len);


