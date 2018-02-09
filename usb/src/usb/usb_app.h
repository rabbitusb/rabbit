

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

void rabbit_usb_init(E_USB usb, E_USB_CLASS cls);



