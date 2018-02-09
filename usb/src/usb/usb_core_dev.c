

#include <stdint.h>
#include "usb_spec.h"
#include "usb_app.h"
#include "usb_hal.h"
#include ".\\class\\usb_cls_cdc_dev.h"

E_USB_CLASS usb_class[usb_end];


void usb_core_set_class(E_USB usb, E_USB_CLASS cls)
{
    usb_class[usb] = cls;

    if(cls == e_usb_class_cdc_dev)
        cdc_dev_init();
}
void usb_core_dev_entry(S_USB_PARA * para)
{
    E_USB_CLASS cls;
    cls = usb_class[para->usb];

/*
    This is the key feature in rabbit usb stack.
    A usb controller run a predefined class on it.
    It can be only one class, also can be a composite class.
*/
    switch(cls)
    {
        case e_usb_class_hid_dev:
            break;

        case e_usb_class_cdc_dev:
            cdc_dev_entry(para);
            break;

        case e_usb_class_msc_dev:
            break;

        case e_usb_class_hid_hid_dev:
            break;

        case e_usb_class_cdc_msc_dev:
            break;

        default:
            break;
    }
}



