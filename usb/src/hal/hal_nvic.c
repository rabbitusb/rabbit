

#include "stdint.h"

typedef struct NVIC_MemMap
{
    uint32_t ISER[4];
    uint8_t RESERVED_0[112];
    uint32_t ICER[4];
    uint8_t RESERVED_1[112];
    uint32_t ISPR[4];
    uint8_t RESERVED_2[112];
    uint32_t ICPR[4];
    uint8_t RESERVED_3[112];
    uint32_t IABR[4];
    uint8_t RESERVED_4[240];
    uint8_t IP[104];
    uint8_t RESERVED_5[2712];
    uint32_t STIR[1];
} volatile *NVIC_MemMapPtr;

#define NVIC_ICER_REG(base,index)                ((base)->ICER[index])
#define NVIC_ICPR_REG(base,index)                ((base)->ICPR[index])
#define NVIC_ISER_REG(base,index)                ((base)->ISER[index])
#define NVIC_IP_REG(base,index)                  ((base)->IP[index])

#define NVICICPR0                                NVIC_ICPR_REG(NVIC_BASE_PTR,0)
#define NVICISER0                                NVIC_ISER_REG(NVIC_BASE_PTR,0)
#define NVICICER0                                NVIC_ICER_REG(NVIC_BASE_PTR,0)
#define NVIC_BASE_PTR                            ((NVIC_MemMapPtr)0xE000E100u)
#define NVICIP0                                  NVIC_IP_REG(NVIC_BASE_PTR,0)

/*
    irq_n: for example, irq_n should be 0 for DMA0
*/
void hal_nvic_enable_irq(int irq_n)
{
    unsigned int offset;

    offset = irq_n>>5;

    *(&NVICICPR0 + offset) |= 1<<(irq_n&0x1f);
    *(&NVICISER0 + offset) |= 1<<(irq_n&0x1f);
}
void hal_nvic_disable_irq(int irq_n)
{
    unsigned int offset;

    offset = irq_n>>5;

    *(&NVICICPR0 + offset) |= 1<<(irq_n&0x1f);
    *(&NVICICER0 + offset) |= 1<<(irq_n&0x1f);
}

/*
    priority should be within 0 - 15
*/
void hal_nvic_set_priority(int irq_n, int priority)
{
    *(unsigned char*)(((int)&NVICIP0) + irq_n) = priority<<4;
}




