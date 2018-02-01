
#define EP_CTRL 0

#define PID_SETUP    0xd
#define PID_OUT      0x1
#define PID_IN       0x9
#define PID_ACK      0x2


#define REQ_STANDARD             0x00
#define REQ_SPECIFIC             0x20
#define REQ_VENDORSPEC           0x40
#define REQ_STANDARD_DEVICE      0x00
#define REQ_CLASS_INTERFACE      0x21
#define REQ_INTERFACE            0x01
#define REQ_ENDPOINT             0x02

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

