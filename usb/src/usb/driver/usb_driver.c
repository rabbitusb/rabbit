

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "..\\..\\hal\\MKL26Z4.h"      // register definition
#include "..\\..\\hal\\hal_nvic.h"     // use to enable IRQ

#include "..\\app\\rabbit_usb.h"        // app callback importing
#include "..\\hal\\usb_hal.h"           // need to construct S_USB_PARA for rabbit stack
#include "..\\class\\usb_cls_cdc_dev.h" // reset class
#include "..\\usb_core_dev.h"           // core entry call back
#include "..\\usb_debug.h"              // debug printf
#include "..\\usb_spec.h"               // use to judge PID
#include "usb_driver.h"                 // need to use api in itself

#define USB_IRQ_NUMBER 0x18

/*
    one BD, 8 bytes
    one directtion has two BD for odd and event, 16 bytes
    one end point has two direction, then 32 bytes
    up to 16 end point, 32*16 = 512 bytes
*/

#define EP_PACKAGE_SIZE 8 //64
#define BUF_SIZE_OF_ONE_EP EP_PACKAGE_SIZE



#define OWN_MCU 0x00
#define OWN_SIE 0x80

#define TOGGLE_DATA0 0x80
#define TOGGLE_DATA1 0xc0

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
        uint8_t bstall:1;              // buffer stall enable
        uint8_t dts:1;                 // data toggle synch enable
        uint8_t ninc:1;                // address increment disable
        uint8_t keep:1;                // bd keep enable
        uint8_t data:1;                // data toggle synch value
        uint8_t uown:1;                // usb ownership
    }flag1;

    struct
    {
        uint8_t    :2;
        uint8_t pid:4;
        uint8_t    :2;
    }flag2;
} U_BD_FLAG; // bd status register

typedef struct
{
    U_BD_FLAG  bd_flag;
    uint8_t    dummy;
    uint16_t   cnt;
    uint32_t   addr;
} S_BD;

static uint8_t toggle_data;

__attribute__((aligned(512))) static S_BD bd_table[16]; // for 4 EP

#define EP_COUNT             4
#define BUF_COUNT_IN_EACH_EP 4

static uint8_t   ep_buf[BUF_SIZE_OF_ONE_EP * EP_COUNT * BUF_COUNT_IN_EACH_EP];
static uint8_t * p_data;
static uint32_t  send_count;

// ------------------------------------------------
static void update_toggle_data(void)
{
    if(toggle_data == TOGGLE_DATA0)
        toggle_data = TOGGLE_DATA1;
    else
        toggle_data = TOGGLE_DATA0;
}

void driver_usb0_set_toggle_data0(void)
{
    toggle_data = TOGGLE_DATA0;
}

void driver_usb0_set_toggle_data1(void)
{
    toggle_data = TOGGLE_DATA1;
}

static void s_store(uint8_t *data, uint32_t size)
{
    // load happens when counter is 0
    p_data     = data;
    send_count = size;
}

static uint8_t s_load(void)
{
    uint32_t r;

    if(send_count > EP_PACKAGE_SIZE)
    {
        // BUF_SIZE_OF_ONE_EP is the maximum package length for one transaction
        r           = EP_PACKAGE_SIZE;
        send_count -= EP_PACKAGE_SIZE;
    }
    else
    {
        r          = send_count;
        send_count = 0;
    }

    return r;
}
static uint8_t s_ep_to_index(uint8_t ep)
{
    uint8_t index;
    if(ep == 0)
        index = index_ep0_out_even;
    else if(ep == 2)
        index = index_ep2_out_even;

    return index;
}
static void s_load_rx_para(S_USB_PARA * usb_para)
{
    uint8_t index;

    usb_para->usb       = usb0;
    usb_para->ep        = (USB0->STAT>>USB_STAT_ENDP_SHIFT) & 0x0f;
    index               = s_ep_to_index(usb_para->ep);
    usb_para->buf       = ep_buf + BUF_SIZE_OF_ONE_EP * index;

    if((USB0->STAT>>USB_STAT_TX_SHIFT) & 0x01)
    {
        // tx
        usb_para->pid = PID_IN;
        usb_para->len = 0;
    }
    else
    {
        // rx
        usb_para->pid = bd_table[index].bd_flag.flag2.pid;
        usb_para->len = bd_table[index].cnt;
    }
}
void driver_usb0_rx_next(uint8_t ep)
{
    uint8_t index;

    index = s_ep_to_index(ep);
    bd_table[index].cnt = 8;
    #define OWN_USB_DATA0 0x80

    debug_record_string(" index:");
    debug_record((char*)&(index), 1);
    bd_table[index].bd_flag._byte = OWN_USB_DATA0;
}
static void s_bd_init(void)
{
    // EP0 OUT, rx
    bd_table[index_ep0_out_even].cnt           = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep0_out_even].addr          = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_out_even);
    bd_table[index_ep0_out_even].bd_flag._byte = OWN_SIE;

    // EP0 OUT, rx
    bd_table[index_ep0_out_odd].cnt            = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep0_out_odd].addr           = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_out_odd);
    bd_table[index_ep0_out_odd].bd_flag._byte  = OWN_MCU;

    // EP0 IN, tx
    bd_table[index_ep0_in_even].cnt            = 0;
    bd_table[index_ep0_in_even].addr           = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_in_even);
    bd_table[index_ep0_in_even].bd_flag._byte  = OWN_MCU;

    // EP0 IN, tx
    bd_table[index_ep0_in_odd].cnt             = 0;
    bd_table[index_ep0_in_odd].addr            = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep0_in_odd);
    bd_table[index_ep0_in_odd].bd_flag._byte   = OWN_MCU;
    // ---------------------------------------------------------------------------------------------------

    // EP1 OUT, rx
    bd_table[index_ep1_out_even].cnt           = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep1_out_even].addr          = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep1_out_even);
    bd_table[index_ep1_out_even].bd_flag._byte = OWN_SIE;

    // EP1 OUT, rx
    bd_table[index_ep1_out_odd].cnt            = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep1_out_odd].addr           = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep1_out_odd);
    bd_table[index_ep1_out_odd].bd_flag._byte  = OWN_MCU;

    // EP1 IN, tx
    bd_table[index_ep1_in_even].cnt            = 0;
    bd_table[index_ep1_in_even].addr           = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep1_in_even);
    bd_table[index_ep1_in_even].bd_flag._byte  = OWN_MCU;

    // EP1 IN, tx
    bd_table[index_ep1_in_odd].cnt             = 0;
    bd_table[index_ep1_in_odd].addr            = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep1_in_odd);
    bd_table[index_ep1_in_odd].bd_flag._byte   = OWN_MCU;
    // ---------------------------------------------------------------------------------------------------

    // EP2 OUT, rx
    bd_table[index_ep2_out_even].cnt           = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep2_out_even].addr          = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep2_out_even);
    bd_table[index_ep2_out_even].bd_flag._byte = OWN_SIE;

    // EP2 OUT, rx
    bd_table[index_ep2_out_odd].cnt            = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep2_out_odd].addr           = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep2_out_odd);
    bd_table[index_ep2_out_odd].bd_flag._byte  = OWN_MCU;

    // EP2 IN, tx
    bd_table[index_ep2_in_even].cnt            = 0;
    bd_table[index_ep2_in_even].addr           = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep2_in_even);
    bd_table[index_ep2_in_even].bd_flag._byte  = OWN_MCU;

    // EP2 IN, tx
    bd_table[index_ep2_in_odd].cnt             = 0;
    bd_table[index_ep2_in_odd].addr            = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep2_in_odd);
    bd_table[index_ep2_in_odd].bd_flag._byte   = OWN_MCU;
    // ---------------------------------------------------------------------------------------------------

    // EP3 OUT, rx
    bd_table[index_ep3_out_even].cnt           = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep3_out_even].addr          = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep3_out_even);
    bd_table[index_ep3_out_even].bd_flag._byte = OWN_SIE;

    // EP3 OUT, rx
    bd_table[index_ep3_out_odd].cnt            = BUF_SIZE_OF_ONE_EP;
    bd_table[index_ep3_out_odd].addr           = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep3_out_odd);
    bd_table[index_ep3_out_odd].bd_flag._byte  = OWN_MCU;

    // EP3 IN, tx
    bd_table[index_ep3_in_even].cnt            = 0;
    bd_table[index_ep3_in_even].addr           = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep3_in_even);
    bd_table[index_ep3_in_even].bd_flag._byte  = OWN_MCU;

    // EP3 IN, tx
    bd_table[index_ep3_in_odd].cnt             = 0;
    bd_table[index_ep3_in_odd].addr            = (uint32_t)(ep_buf + BUF_SIZE_OF_ONE_EP*index_ep3_in_odd);
    bd_table[index_ep3_in_odd].bd_flag._byte   = OWN_MCU;
}

static void s_handler_reset(void)
{
    send_count = 0;
    driver_usb0_set_addr(0);
    cdc_dev_init();
}

static void s_handler_token(void)
{
    S_USB_PARA usb_para;

    s_load_rx_para(&usb_para);
    usb_core_dev_entry(&usb_para);

    USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK; // allow SIE receive new data
}
static void s_usb_clock_init(void)
{
    // USB clock src = PLL
    SIM->SOPT2  = 0x04050000;

    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
}
static void s_usb_regulator_init(void)
{
    SIM->SOPT1 |= (1u<<SIM_SOPT1_USBREGEN_SHIFT);
}

// ------------------------------------------------
/*
    return the bytes send out
*/
uint32_t driver_usb0_send(uint8_t ep, uint8_t *buf, uint32_t buf_len)
{
    uint8_t  *p;
    uint32_t len;
    uint32_t ep_entry;

    if(send_count > 0)
        return 0;

    ep_entry = ep*4 + 2;
    p = ep_buf + ep_entry*BUF_SIZE_OF_ONE_EP;  // 32 byte each ep.

    s_store(buf, buf_len);
    len = s_load();

    debug_record_string(" load len:");
    debug_record((char*)&len, 1);

    bd_table[ep_entry].cnt = (unsigned short)len;

    // copy data to tx buffer
    while(len--)
         *p++ = *p_data++;

    if(ep == 0)
    {
        // control ep
        debug_record_string("tg: ");
        debug_record((char*)&toggle_data, 1);
        bd_table[ep_entry].bd_flag._byte = toggle_data;
        update_toggle_data();
    }
    else if(ep == 2)
    {
        // cdc data ep
        driver_usb0_set_toggle_data0();
        bd_table[ep_entry].bd_flag._byte = toggle_data;
        update_toggle_data();
    }

    return buf_len;
}


void driver_usb0_send_continous(uint8_t ep)
{
    uint8_t  *p;
    uint32_t  len;
    uint32_t  ep_entry;

    debug_record_string("send_c,");
    /*
        DIR        ODD
        USB->M     0
        USB->M     1
        M->USB     0
        M->USB     1
    */

    ep_entry = ep*4 + 2;
    debug_record_string("ep_en: ");
    debug_record((char*)&ep_entry, 1);

    p = ep_buf + ep_entry*BUF_SIZE_OF_ONE_EP;  // 32 byte each ep.

    // get then len for sending
    len = s_load();

    if(len == 0)
    {
        debug_record_string("none.");
        return;
    }
    else
        debug_record((char*)&len, 1);

    bd_table[ep_entry].cnt = (unsigned short)len;

    // copy data to tx buffer
    while(len--)
        *p++ = *p_data++;

    // send data to host
    debug_record_string(" tgl: ");
    debug_record((char*)&toggle_data, 1);
    bd_table[ep_entry].bd_flag._byte = toggle_data;
    update_toggle_data();
}

void driver_usb0_set_addr(uint8_t addr)
{
    USB0->ADDR = addr;
}


void driver_usb0_init(void)
{
    s_usb_regulator_init();
    s_usb_clock_init();
    hal_nvic_enable_irq(USB_IRQ_NUMBER);
    s_bd_init();
    send_count = 0;

    // Set BDT Base Register
    USB0->BDTPAGE1 = (uint8_t)((uint32_t)bd_table>>8);
    USB0->BDTPAGE2 = (uint8_t)((uint32_t)bd_table>>16);
    USB0->BDTPAGE3 = (uint8_t)((uint32_t)bd_table>>24);

    USB0->INTEN |= USB_INTEN_TOKDNEEN_MASK | USB_INTEN_USBRSTEN_MASK;

    // Enable EP0
    USB0->ENDPOINT[0].ENDPT = 0x0D;
    // Enable EP1
    USB0->ENDPOINT[1].ENDPT = 0x0D;
    // Enable EP2
    USB0->ENDPOINT[2].ENDPT = 0x0D;
    // Enable EP3
    USB0->ENDPOINT[3].ENDPT = 0x0D;

    // Disable weak pull downs
    USB0->USBCTRL &= ~(uint8_t)(USB_USBCTRL_PDE_MASK | USB_USBCTRL_SUSP_MASK);

    // Reset ODD
    USB0->CTL |= USB_CTL_ODDRST_MASK;

    // Enable usb module
    USB0->CTL |= 0x01;

    // Pull up enable
    USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
}

void driver_usb0_isr(void)
{
    // do not use if... else if... else if here.

    debug_record_string("\r\n isr, ");
    debug_record((char*)(&(USB0->ISTAT)), 1 );
    debug_record_string(" ");

    if(USB0->ISTAT & USB_ISTAT_USBRST_MASK)
    {
        debug_record_string("reset, ");
        USB0->ISTAT |= 0xff;
        s_handler_reset();
        return;
    }

    if(USB0->ISTAT & USB_ISTAT_TOKDNE_MASK)
    {
        debug_record_string(" tk, ");
        s_handler_token();
        USB0->ISTAT |= USB_ISTAT_TOKDNE_MASK;
    }

    asm(" nop");
    asm(" nop");
    asm(" nop");
    asm(" nop");
}

void usb0_handler(void)
{
    driver_usb0_isr();
}













