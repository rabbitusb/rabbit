
#include <stdint.h>
#include "rabbit_usb.h"
#include "..\\hal\\usb_hal.h"
#include "..\\usb_core_dev.h"


void rabbit_usb_init(E_USB usb, E_USB_CLASS cls)
{
    usb_hal_init(usb);
    usb_core_set_class(usb, cls);
}

