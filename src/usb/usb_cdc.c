

#define DEBUG_ENABLE 0


#include <stdint.h>
#include "usb_driver.h"

#if DEBUG_ENABLE
#include "usb_debug.h"
#endif

enum E_USB_STATE
{
    power,
    enumerated,
    enabled,
    address,
    ready
};

enum E_EP_LIST
{
    EP0,
    EP1,
    EP2,
    EP3,
};

typedef struct
{
    unsigned char req_type;
    unsigned char req;
    unsigned char value_l;
    unsigned char value_h;
    unsigned char index_l;
    unsigned char index_h;
    unsigned char len_l;
    unsigned char len_h;
}S_SETUP;

#define EP_IN  EP2
#define EP_OUT EP3

#define REQUEST_SET_ADDRESS    5
#define REQUEST_GET_DESCRIPTOR 6
#define REQUEST_SET_CONFIG     9

#define DESCRIPTOR_DEVICE          1
#define DESCRIPTOR_CONFIGURATION   2
#define DESCRIPTOR_STRING          3
#define DESCRIPTOR_INTERFACE       4
#define DESCRIPTOR_ENDPOINT        5
#define DESCRIPTOR_DEVICE_QUALIFIE 6
#define DESCRIPTOR_INTERFACE_POWER 8

#define REQ_STANDARD    0x00
#define REQ_SPECIFIC    0x20
#define REQ_VENDORSPEC  0x40
#define REQ_DEVICE      0x00
#define REQ_INTERFACE   0x01
#define REQ_ENDPOINT    0x02

#define PID_SETUP    0xd
#define PID_OUT      0x1
#define PID_IN       0x9
#define PID_ACK      0x2

#define CDC_REQ_WAITING_FOR_ENUMERATION 0x00
#define CDC_REQ_SET_LINE_CODING         0x20
#define CDC_REQ_GET_LINE_CODING         0x21
#define CDC_REQ_SET_CONTROL_LINE_STATE  0x22
#define CDC_REQ_LOADER_MODE             0xAA
#define CDC_REQ_GET_INTERFACE           0x0A
#define CDC_REQ_GET_STATUS              0x00
#define CDC_REQ_CLEAR_FEATURE           0x01
#define CDC_REQ_SET_FEATURE             0x03

typedef struct
{
    unsigned int  dte_rate;
    unsigned char format;
    unsigned char parity;
    unsigned char bits;
}S_CDC_LINE_CODING;

enum E_USB_STATE   usb_state;
S_SETUP *          setup;
S_CDC_LINE_CODING  line_coding;
unsigned char      usb_addr;
static void (*p_call_back_get_data)(unsigned char * buf, int len);


void cdc_init(void (call_back_get_data)(unsigned char * buf, int len))
{
    p_call_back_get_data = call_back_get_data;
    usb_state            = power;

    /* Line Coding Initialization */
    line_coding.dte_rate    = 9600;
    line_coding.format = 0;
    line_coding.parity = 0;
    line_coding.bits   = 0x08;

    driver_usb_set_ep_in(EP_IN);
}


extern const unsigned char string_descriptor0[];
extern const unsigned char string_descriptor1[];
extern const unsigned char string_descriptor2[];
extern const unsigned char string_descriptor3[];
extern const unsigned char device_descriptor[18];
extern const unsigned char config_descriptor[0x43];

const unsigned char* string_table[4]=
{
    string_descriptor0,
    string_descriptor1,
    string_descriptor2,
    string_descriptor3
};

//static char buf_debug[100];

void cdc_request_device(void)
{
    switch(setup->req)
    {
        case REQUEST_SET_ADDRESS:
            usb_addr  = setup->value_l;
            usb_state = address;
            driver_usb_send(EP0,0,0);
            break;

        case REQUEST_GET_DESCRIPTOR:

            switch(setup->value_h)
            {
                case DESCRIPTOR_DEVICE:
                    driver_usb_send(EP0,
                                    (unsigned char*)device_descriptor,
                                    sizeof(device_descriptor));
                    break;

                case DESCRIPTOR_CONFIGURATION:
                    driver_usb_send(EP0,
                                    (unsigned char*)config_descriptor,
                                    sizeof(config_descriptor));
                    break;

                case DESCRIPTOR_STRING:
                    driver_usb_send(EP0,
                                    (unsigned char*)string_table[setup->value_l],
                                    string_table[setup->value_l][0]);
                    break;

                default:
                    driver_usb_send(EP0,0,0);
                    break;
            }
            break;

        case REQUEST_SET_CONFIG:
            // get this request means enumuation finished.
            driver_usb_set_interface();
            usb_state = enumerated;
            driver_usb_send(EP0,0,0);
            break;

        default:
            // never requested.
            driver_usb_send(EP0,0,0);
            break;
    }
}

void cdc_usb_pid_setup(void)
{
    driver_set_toggle_data1();

    switch(setup->req_type & 0x1F)
    {
        case REQ_DEVICE:
            cdc_request_device();
            break;

        default:
            driver_usb_send(EP0,0,0);
            break;
    }
}


void cdc_entry(unsigned char ep, enum ENUM_DIRECTION dir, unsigned char * buf, int size)
{
#if 0
    if(ep)
    {
        // data
        if(dir == direction_rx)
        {
            // let task process it
            driver_usb_notify_get_data(ep);
            p_call_back_get_data(buf, size);
            driver_usb_notify_next_rx(ep);
        }
        else
        {
            // tx
            void driver_usb_send_continous(void);
            driver_usb_send_continous();
        }
    }
#endif

    if(ep == 0)
    {
        // enumeration
        setup = (S_SETUP*)buf;

        // EP0
        #if DEBUG_ENABLE
        debug_record((char*)setup, size);
        #endif

        if(dir == direction_tx)
        {
            if(usb_state == address)
            {
                // set address after current transaction finished
                driver_set_addr(usb_addr);
                usb_state = ready;
            }

            driver_usb_send_continous();
        }
        else
        {
            switch(driver_get_pid())
            {
                case PID_SETUP:
                    cdc_usb_pid_setup();
                    break;
                default:
                    driver_usb_send(0,0,0);
            }
        }
    }
}

void cdc_send(unsigned char * buf, int len)
{
    driver_usb_send(EP_IN, buf, len);
}

void cdc_wait_enumerate(void)
{
    while(usb_state != enumerated);
}


//-------------------------------------------
// descriptors
//-------------------------------------------
const unsigned char device_descriptor[18]=
{
    0x12,      // blength
    0x01,      // bDescriptor
    0x10,0x01, // bcdUSB ver R=2.00
    0x02,      // bDeviceClass
    0x00,      // bDeviceSubClass
    0x00,      // bDeviceProtocol
    0x20,      // bMaxPacketSize0
    0xA2,0x15, // idVendor - 0x15A2(freescale Vendor ID)
    0x0F,0xA5, // idProduct
    0x00,0x00, // bcdDevice - Version 1.00
    0x01,      // iManufacturer - Index to string Manufacturer descriptor
    0x02,      // iProduct  - Index to string product descriptor
    0x03,      // iSerialNumber - Index to string serial number
    0x01       // bNumConfigurations - # of config. at current speed,
};

const unsigned char config_descriptor[0x43]=
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
    0x09,       //blength
    0x04,       //bDescriptorType - Interface descriptor
    0x00,       //bInterfaceNumber - Zero based value identifying the index of the config.
    0x00,       //bAlternateSetting;
    0x01,       //bNumEndpoints - 2 endpoints
    0x02,       //bInterfaceClass - mass storage
    0x02,       //bInterfaceSubClass - SCSI Transparent command Set
    0x01,       //bInterfaceProtocol - Bulk-Only transport
    0x01,       //iInterface - Index to String descriptor

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
    0x07,           //blength
    0x05,           //bDescriptorType - EndPoint
    0x81,           //bEndpointAddress
    0x03,           //bmAttributes
    0x20,0x00,      //wMaxPacketSize
    0x02,           //bInterval

    // Interface Descriptor
    0x09,           //blength
    0x04,           //bDescriptorType - Interface descriptor
    0x01,           //bInterfaceNumber - Zero based value identifying the index of the config.
    0x00,           //bAlternateSetting;
    0x02,           //bNumEndpoints - 2 endpoints
    0x0A,           //bInterfaceClass - mass storage
    0x00,           //bInterfaceSubClass - SCSI Transparent command Set
    0x00,           //bInterfaceProtocol - Bulk-Only transport
    0x01,           //iInterface - Index to String descriptor

    // Endpoint OUT Descriptor
    0x07,           //blength
    0x05,           //bDescriptorType - EndPoint
    0x82,           //bEndpointAddress
    0x02,           //bmAttributes
    0x20,0x00,      //wMaxPacketSize
    0x00,           //bInterval

    // Endpoint IN Descriptor
    0x07,           //blength
    0x05,           //bDescriptorType - EndPoint
    0x03,           //bEndpointAddress
    0x02,           //bmAttributes
    0x20,0x00,      //wMaxPacketSize
    0x00,           //bInterval
};

const unsigned char string_descriptor0[] =
{
    0x04,           // bLength;
    0x03,           // bDescriptorType - STRING descriptor
    0x09,0x04       // wLANDID0 - English (American)
};

const unsigned char string_descriptor1[] =
{
    0x04,           //bLength; 11 bytes
    0x03,           //bDescriptorType - STRING descriptor
    'g',0x00,
    'j',0x00,
};

const unsigned char string_descriptor2[] =
{
    0x12,           //bLength;
    0x03,           //bDescriptorType - STRING descriptor
    'U',0x00,
    'S',0x00,
    'B',0x00,
    '-',0x00,
    'U',0x00,
    'A',0x00,
    'R',0x00,
    'T',0x00
};

const unsigned char string_descriptor3[] =
{
    0x12,      //bLength;
    0x03,      //bDescriptorType - STRING descriptor
    'T',0x00,
    'E',0x00,
    'S',0x00,
    'T',0x00,
    '_',0x00,
    '1',0x00,
    '.',0x00,
    '0',0x00
};


