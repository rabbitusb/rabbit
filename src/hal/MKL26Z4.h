

// -----------------------------------------------
// SIM
// -----------------------------------------------
typedef struct
{
    uint32_t SOPT1;
    uint32_t SOPT1CFG;
    uint8_t  RESERVED_0[4092];
    uint32_t SOPT2;
    uint8_t  RESERVED_1[4];
    uint32_t SOPT4;
    uint32_t SOPT5;
    uint8_t  RESERVED_2[4];
    uint32_t SOPT7;
    uint8_t  RESERVED_3[8];
    uint32_t SDID;
    uint8_t  RESERVED_4[12];
    uint32_t SCGC4;
    uint32_t SCGC5;
    uint32_t SCGC6;
    uint32_t SCGC7;
    uint32_t CLKDIV1;
    uint8_t  RESERVED_5[4];
    uint32_t FCFG1;
    uint32_t FCFG2;
    uint8_t  RESERVED_6[4];
    uint32_t UIDMH;
    uint32_t UIDML;
    uint32_t UIDL;
    uint8_t  RESERVED_7[156];
    uint32_t COPC;
    uint32_t SRVCOP;
} SIM_Type;


#define SIM_BASE (0x40047000u)
#define SIM      ((SIM_Type *)SIM_BASE)

#define SIM_SOPT2_UART0SRC_MASK                  (0xC000000U)
#define SIM_SOPT2_UART0SRC_SHIFT                 (26U)
#define SIM_SOPT2_UART0SRC(x)                    (((uint32_t)(((uint32_t)(x)) << SIM_SOPT2_UART0SRC_SHIFT)) & SIM_SOPT2_UART0SRC_MASK)

#define SIM_SOPT5_UART0TXSRC_MASK                (0x3U)
#define SIM_SOPT5_UART0TXSRC_SHIFT               (0U)
#define SIM_SOPT5_UART0TXSRC(x)                  (((uint32_t)(((uint32_t)(x)) << SIM_SOPT5_UART0TXSRC_SHIFT)) & SIM_SOPT5_UART0TXSRC_MASK)
#define SIM_SOPT5_UART0RXSRC_MASK                (0x4U)
#define SIM_SOPT5_UART0RXSRC_SHIFT               (2U)
#define SIM_SOPT5_UART0RXSRC(x)                  (((uint32_t)(((uint32_t)(x)) << SIM_SOPT5_UART0RXSRC_SHIFT)) & SIM_SOPT5_UART0RXSRC_MASK)

#define SIM_SCGC4_UART0_MASK                     (0x400U)
#define SIM_SCGC5_PORTA_MASK                     (0x200U)


// -----------------------------------------------
// PORT
// -----------------------------------------------
typedef struct {
    uint32_t PCR[32];
    uint32_t GPCLR;
    uint32_t GPCHR;
    uint8_t RESERVED_0[24];
    uint32_t ISFR;
} PORT_Type;

#define PORTA_BASE                               (0x40049000u)
#define PORTA                                    ((PORT_Type *)PORTA_BASE)
#define PORT_PCR_MUX_MASK                        (0x700U)
#define PORT_PCR_MUX_SHIFT                       (8U)
#define PORT_PCR_MUX(x)                          (((uint32_t)(((uint32_t)(x)) << PORT_PCR_MUX_SHIFT)) & PORT_PCR_MUX_MASK)


// -----------------------------------------------
// UART
// -----------------------------------------------
typedef struct
{
    uint8_t BDH;
    uint8_t BDL;
    uint8_t C1;
    uint8_t C2;
    uint8_t S1;
    uint8_t S2;
    uint8_t C3;
    uint8_t D;
    uint8_t MA1;
    uint8_t MA2;
    uint8_t C4;
    uint8_t C5;
} UART0_Type;

#define UART0_C2_TE_MASK                         (0x8U)
#define UART0_C2_RE_MASK                         (0x4U)
#define UART0_C4_OSR_MASK                        (0x1FU)
#define UART0_BDH_SBR_MASK                       (0x1FU)
#define UART0_BDH_SBNS_MASK                      (0x20U)
#define UART0_BDH_SBNS_SHIFT                     (5U)
#define UART0_BDH_SBNS(x)                        (((uint8_t)(((uint8_t)(x)) << UART0_BDH_SBNS_SHIFT)) & UART0_BDH_SBNS_MASK)
#define UART0_S1_TDRE_MASK                       (0x80U)

#define UART0_BASE                               (0x4006A000u)
#define UART0                                    ((UART0_Type *)UART0_BASE)

// -----------------------------------------------
// OSC
// -----------------------------------------------
typedef struct
{
    uint8_t CR;                                 /**< OSC Control Register, offset: 0x0 */
} OSC_Type;
#define OSC0_BASE                                (0x40065000u)
#define OSC0                                     ((OSC_Type *)OSC0_BASE)


// -----------------------------------------------
// MCG
// -----------------------------------------------
typedef struct
{
    uint8_t C1;
    uint8_t C2;
    uint8_t C3;
    uint8_t C4;
    uint8_t C5;
    uint8_t C6;
    uint8_t S;
    uint8_t RESERVED_0[1];
    uint8_t SC;
    uint8_t RESERVED_1[1];
    uint8_t ATCVH;
    uint8_t ATCVL;
    uint8_t C7;
    uint8_t C8;
    uint8_t C9;
    uint8_t C10;
} MCG_Type;


#define MCG_C2_LP_MASK                           (0x2U)
#define MCG_S_CLKST_MASK                         (0xCU)
#define MCG_S_CLKST_SHIFT                        (2U)
#define MCG_S_CLKST(x)                           (((uint8_t)(((uint8_t)(x)) << MCG_S_CLKST_SHIFT)) & MCG_S_CLKST_MASK)
#define MCG_S_CLKST_VAL                          ((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT)
#define MCG_S_IRCST_MASK                         (0x1U)
#define MCG_S_IRCST_SHIFT                        (0U)
#define MCG_S_IRCST_VAL                         ((MCG->S & MCG_S_IRCST_MASK) >> MCG_S_IRCST_SHIFT)
#define MCG_S_IREFST_MASK                        (0x10U)
#define MCG_S_IREFST_SHIFT                       (4U)
#define MCG_S_IREFST(x)                          (((uint8_t)(((uint8_t)(x)) << MCG_S_IREFST_SHIFT)) & MCG_S_IREFST_MASK)
#define MCG_S_LOCK0_MASK                         (0x40U)
#define MCG_S_OSCINIT0_MASK                      (0x2U)
#define MCG_S_PLLST_MASK                         (0x20U)


#define MCG_BASE                                 (0x40064000u)
#define MCG                                      ((MCG_Type *)MCG_BASE)

