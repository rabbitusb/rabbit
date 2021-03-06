


#include <stdint.h>
#include <string.h>
#include "..\\usb_config.h"
#include "..\\usb_spec.h"
#include "..\\app\\rabbit_usb.h"
#include "..\\hal\\usb_hal.h"
#include "..\\usb_debug.h"
#include ".\\usb_cls_cdc_dev.h"


#if 1 //USB_CFG_CDC_DEV_ENABLE

#define EP_CTRL      enum_ep0
#define EP_CDC_IN    enum_ep2
#define EP_CDC_OUT   enum_ep2


typedef enum
{
    enum_default,
    enum_set_addr,
    enum_addr_ready,
    enum_configured
}E_USB_STATE;

typedef enum
{
    enum_cmd_init,
    enum_cmd_set_line_coding
}E_CMD_STATE;

#pragma pack(push)
#pragma pack(1)
typedef struct
{
    uint32_t  dte_rate;
    uint8_t   format;
    uint8_t   parity;
    uint8_t   bits;
}S_CDC_LINE_CODING;
#pragma pack(pop)

static uint8_t            s_usb_addr  = 0;
static E_USB_STATE        s_usb_state = enum_default;
static E_CMD_STATE        s_cmd_state = enum_cmd_init;
static S_CDC_LINE_CODING  s_line_coding = {0};

static const uint8_t  device_descriptor[18];
static const uint8_t  config_descriptor[0x43];
static const uint8_t* string_table[4];


static void s_req_dev(S_SETUP * setup)
{
    switch(setup->req)
    {
        case REQ_SET_ADDR:
            debug_record_string("set address, ");
            s_usb_addr  = setup->value_l;
            s_usb_state = enum_set_addr;
            usb_hal_send(usb0, enum_ep0, 0, 0);
            break;

        case REQ_GET_DES:
            switch(setup->value_h)
            {
                case DES_DEVICE:
                    debug_record_string("des, device");
                    usb_hal_send(usb0,
                                 enum_ep0,
                                 (uint8_t*)device_descriptor,
                                 sizeof(device_descriptor));
                    break;

                case DES_CONFIGURATION:
                    debug_record_string("config");
                    usb_hal_send(usb0,
                                 enum_ep0,
                                 (uint8_t*)config_descriptor,
                                 sizeof(config_descriptor));
                    break;

                case DES_STRING:
                    debug_record_string("string");
                    usb_hal_send(usb0,
                                 enum_ep0,
                                 (uint8_t*)string_table[setup->value_l],
                                 string_table[setup->value_l][0]);
                    break;

                default:
                    usb_hal_send(usb0, enum_ep0, 0, 0);
                    break;
            }
            break;

        case REQ_SET_CONFIG:
            // get this request means enumuation finished.
            // driver_usb_set_interface();
            s_usb_state = enum_configured;
            usb_hal_send(usb0, enum_ep0, 0, 0);
            break;

        default:
            // never requested.
            usb_hal_send(usb0, enum_ep0, 0, 0);
            break;
    }
}
static void s_req_cls(S_SETUP * setup)
{
    switch(setup->req)
    {
        case REQ_CDC_SET_CONTROL_LINE_STATE:
            usb_hal_send(usb0, enum_ep0, 0, 0);
            break;

        case REQ_CDC_GET_LINE_CODING:
            usb_hal_send(usb0, enum_ep0, (uint8_t*)&s_line_coding, sizeof(s_line_coding));
            break;

        case REQ_CDC_SET_LINE_CODING:
            // prepare to set line coding in .
            s_cmd_state = enum_cmd_set_line_coding;
            usb_hal_send(usb0, enum_ep0, 0, 0);
            break;

        default:
            break;
    }
}
static void s_pck_setup(S_SETUP * setup)
{
    // after getting setup, should set to data1 state
    usb_hal_set_toggle_data1(usb0);

    switch(setup->req_type & 0x7f)
    {
        case REQ_STANDARD_DEVICE:
            s_req_dev(setup);
            break;

        case REQ_CLASS_INTERFACE:
            s_req_cls(setup);
            break;

        default:
            break;
    }
}
static void s_pck_out(S_USB_PARA * para)
{
    if(s_cmd_state == enum_cmd_set_line_coding)
    {
        memcpy(&s_line_coding, para->buf, sizeof(s_line_coding));
        s_cmd_state = enum_cmd_init;
    }

}
static void s_process_ep_ctrl(S_USB_PARA * para)
{
    // enumeration

    if(para->pid == PID_SETUP)
    {
        s_pck_setup((S_SETUP*)(para->buf));
        usb_hal_rx_next(usb0, para->ep);
    }
    else if(para->pid == PID_OUT)
    {
        s_pck_out(para);
        usb_hal_rx_next(usb0, para->ep);
    }
    else if(para->pid == PID_IN)
    {
        if(s_usb_state == enum_set_addr)
        {
            // set address after current transaction finished
            usb_hal_set_addr(0, s_usb_addr);
            s_usb_state = enum_addr_ready;
        }

        usb_hal_send_continous(0, para->ep);
    }
}
static void s_process_ep_cdc(S_USB_PARA * para)
{
    if(para->pid == PID_OUT)
    {
        // Call back to app.
        // Do not want to use function pointer here.
        rabbit_usb_cdc_dev_rx(0, para->buf, para->len);
        usb_hal_rx_next(usb0, para->ep);
    }
    else if(para->pid == PID_IN)
    {
        usb_hal_send_continous(0, para->ep);
    }
}

void cdc_dev_init(void)
{
    s_usb_state = enum_default;
    s_usb_addr  = 0;
    s_usb_state = enum_default;
    s_cmd_state = enum_cmd_init;

    s_line_coding.dte_rate  = 9600;
    s_line_coding.format    = 0;
    s_line_coding.parity    = 0;
    s_line_coding.bits      = 0x08;
}

void cdc_dev_entry(S_USB_PARA * para)
{
    if(para->ep == EP_CTRL)
    {
        s_process_ep_ctrl(para);
    }
    else if(para->ep == EP_CDC_OUT)
    {
        s_process_ep_cdc(para);
    }
}

uint32_t rabbit_usb_cdc_dev_tx(E_CDC cdc, uint8_t* buf, uint32_t len)
{
    uint8_t ep_tx;

    // enumerated?
    if(s_usb_state != enum_configured)
        return 0;

    // convert cdc to ep
    if(cdc == e_cdc0)
        ep_tx = EP_CDC_IN;

    return usb_hal_send(usb0, ep_tx, buf, len);
}



void rabbit_usb_wait_enumerate(void)
{
    int i = 0;
    while(s_usb_state != enum_configured)
    {
        i++;
        if(i == 1000*1000*3)
        {
            debug_show();
        }
    }
}


//-------------------------------------------
// descriptors
//-------------------------------------------
static const uint8_t device_descriptor[18]=
{
    0x12,      // blength
    0x01,      // bDescriptor
    0x10,0x01, // bcdUSB version = 1.10
    0x02,      // bDeviceClass
    0x00,      // bDeviceSubClass
    0x00,      // bDeviceProtocol
    0x08,      // 0x40,      // bMaxPacketSize0
    0xA2,0x15, // idVendor - 0x15A2(freescale Vendor ID)
    0x0F,0xA5, // idProduct
    0x00,0x00, // bcdDevice - Version 1.00
    0x01,      // iManufacturer - Index to string Manufacturer descriptor
    0x02,      // iProduct  - Index to string product descriptor
    0x03,      // iSerialNumber - Index to string serial number
    0x01       // bNumConfigurations - # of config. at current speed,
};

static const uint8_t config_descriptor[0x43]=
{

    0x09,       // blength
    0x02,       // bDescriptor
    0x43,0x00,  // wTotalLength   - # of bytes including interface and endpoint descpt.
    0x02,       // bNumInterfaces - at least 1 data interface
    0x01,       // bConfigurationValue
    0x00,       // iConfiguration - index to string descriptor
    0xC0,       // bmAttributes   - bit 7- Compatibility with USB 1.0
                //                  bit 6 if 1 self powered else Bus powered
                //                  bit 5-remote wakeup
                //                  bit 4-0-reserved
    0x32,       // bMaxPower - 200mA

    // Interface Descriptor
    0x09,       // blength
    0x04,       // bDescriptorType - Interface descriptor
    0x00,       // bInterfaceNumber - Zero based value identifying the index of the config.
    0x00,       // bAlternateSetting;
    0x01,       // bNumEndpoints - 2 endpoints
    0x02,       // bInterfaceClass - mass storage
    0x02,       // bInterfaceSubClass - SCSI Transparent command Set
    0x01,       // bInterfaceProtocol - Bulk-Only transport
    0x01,       // iInterface - Index to String descriptor

    0x05,
    0x24,
    0x00,
    0x10,
    0x01,
    0x05,
    0x24,0x01,
    0x00,0x01,
    0x04,0x24,
    0x02,0x00,
    0x05,0x24,
    0x06,0x00,
    0x01,

    // Endpoint  Descriptor
    0x07,           // blength
    0x05,           // bDescriptorType - EndPoint
    0x81,           // bEndpointAddress
    0x03,           // bmAttributes
    0x20,0x00,      // wMaxPacketSize
    0x02,           // bInterval

    // Interface Descriptor
    0x09,           // len
    0x04,           // ep - interface
    0x01,           // number
    0x00,           // alternate setting;
    0x02,           // include 2 endpoints
    0x0A,           // class
    0x00,           // subclass
    0x00,           // bulk
    0x01,           // index of string descriptor


    0x07,              // len
    0x05,              // ep - descriptor
    0x80|EP_CDC_IN,    // in, EP_CDC_IN
    0x02,              // bulk
    0x20,0x00,         // max package size for this ep is 32 byte
    0x00,              // interval

    0x07,              // len
    0x05,              // ep - descriptor
    0x00|EP_CDC_OUT,   // out, CDC_EP_OUT
    0x02,              // bulk
    0x20,0x00,         // max package size for this ep is 32 byte
    0x00,              // interval
};

static const uint8_t string_descriptor0[] =
{
    0x04,           // bLength;
    0x03,           // bDescriptorType - STRING descriptor
    0x09,0x04       // wLANDID0 - English (American)
};

static const uint8_t string_descriptor1[] =
{
    0x04,           // bLength; 11 bytes
    0x03,           // bDescriptorType - STRING descriptor
    'g',0x00,
    'j',0x00,
};

static const uint8_t string_descriptor2[] =
{
    0x12,           // bLength;
    0x03,           // bDescriptorType - STRING descriptor
    'U',0x00,
    'S',0x00,
    'B',0x00,
    '-',0x00,
    'U',0x00,
    'A',0x00,
    'R',0x00,
    'T',0x00
};

static const uint8_t string_descriptor3[] =
{
    0x12,       // bLength;
    0x03,       // bDescriptorType - STRING descriptor
    'T',0x00,
    'E',0x00,
    'S',0x00,
    'T',0x00,
    '_',0x00,
    '1',0x00,
    '.',0x00,
    '0',0x00
};

static const uint8_t* string_table[4] =
{
    string_descriptor0,
    string_descriptor1,
    string_descriptor2,
    string_descriptor3
};
#else
// dummy interface implementation, as CDC is not enabled.
void cdc_dev_init(void (call_back_get_data)(unsigned char * buf, int len)){}
void cdc_dev_entry(S_USB_PARA * para){}
void cdc_dev_wait_enumerate(void){}
void cdc_dev_send(unsigned char * buf, int len){}
#endif
