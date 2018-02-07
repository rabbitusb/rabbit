


#include <stdint.h>
#include <string.h>
#include "..\\usb_config.h"
#include "..\\usb_spec.h"
#include "..\\usb_app.h"
#include "..\\usb_hal.h"
#include "..\\usb_debug.h"

#if 1 //USB_CFG_CDC_DEV_ENABLE


#define EP_IN  enum_ep2
#define EP_OUT enum_ep3

#define REQUEST_SET_ADDRESS    5
#define REQUEST_GET_DESCRIPTOR 6
#define REQUEST_SET_CONFIG     9

#define REQUEST_SET_LINE_CODING        0x20
#define REQUEST_GET_LINE_CODING        0x21
#define REQUEST_SET_CONTROL_LINE_STATE 0x22


#define DESCRIPTOR_DEVICE          1
#define DESCRIPTOR_CONFIGURATION   2
#define DESCRIPTOR_STRING          3
#define DESCRIPTOR_INTERFACE       4
#define DESCRIPTOR_ENDPOINT        5
#define DESCRIPTOR_DEVICE_QUALIFIE 6
#define DESCRIPTOR_INTERFACE_POWER 8

#define CDC_REQ_WAITING_FOR_ENUMERATION 0x00
#define CDC_REQ_SET_LINE_CODING         0x20
#define CDC_REQ_GET_LINE_CODING         0x21
#define CDC_REQ_SET_CONTROL_LINE_STATE  0x22
#define CDC_REQ_LOADER_MODE             0xAA
#define CDC_REQ_GET_INTERFACE           0x0A
#define CDC_REQ_GET_STATUS              0x00
#define CDC_REQ_CLEAR_FEATURE           0x01
#define CDC_REQ_SET_FEATURE             0x03

typedef enum
{
    enum_powerd,
    enum_set_addr,
    enum_addr_ready,
    enum_configured
}E_USB_STATE;

typedef enum
{
    enum_ep0,
    enum_ep1,
    enum_ep2,
    enum_ep3
}E_EP_LIST;

typedef enum
{
    enum_cmd_init,
    enum_cmd_set_line_coding
}E_CMD_STATE;


#pragma pack(1)
typedef struct
{
    uint32_t  dte_rate;
    uint8_t   format;
    uint8_t   parity;
    uint8_t   bits;
}S_CDC_LINE_CODING;
#pragma pack()

static uint8_t            s_usb_addr  = 0;
static E_USB_STATE        s_usb_state = enum_powerd;
static E_CMD_STATE        s_cmd_state = enum_cmd_init;
static S_CDC_LINE_CODING  s_line_coding = {0};

static void (*p_call_back_get_data)(uint8_t * buf, int len);

static const uint8_t  device_descriptor[18];
static const uint8_t  config_descriptor[0x43];
static const uint8_t* string_table[4];


static void s_req_dev(S_SETUP * setup)
{
    switch(setup->req)
    {
        case REQUEST_SET_ADDRESS:
            debug_record_string("set address, ");
            s_usb_addr  = setup->value_l;
            s_usb_state = enum_set_addr;
            usb_hal_send(usb0, enum_ep0, 0, 0);
            break;

        case REQUEST_GET_DESCRIPTOR:
            switch(setup->value_h)
            {
                case DESCRIPTOR_DEVICE:
                    debug_record_string("des, device");
                    usb_hal_send(usb0,
                                 enum_ep0,
                                 (uint8_t*)device_descriptor,
                                 sizeof(device_descriptor));
                    break;

                case DESCRIPTOR_CONFIGURATION:
                    debug_record_string("config");
                    usb_hal_send(usb0,
                                 enum_ep0,
                                 (uint8_t*)config_descriptor,
                                 sizeof(config_descriptor));
                    break;

                case DESCRIPTOR_STRING:
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

        case REQUEST_SET_CONFIG:
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
        case REQUEST_SET_CONTROL_LINE_STATE:
            usb_hal_send(usb0, enum_ep0, 0, 0);
            break;

        case REQUEST_GET_LINE_CODING:
            usb_hal_send(usb0, enum_ep0, (uint8_t*)&s_line_coding, sizeof(s_line_coding));
            break;

        case REQUEST_SET_LINE_CODING:
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
static void s_process_ep0(S_USB_PARA * para)
{
    // enumeration

    switch(para->pid)
    {
        case PID_SETUP:
            debug_record_string("SET, ");
            s_pck_setup((S_SETUP*)(para->buf));
            usb_hal_rx_next(0, para->ep);
            break;

        case PID_OUT:
            debug_record_string("OUT, ");
            s_pck_out(para);
            usb_hal_rx_next(0, para->ep);
            break;

        case PID_IN:
            debug_record_string("IN, ");
            if(s_usb_state == enum_set_addr)
            {
                // set address after current transaction finished
                usb_hal_set_addr(0, s_usb_addr);
                s_usb_state = enum_addr_ready;
            }

            usb_hal_send_continous(0, para->ep);
            break;

        default:
            ;
    }
}


void cdc_dev_init(void (call_back_get_data)(uint8_t * buf, int len))
{
    p_call_back_get_data = call_back_get_data;
    s_usb_state          = enum_powerd;

    /* Line Coding Initialization */
    s_line_coding.dte_rate  = 9600;
    s_line_coding.format    = 0;
    s_line_coding.parity    = 0;
    s_line_coding.bits      = 0x08;

    // driver_usb_set_ep_in(EP_IN);
}

void cdc_dev_entry(S_USB_PARA * para)
{
/*
    if(para->ep)
    {
        // data
        if(dir == direction_rx)
        {
            // let task process it
            driver_usb_notify_get_data(para->ep);
            p_call_back_get_data(para->buf, para->len);
            driver_usb_notify_next_rx(para->ep);
        }
        else
        {
            // tx
            //driver_usb_send_continous(para->ep);
        }
    }
*/
    if(para->ep == 0)
    {
        s_process_ep0(para);
    }
    else
    {
        debug_record_string("non ep0,");
    }
}

void cdc_dev_send(uint8_t * buf, int len)
{
    usb_hal_send(usb0, EP_IN, buf, len);
}


void cdc_dev_wait_enumerate(void)
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
    0x09,           // blength
    0x04,           // bDescriptorType - Interface descriptor
    0x01,           // bInterfaceNumber - Zero based value identifying the index of the config.
    0x00,           // bAlternateSetting;
    0x02,           // bNumEndpoints - 2 endpoints
    0x0A,           // bInterfaceClass - mass storage
    0x00,           // bInterfaceSubClass - SCSI Transparent command Set
    0x00,           // bInterfaceProtocol - Bulk-Only transport
    0x01,           // iInterface - Index to String descriptor

    // Endpoint OUT Descriptor
    0x07,           // blength
    0x05,           // bDescriptorType - EndPoint
    0x82,           // bEndpointAddress
    0x02,           // bmAttributes
    0x20,0x00,      // wMaxPacketSize
    0x00,           // bInterval

    // Endpoint IN Descriptor
    0x07,           // blength
    0x05,           // bDescriptorType - EndPoint
    0x03,           // bEndpointAddress
    0x02,           // bmAttributes
    0x20,0x00,      // wMaxPacketSize
    0x00,           // bInterval
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
