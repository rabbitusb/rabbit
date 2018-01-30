

#include <stdint.h>
#include <stddef.h>
#include "..\\hal\\MKL26Z4.h"
#include "..\\hal\\hal_nvic.h"
#include "usb_driver.h"
#include "usb_cdc.h"


#define USB_IRQ_NUMBER 0x18

/*
    one BD, 8 bytes
    one directtion has two BD for odd and event, 16 bytes
    one end point has two direction, then 32 bytes
    up to 16 end point, 32*16 = 512 bytes
*/
#define BUF_SIZE_OF_ONE_EP 32

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
    unsigned char _byte;
    struct
    {
        unsigned char :1;
        unsigned char :1;
        unsigned char bstall:1;              //buffer stall enable
        unsigned char dts:1;                 //data toggle synch enable
        unsigned char ninc:1;                //address increment disable
        unsigned char keep:1;                //bd keep enable
        unsigned char data:1;                //data toggle synch value
        unsigned char uown:1;                //usb ownership
    }flag1;

    struct
    {
        unsigned char    :2;
        unsigned char pid:4;                 //Packet Identifier
        unsigned char    :2;
    }flag2;
} U_BD_FLAG;                         //Buffer Descriptor Status Register

typedef struct
{
    U_BD_FLAG     bd_flag;
    unsigned char dummy;
    uint16_t      cnt;
    unsigned int  addr;
} S_BD;

#define TOGGLE_UDATA0   0x88
#define TOGGLE_UDATA1   0xC8
static unsigned char toggle_data;


__attribute__((aligned(512))) static S_BD bd_table[16]; // for 4 EP

#define EP_COUNT             4
#define BUF_COUNT_IN_EACH_EP 4

static unsigned char   ep_buf[BUF_SIZE_OF_ONE_EP * EP_COUNT * BUF_COUNT_IN_EACH_EP];
static unsigned char * p_data;
static unsigned int    counter;
static unsigned char   ep_in;

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

static void load_to_send_buffer_manager(unsigned char *data,unsigned int size)
{
    if(counter == 0)
    {
        // load happens when counter is 0
        p_data  = data;
        counter = size;
    }
}

static unsigned char load_from_send_buffer_manager(void)
{
    unsigned int r;

    if(counter > BUF_SIZE_OF_ONE_EP)
    {
        // BUF_SIZE_OF_ONE_EP is the maximum package length for one transaction
        r        = BUF_SIZE_OF_ONE_EP;
        counter -= BUF_SIZE_OF_ONE_EP;
    }
    else
    {
        r       = counter;
        counter = 0;
    }

    return r;
}

static void handler_stall(void)
{
    if(USB0->ENDPOINT[0].ENDPT & USB_ENDPT_EPSTALL_MASK)
        USB0->ENDPOINT[0].ENDPT &= ~USB_ENDPT_EPSTALL_MASK;

    USB0->ISTAT |= USB_ISTAT_STALL_MASK;
}

static void handler_reset(void)
{
    /* Disable all data EP registers */
    USB0->ENDPOINT[1].ENDPT = 0x00;
    USB0->ENDPOINT[2].ENDPT = 0x00;
    USB0->ENDPOINT[3].ENDPT = 0x00;

    /* EP0 BDT Setup */
    // EP0 OUT BDT Settings
    bd_table[index_ep0_out_even].cnt           = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep0_out_even].addr          = (unsigned int)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_out_even);
    bd_table[index_ep0_out_even].bd_flag._byte = TOGGLE_DATA1;
    // EP0 OUT BDT Settings
    bd_table[index_ep0_out_odd].cnt           = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep0_out_odd].addr          = (unsigned int)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_out_odd);
    bd_table[index_ep0_out_odd].bd_flag._byte = TOGGLE_DATA1;
    // EP0 IN BDT Settings
    bd_table[index_ep0_in_even].cnt           = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep0_in_even].addr          = (unsigned int)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_in_even);
    bd_table[index_ep0_in_even].bd_flag._byte = TOGGLE_DATA0;
    // EP0 IN BDT Settings
    bd_table[index_ep0_in_odd].cnt           = (BUF_SIZE_OF_ONE_EP);
    bd_table[index_ep0_in_odd].addr          = (unsigned int)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_in_odd);
    bd_table[index_ep0_in_odd].bd_flag._byte = TOGGLE_DATA0;

    // Enable EP0
    USB0->ENDPOINT[0].ENDPT = 0x0D;

    // Clear all Error flags
    USB0->ERRSTAT = 0xFF;

    // CLear all USB ISR flags
    USB0->ISTAT = 0xFF;

    // Set default Address
    USB0->ADDR = 0x00;

    // Enable all error sources
    USB0->ERREN = 0xFF;

    // USB Interrupt Enablers
    USB0->INTEN |= USB_INTEN_TOKDNEEN_MASK |
                  USB_INTEN_ERROREN_MASK  |
                  USB_INTEN_USBRSTEN_MASK;
}

static void usb_clock_init(void)
{
    //USB0->CLK_RECOVER_IRC_EN |= USB_CLK_RECOVER_IRC_EN_REG_EN_MASK |
    //                            USB_CLK_RECOVER_IRC_EN_IRC_EN_MASK;
    //USB0->CLK_RECOVER_CTRL |= USB_CLK_RECOVER_CTRL_CLOCK_RECOVER_EN_MASK;
//
    SIM->SOPT1 |= SIM_SOPT1_USBREGEN_MASK;
    SIM->SOPT1 &= ~(SIM_SOPT1_USBVSTBY_MASK | SIM_SOPT1_USBSSTBY_MASK);
}

static unsigned char * get_ep_rx_buf(unsigned char ep)
{
    int index = 0;

    switch(ep)
    {
        case 0:
            index = index_ep0_out_even;
            break;
        case 3:
            index = index_ep3_out_even;
            break;
        default:
            return NULL;
    }

    return ep_buf + BUF_SIZE_OF_ONE_EP*index;
}

static unsigned int get_data_len(unsigned char ep)
{
    return bd_table[ep<<2].cnt;
}

// ------------------------------------------------
void driver_usb_send(unsigned char ep, unsigned char *buf, unsigned int buf_len)
{
    unsigned char  *p;
    unsigned int   len;
    int            ep_entry;

    /*
        TX         ODD
        USB->M     0
        USB->M     1
        M->USB     0
        M->USB     1
    */

    ep_entry = ep*4 + 2;

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


void driver_usb_send_continous(void)
{
    unsigned char  *p;
    unsigned int   len;
    int            ep_entry;

    /*
        TX         ODD
        USB->M     0
        USB->M     1
        M->USB     0
        M->USB     1
    */

    ep_entry = ep_in*4 + 2;

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

    /* EndPoint 1 BDT Settings*/
    bd_table[index_ep1_in_even].bd_flag._byte = OWN_MCU;
    bd_table[index_ep1_in_even].cnt           = 0x0;
    bd_table[index_ep1_in_even].addr          = (unsigned int)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep1_in_even);

    /* EndPoint 2 BDT Settings*/
    bd_table[index_ep2_in_even].bd_flag._byte = OWN_MCU;
    bd_table[index_ep2_in_even].cnt           = 0x0;
    bd_table[index_ep2_in_even].addr          = (unsigned int)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep2_in_even);;

    /* EndPoint 3 BDT Settings*/
    bd_table[index_ep3_out_even].bd_flag._byte = OWN_SIE;
    bd_table[index_ep3_out_even].cnt           = 0xFF;
    bd_table[index_ep3_out_even].addr          = (unsigned int)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep3_out_even);;
}

void driver_set_addr(unsigned char addr)
{
    USB0->ADDR = addr;
}
unsigned char driver_get_pid(void)
{
    return bd_table[index_ep0_out_even].bd_flag.flag2.pid;
}

void driver_usb_init(void)
{
    SIM->SOPT1 |= (1u<<SIM_SOPT1_USBREGEN_SHIFT) |
                  (1u<<SIM_SOPT1_USBSSTBY_SHIFT) |
                  (1u<<SIM_SOPT1_USBVSTBY_SHIFT);

    SIM->SOPT1CFG |= (1u<<SIM_SOPT1CFG_URWE_SHIFT) |
                     (1u<<SIM_SOPT1CFG_USSWE_SHIFT) |
                     (1u<<SIM_SOPT1CFG_UVSWE_SHIFT);

    // Set USB clock to be 48MHz
    usb_clock_init();

    hal_nvic_enable_irq(USB_IRQ_NUMBER);

    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
    // USB clock src = PLL
    SIM->SOPT2  = 0x04050000;

    /* USB Module Configuration */
    // Reset USB Module
    USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
    while(USB0->USBTRC0 & USB_USBTRC0_USBRESET_MASK);

    // Set BDT Base Register
    USB0->BDTPAGE1 = (unsigned char)((unsigned int)bd_table>>8);
    USB0->BDTPAGE2 = (unsigned char)((unsigned int)bd_table>>16);
    USB0->BDTPAGE3 = (unsigned char)((unsigned int)bd_table>>24);

    // Clear USB Reset flag
    USB0->ISTAT |= USB_ISTAT_USBRST_MASK;

    // Enable USB Reset Interrupt
    USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;

    // Enable weak pull downs
    // USB0->USBCTRL = USB_USBCTRL_PDE_MASK;

    // Disable weak pull downs
    USB0->USBCTRL &= ~(unsigned char)(USB_USBCTRL_PDE_MASK | USB_USBCTRL_SUSP_MASK);

    // enable usb module
    USB0->CTL |= 0x01;

    // Pull up enable
    USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;

    //driver_usb_set_interface();
}

void driver_usb_notify_next_rx(unsigned char ep)
{
    bd_table[ep<<2].cnt = BUF_SIZE_OF_ONE_EP;
    bd_table[ep<<2].bd_flag._byte = OWN_SIE;
}
unsigned int driver_usb_get_data_len1(unsigned char ep)
{
    return bd_table[(ep<<2)+1].cnt;
}
void driver_usb_set_ep_in(unsigned char ep)
{
    ep_in = ep;
}
void driver_usb_notify_get_data(unsigned char ep)
{
    (bd_table[ep<<2].bd_flag._byte = OWN_MCU);
}
void driver_usb_isr(void)
{
    unsigned char ep;

    if(USB0->ISTAT & USB_ISTAT_USBRST_MASK)
    {
        handler_reset();
        return;
    }

    if(USB0->ISTAT & USB_ISTAT_STALL_MASK)
    {
        handler_stall();
        return;
    }

    if(USB0->ISTAT & USB_ISTAT_TOKDNE_MASK)
    {
        USB0->CTL |= USB_CTL_ODDRST_MASK;

        ep = USB0->STAT>>4;
        if(USB0->STAT & 0x08)
            // tx
            cdc_entry(ep, direction_tx, get_ep_rx_buf(ep), get_data_len(ep));
        else
            // rx
            cdc_entry(ep, direction_rx, get_ep_rx_buf(ep), get_data_len(ep));


        USB0->ISTAT |= USB_ISTAT_TOKDNE_MASK;
        USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
        bd_table[index_ep0_out_even].bd_flag._byte = TOGGLE_DATA0;
        return;
    }

    if(USB0->ISTAT & USB_ISTAT_SLEEP_MASK)
    {
        USB0->ISTAT |= USB_ISTAT_SLEEP_MASK;
        return;
    }

    if(USB0->ISTAT & USB_ISTAT_ERROR_MASK)
    {
        USB0->ISTAT |= USB_ISTAT_ERROR_MASK;
        return;
    }

}

void usb0_handler(void)
{
    driver_usb_isr();
}













