
#include <stdint.h>
#include "usb_app.h"
#include "usb_hal.h"
#include "usb_core_dev.h"


void usb_app_init(E_USB usb, E_USB_CLASS cls)
{
    usb_hal_init(usb);
    usb_core_set_class(usb, cls);
}

