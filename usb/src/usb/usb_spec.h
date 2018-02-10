

#define PID_SETUP    0xd
#define PID_OUT      0x1
#define PID_IN       0x9
#define PID_ACK      0x2


#define REQ_STANDARD         0x00
#define REQ_SPECIFIC         0x20
#define REQ_VENDORSPEC       0x40
#define REQ_STANDARD_DEVICE  0x00
#define REQ_CLASS_INTERFACE  0x21
#define REQ_INTERFACE        0x01
#define REQ_ENDPOINT         0x02

#define REQ_SET_ADDR       5
#define REQ_GET_DES        6
#define REQ_SET_CONFIG     9

#define DES_DEVICE          1
#define DES_CONFIGURATION   2
#define DES_STRING          3
#define DES_INTERFACE       4
#define DES_ENDPOINT        5
#define DES_DEVICE_QUALIFIE 6
#define DES_INTERFACE_POWER 8

#define REQ_CDC_SET_LINE_CODING        0x20
#define REQ_CDC_GET_LINE_CODING        0x21
#define REQ_CDC_SET_CONTROL_LINE_STATE 0x22

#define REQ_CDC_WAITING_FOR_ENUMERATION 0x00
#define REQ_CDC_SET_LINE_CODING         0x20
#define REQ_CDC_GET_LINE_CODING         0x21
#define REQ_CDC_SET_CONTROL_LINE_STATE  0x22
#define REQ_CDC_LOADER_MODE             0xAA
#define REQ_CDC_GET_INTERFACE           0x0A
#define REQ_CDC_GET_STATUS              0x00
#define REQ_CDC_CLEAR_FEATURE           0x01
#define REQ_CDC_SET_FEATURE             0x03

typedef struct
{
    uint8_t req_type;
    uint8_t req;
    uint8_t value_l;
    uint8_t value_h;
    uint8_t index_l;
    uint8_t index_h;
    uint8_t len_l;
    uint8_t len_h;
}S_SETUP;

