

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "..\\hal\\MKL26Z4.h"
#include "..\\hal\\hal_nvic.h"
#include "usb_driver.h"
#include "usb_cdc.h"
#include "usb_stack_device.h"


#define USB_IRQ_NUMBER 0x18

/*
    one BD, 8 bytes
    one directtion has two BD for odd and event, 16 bytes
    one end point has two direction, then 32 bytes
    up to 16 end point, 32*16 = 512 bytes
*/
#define BUF_SIZE_OF_ONE_EP 64

#define PACKAGE_SIZE 64 //64?

#define OWN_MCU 0x00
#define OWN_SIE 0x80

#define TOGGLE_DATA0 0x88
#define TOGGLE_DATA1 0xc8

enum E_EP_CONFIG_INDEX
{
    index_ep0_out_even,     // USB-->Mem, RX
    index_ep0_out_odd,      // USB-->Mem, RX
    index_ep0_in_even,      // Mem-->USB, TX
    index_ep0_in_odd,       // Mem-->USB, TX
    index_ep1_out_even,
    index_ep1_out_odd,
    index_ep1_in_even,
    index_ep1_in_odd,
    index_ep2_out_even,
    index_ep2_out_odd,
    index_ep2_in_even,
    index_ep2_in_odd,
    index_ep3_out_even,
    index_ep3_out_odd,
    index_ep3_in_even,
    index_ep3_in_odd
};


typedef union
{
    uint8_t _byte;
    struct
    {
        uint8_t :1;
        uint8_t :1;
        uint8_t bstall:1;              //buffer stall enable
        uint8_t dts:1;                 //data toggle synch enable
        uint8_t ninc:1;                //address increment disable
        uint8_t keep:1;                //bd keep enable
        uint8_t data:1;                //data toggle synch value
        uint8_t uown:1;                //usb ownership
    }flag1;

    struct
    {
        uint8_t    :2;
        uint8_t pid:4;                 //Packet Identifier
        uint8_t    :2;
    }flag2;
} U_BD_FLAG;                         //Buffer Descriptor Status Register

typedef struct
{
    U_BD_FLAG  bd_flag;
    uint8_t    dummy;
    uint16_t   cnt;
    uint32_t   addr;
} S_BD;



#define TOGGLE_UDATA0   0x88
#define TOGGLE_UDATA1   0xC8
static uint8_t toggle_data;


__attribute__((aligned(512))) static S_BD bd_table[16]; // for 4 EP

#define EP_COUNT             4
#define BUF_COUNT_IN_EACH_EP 4

static uint8_t   ep_buf[BUF_SIZE_OF_ONE_EP * EP_COUNT * BUF_COUNT_IN_EACH_EP];
static uint8_t * p_data;
static uint32_t  counter;
static uint8_t   ep_in;
static uint8_t   usb_tx_odd[EP_COUNT];
// ------------------------------------------------
static void update_toggle_data(void)
{
    if(toggle_data == TOGGLE_UDATA0)
        toggle_data = TOGGLE_UDATA1;
    else
        toggle_data = TOGGLE_UDATA0;
}

void driver_set_toggle_data0(void)
{
    toggle_data = TOGGLE_UDATA0;
}

void driver_set_toggle_data1(void)
{
    toggle_data = TOGGLE_UDATA1;
}

static void load_to_send_buffer_manager(uint8_t *data, uint32_t size)
{
    if(counter == 0)
    {
        // load happens when counter is 0
        p_data  = data;
        counter = size;
    }
}

static uint8_t load_from_send_buffer_manager(void)
{
    uint32_t r;

    if(counter > PACKAGE_SIZE)
    {
        // BUF_SIZE_OF_ONE_EP is the maximum package length for one transaction
        r        = PACKAGE_SIZE;
        counter -= PACKAGE_SIZE;
    }
    else
    {
        r       = counter;
        counter = 0;
    }

    return r;
}

static void load_rx_para(S_USB_PARA * usb_para)
{
    usb_para->ep  = USB0->STAT>>4;
    usb_para->odd = (USB0->STAT>>2) & 0x01;
    switch(usb_para->ep)
    {
        case 0:
            usb_para->index = index_ep0_out_even;
            break;
        case 3:
            usb_para->index = index_ep3_out_even;
            break;
        default:
            ;
    }

    usb_para->index += usb_para->odd;
    usb_para->buf    = ep_buf + BUF_SIZE_OF_ONE_EP * usb_para->index;
    usb_para->pid    = bd_table[usb_para->index].bd_flag.flag2.pid;
    usb_para->len    = bd_table[usb_para->index].cnt;
}
void  driver_usb_update_bd(S_USB_PARA * usb_para)
{
    bd_table[usb_para->index].cnt = 8;
    #define USB_RECEIVE_DATA0 0x80
    #define USB_RECEIVE_DATA1 0xC0

    bd_table[usb_para->index].bd_flag._byte = USB_RECEIVE_DATA0;
}
static void s_bd_init(void)
{
    // EP0 OUT BDT Settings
    bd_table[index_ep0_out_even].cnt           = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep0_out_even].addr          = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_out_even);
    bd_table[index_ep0_out_even].bd_flag._byte = 0x80; //0xc0; //TOGGLE_DATA1; // OWN_SIE;

    // EP0 OUT BDT Settings
    bd_table[index_ep0_out_odd].cnt           = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep0_out_odd].addr          = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_out_odd);
    bd_table[index_ep0_out_odd].bd_flag._byte = 0x80; //0xc0; //TOGGLE_DATA1; // OWN_SIE;

    // EP0 IN BDT Settings
    bd_table[index_ep0_in_even].cnt           = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep0_in_even].addr          = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_in_even);
    bd_table[index_ep0_in_even].bd_flag._byte = OWN_MCU; // TOGGLE_DATA0; //OWN_MCU;
    // EP0 IN BDT Settings
    bd_table[index_ep0_in_odd].cnt            = (BUF_SIZE_OF_ONE_EP);
    bd_table[index_ep0_in_odd].addr           = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_in_odd);
    bd_table[index_ep0_in_odd].bd_flag._byte  = OWN_MCU;  // TOGGLE_DATA0; //OWN_MCU;
}
static void s_handler_stall(void)
{
    if(USB0->ENDPOINT[0].ENDPT & USB_ENDPT_EPSTALL_MASK)
        USB0->ENDPOINT[0].ENDPT &= ~USB_ENDPT_EPSTALL_MASK;

    USB0->ISTAT |= USB_ISTAT_STALL_MASK;
}
static void s_handler_reset(void)
{

}

static void s_handler_token(void)
{
    S_USB_PARA usb_para;

    // load pid, buf, len
    load_rx_para(&usb_para);

    if(USB0->STAT & 0x08)
        // tx
        cdc_entry(&usb_para, direction_tx);
        // usb_stack_device_entry_tx(ep);
    else
    {
        // rx
        cdc_entry(&usb_para, direction_rx);
    }

        /*
        usb_stack_device_entry_rx(ep,
                                  bd_table[index_ep0_out_even + ep *4 + 0].bd_flag.flag2.pid,
                                  get_ep_rx_buf(ep),
                                  get_data_len(ep));
                                  // */

    USB0->ISTAT |=  USB_ISTAT_TOKDNE_MASK;
    USB0->CTL   &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
    //bd_table[index_ep0_out_even].bd_flag._byte = TOGGLE_DATA0;
}
static void s_usb_clock_init(void)
{
    // USB clock src = PLL
    SIM->SOPT2  = 0x04050000;

    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
}
static void s_usb_reg_init(void)
{
    SIM->SOPT1 |= (1u<<SIM_SOPT1_USBREGEN_SHIFT);
}
// ------------------------------------------------
int tt_cnt = 0;
void driver_usb_send(uint8_t ep, uint8_t *buf, uint32_t buf_len)
{
    uint8_t  *p;
    uint32_t len;
    uint32_t ep_entry;
tt_cnt++;
    /*
        TX         ODD
        USB->M     0
        USB->M     1
        M->USB     0
        M->USB     1
    */
    ep_in    = ep;
    ep_entry = ep*4 + 2 + usb_tx_odd[ep];
    usb_tx_odd[ep] ^= 1;

    p = ep_buf + ep_entry*BUF_SIZE_OF_ONE_EP;  // 32 byte each ep.

    // give buf and buf len.
    load_to_send_buffer_manager(buf, buf_len);

    // get then len for sending
    len = load_from_send_buffer_manager();

    //if(len == 0)
    //    return;

    bd_table[ep_entry].cnt = (unsigned short)len;

    // copy data to tx buffer
    while(len--)
         *p++ = *p_data++;

    if(ep == 0)
    {
        // control point
        bd_table[ep_entry].bd_flag._byte = toggle_data;
        update_toggle_data();
    }

    if(ep == ep_in)
    {
        // send data to CDC
        update_toggle_data();
        bd_table[ep_entry].bd_flag._byte = toggle_data;
    }
}


void driver_usb_send_continous(uint8_t ep)
{
    uint8_t  *p;
    uint32_t  len;
    uint32_t  ep_entry;

    /*
        TX         ODD
        USB->M     0
        USB->M     1
        M->USB     0
        M->USB     1
    */

    ep_entry = ep*4 + 2 + usb_tx_odd[ep];
    usb_tx_odd[ep] ^= 1;

    p = ep_buf + ep_entry*BUF_SIZE_OF_ONE_EP;  // 32 byte each ep.

    // get then len for sending
    len = load_from_send_buffer_manager();

    if(len == 0)
        return;

    bd_table[ep_entry].cnt = (unsigned short)len;

    // copy data to tx buffer
    while(len--)
        *p++ = *p_data++;

    // send data to CDC
    update_toggle_data();
    bd_table[ep_entry].bd_flag._byte = toggle_data;
}

void driver_usb_set_interface(void)
{
    /* EndPoint Register settings */
    USB0->ENDPOINT[1].ENDPT = USB_ENDPT_EPTXEN_MASK  | USB_ENDPT_EPHSHK_MASK; // direction = in
    USB0->ENDPOINT[2].ENDPT = USB_ENDPT_EPTXEN_MASK  | USB_ENDPT_EPHSHK_MASK; // direction = in
    USB0->ENDPOINT[3].ENDPT = USB_ENDPT_EPRXEN_MASK  | USB_ENDPT_EPHSHK_MASK; // direction = out

    // EP0 OUT BDT Settings
    bd_table[index_ep0_out_even].cnt           = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep0_out_even].addr          = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_out_even);
    bd_table[index_ep0_out_even].bd_flag._byte = OWN_SIE;
    // EP0 OUT BDT Settings
    bd_table[index_ep0_out_odd].cnt           = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep0_out_odd].addr          = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_out_odd);
    bd_table[index_ep0_out_odd].bd_flag._byte = OWN_SIE;

    /* EndPoint 1 BDT Settings*/
    bd_table[index_ep1_in_even].cnt           = 0x0;
    bd_table[index_ep1_in_even].addr          = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep1_in_even);
    bd_table[index_ep1_in_even].bd_flag._byte = OWN_MCU;

    /* EndPoint 2 BDT Settings*/
    bd_table[index_ep2_in_even].cnt           = 0x0;
    bd_table[index_ep2_in_even].addr          = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep2_in_even);
    bd_table[index_ep2_in_even].bd_flag._byte = OWN_MCU;

    /* EndPoint 3 BDT Settings*/
    bd_table[index_ep3_out_even].cnt           = 0xFF;
    bd_table[index_ep3_out_even].addr          = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep3_out_even);
    bd_table[index_ep3_out_even].bd_flag._byte = OWN_SIE;
}

void driver_set_addr(uint8_t addr)
{
    USB0->ADDR = addr;
}

void driver_usb_init(void)
{
    s_usb_reg_init();
    s_usb_clock_init();
    hal_nvic_enable_irq(USB_IRQ_NUMBER);

    s_bd_init();
    memset(usb_tx_odd, 0, sizeof(usb_tx_odd));

    // Set BDT Base Register
    USB0->BDTPAGE1 = (uint8_t)((uint32_t)bd_table>>8);
    USB0->BDTPAGE2 = (uint8_t)((uint32_t)bd_table>>16);
    USB0->BDTPAGE3 = (uint8_t)((uint32_t)bd_table>>24);


    USB0->INTEN |= USB_INTEN_TOKDNEEN_MASK;

    // Enable EP0
    USB0->ENDPOINT[0].ENDPT = 0x0D;

    // Disable weak pull downs
    USB0->USBCTRL &= ~(uint8_t)(USB_USBCTRL_PDE_MASK | USB_USBCTRL_SUSP_MASK);

    // enable usb module
    USB0->CTL |= 0x01;

    // Pull up enable
    USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
}

void driver_usb_notify_next_rx(uint8_t ep)
{
    bd_table[ep<<2].cnt = BUF_SIZE_OF_ONE_EP;
    bd_table[ep<<2].bd_flag._byte = OWN_SIE;
}
uint32_t driver_usb_get_data_len1(uint8_t ep)
{
    return bd_table[(ep<<2)+1].cnt;
}
void driver_usb_set_ep_in(uint8_t ep)
{
    ep_in = ep;
}
void driver_usb_notify_get_data(uint8_t ep)
{
    (bd_table[ep<<2].bd_flag._byte = OWN_MCU);
}
void driver_usb_isr(void)
{
    // do not use if... else if... else if here.

    if(USB0->ISTAT & USB_ISTAT_TOKDNE_MASK)
    {
        s_handler_token();
        USB0->ISTAT |= USB_ISTAT_TOKDNE_MASK;
    }

    if(USB0->ISTAT & USB_ISTAT_STALL_MASK)
    {
        s_handler_stall();
        USB0->ISTAT |= USB_ISTAT_STALL_MASK;
    }

    if(USB0->ISTAT & USB_ISTAT_USBRST_MASK)
    {
        USB0->ISTAT |= USB_ISTAT_USBRST_MASK;
    }

    if(USB0->ISTAT & USB_ISTAT_SLEEP_MASK)
    {
        USB0->ISTAT |= USB_ISTAT_SLEEP_MASK;
    }

    if(USB0->ISTAT & USB_ISTAT_ERROR_MASK)
    {
        USB0->ISTAT |= USB_ISTAT_ERROR_MASK;
    }
}

void usb0_handler(void)
{
    driver_usb_isr();
}













