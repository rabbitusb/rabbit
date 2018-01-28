


#include ".\\hal\\hal_watchdog.h"

int main(void);

void zero_bss_sec(void)
{
    extern int __bss_start__;
    extern int __bss_end__;
    int *p;
    p = &__bss_start__;

    for(p = &__bss_start__; p < &__bss_end__; p++)
    {
        *p = 0;
    }
}

void copy_data_sec(void)
{
    extern int  __etext;
    extern int  __data_start__;
    extern int  __data_end__;

    int *p;
    int *p_const;

    p_const = &__etext;
    for(p = &__data_start__; p < &__data_end__; p++)
    {
        *p = *p_const;
        p_const++;
    }
}


void reset_handler(void)
{
    hal_watchdog_disable();
    zero_bss_sec();
    copy_data_sec();
    main();
}

void default_handler(void)
{
    while(1);
}

/*
    Interrupt vector table placed at 0x0000_0000
*/
extern int __stack_end__;
volatile const int int_vec[] __attribute__((section(".isr_vector"))) =
{
    (const int)(&__stack_end__),// 0x0, SP_RESET
    (const int)reset_handler,   // 0x1, PC_RESET
    (const int)default_handler, // 0x2
    (const int)default_handler, // 0x3
    (const int)default_handler, // 0x4
    (const int)default_handler, // 0x5
    (const int)default_handler, // 0x6
    (const int)default_handler, // 0x7
    (const int)default_handler, // 0x8
    (const int)default_handler, // 0x9
    (const int)default_handler, // 0xa
    (const int)default_handler, // 0xb
    (const int)default_handler, // 0xc
    (const int)default_handler, // 0xd
    (const int)default_handler, // 0xe
    (const int)default_handler, // 0xf
    (const int)default_handler, // 0x10
    (const int)default_handler, // 0x11
    (const int)default_handler, // 0x12
    (const int)default_handler, // 0x13
    (const int)default_handler, // 0x14
    (const int)default_handler, // 0x15
    (const int)default_handler, // 0x16
    (const int)default_handler, // 0x17
    (const int)default_handler, // 0x18
    (const int)default_handler, // 0x19
    (const int)default_handler, // 0x1a
    (const int)default_handler, // 0x1b
    (const int)default_handler, // 0x1c
    (const int)default_handler, // 0x1d
    (const int)default_handler, // 0x1e
    (const int)default_handler, // 0x1f
    (const int)default_handler, // 0x20
    (const int)default_handler, // 0x21
    (const int)default_handler, // 0x22
    (const int)default_handler, // 0x23
    (const int)default_handler, // 0x24
    (const int)default_handler, // 0x25
    (const int)default_handler, // 0x26
    (const int)default_handler, // 0x27
    (const int)default_handler, // 0x28
    (const int)default_handler, // 0x29
    (const int)default_handler, // 0x2a
    (const int)default_handler, // 0x2b
    (const int)default_handler, // 0x2c
    (const int)default_handler, // 0x2d
    (const int)default_handler, // 0x2e
    (const int)default_handler, // 0x2f
    (const int)default_handler, // 0x30
    (const int)default_handler, // 0x31
    (const int)default_handler, // 0x32
    (const int)default_handler, // 0x33
    (const int)default_handler, // 0x34
    (const int)default_handler, // 0x35
    (const int)default_handler, // 0x36
    (const int)default_handler, // 0x37
    (const int)default_handler, // 0x38
    (const int)default_handler, // 0x39
    (const int)default_handler, // 0x3a
    (const int)default_handler, // 0x3b
    (const int)default_handler, // 0x3c
    (const int)default_handler, // 0x3d
    (const int)default_handler, // 0x3e
    (const int)default_handler, // 0x3f
    (const int)default_handler, // 0x40
    (const int)default_handler, // 0x41
    (const int)default_handler, // 0x42
    (const int)default_handler, // 0x43
    (const int)default_handler, // 0x44
    (const int)default_handler, // 0x45
    (const int)default_handler, // 0x46
    (const int)default_handler, // 0x47
    (const int)default_handler, // 0x48
    (const int)default_handler, // 0x49
    (const int)default_handler, // 0x4a
    (const int)default_handler, // 0x4b
    (const int)default_handler, // 0x4c
    (const int)default_handler, // 0x4d
    (const int)default_handler, // 0x4e
    (const int)default_handler, // 0x4f
    (const int)default_handler, // 0x50
    (const int)default_handler, // 0x51
    (const int)default_handler, // 0x52
    (const int)default_handler, // 0x53
    (const int)default_handler, // 0x54
    (const int)default_handler, // 0x55
    (const int)default_handler, // 0x56
    (const int)default_handler, // 0x57
    (const int)default_handler, // 0x58
    (const int)default_handler, // 0x59
    (const int)default_handler, // 0x5a
    (const int)default_handler, // 0x5b
    (const int)default_handler, // 0x5c
    (const int)default_handler, // 0x5d
    (const int)default_handler, // 0x5e
    (const int)default_handler, // 0x5f
    (const int)default_handler, // 0x60
    (const int)default_handler, // 0x61
    (const int)default_handler, // 0x62
    (const int)default_handler, // 0x63
    (const int)default_handler, // 0x64
    (const int)default_handler, // 0x65
    (const int)default_handler, // 0x66
    (const int)default_handler, // 0x67
    (const int)default_handler, // 0x68
    (const int)default_handler, // 0x69
    (const int)default_handler, // 0x6a
    (const int)default_handler, // 0x6b
    (const int)default_handler, // 0x6c
    (const int)default_handler, // 0x6d
    (const int)default_handler, // 0x6e
    (const int)default_handler, // 0x6f
    (const int)default_handler, // 0x70
    (const int)default_handler, // 0x71
    (const int)default_handler, // 0x72
    (const int)default_handler, // 0x73
    (const int)default_handler, // 0x74
    (const int)default_handler, // 0x75
    (const int)default_handler, // 0x76
    (const int)default_handler, // 0x77
    (const int)default_handler, // 0x78
    (const int)default_handler, // 0x79
    (const int)default_handler, // 0x7a
    (const int)default_handler, // 0x7b
    (const int)default_handler, // 0x7c
    (const int)default_handler, // 0x7d
    (const int)default_handler, // 0x7e
    (const int)default_handler, // 0x7f
    (const int)default_handler, // 0x80
    (const int)default_handler, // 0x81
    (const int)default_handler, // 0x82
    (const int)default_handler, // 0x83
    (const int)default_handler, // 0x84
    (const int)default_handler, // 0x85
    (const int)default_handler, // 0x86
    (const int)default_handler, // 0x87
    (const int)default_handler, // 0x88
    (const int)default_handler, // 0x89
    (const int)default_handler, // 0x8a
    (const int)default_handler, // 0x8b
    (const int)default_handler, // 0x8c
    (const int)default_handler, // 0x8d
    (const int)default_handler, // 0x8e
    (const int)default_handler, // 0x8f
    (const int)default_handler, // 0x90
    (const int)default_handler, // 0x91
    (const int)default_handler, // 0x92
    (const int)default_handler, // 0x93
    (const int)default_handler, // 0x94
    (const int)default_handler, // 0x95
    (const int)default_handler, // 0x96
    (const int)default_handler, // 0x97
    (const int)default_handler, // 0x98
    (const int)default_handler, // 0x99
    (const int)default_handler, // 0x9a
    (const int)default_handler, // 0x9b
    (const int)default_handler, // 0x9c
    (const int)default_handler, // 0x9d
    (const int)default_handler, // 0x9e
    (const int)default_handler, // 0x9f
    (const int)default_handler, // 0xa0
    (const int)default_handler, // 0xa1
    (const int)default_handler, // 0xa2
    (const int)default_handler, // 0xa3
    (const int)default_handler, // 0xa4
    (const int)default_handler, // 0xa5
    (const int)default_handler, // 0xa6
    (const int)default_handler, // 0xa7
    (const int)default_handler, // 0xa8
    (const int)default_handler, // 0xa9
    (const int)default_handler, // 0xaa
    (const int)default_handler, // 0xab
    (const int)default_handler, // 0xac
    (const int)default_handler, // 0xad
    (const int)default_handler, // 0xae
    (const int)default_handler, // 0xaf
    (const int)default_handler, // 0xb0
    (const int)default_handler, // 0xb1
    (const int)default_handler, // 0xb2
    (const int)default_handler, // 0xb3
    (const int)default_handler, // 0xb4
    (const int)default_handler, // 0xb5
    (const int)default_handler, // 0xb6
    (const int)default_handler, // 0xb7
    (const int)default_handler, // 0xb8
    (const int)default_handler, // 0xb9
    (const int)default_handler, // 0xba
    (const int)default_handler, // 0xbb
    (const int)default_handler, // 0xbc
    (const int)default_handler, // 0xbd
    (const int)default_handler, // 0xbe
    (const int)default_handler, // 0xbf
    (const int)default_handler, // 0xc0
    (const int)default_handler, // 0xc1
    (const int)default_handler, // 0xc2
    (const int)default_handler, // 0xc3
    (const int)default_handler, // 0xc4
    (const int)default_handler, // 0xc5
    (const int)default_handler, // 0xc6
    (const int)default_handler, // 0xc7
    (const int)default_handler, // 0xc8
    (const int)default_handler, // 0xc9
    (const int)default_handler, // 0xca
    (const int)default_handler, // 0xcb
    (const int)default_handler, // 0xcc
    (const int)default_handler, // 0xcd
    (const int)default_handler, // 0xce
    (const int)default_handler, // 0xcf
    (const int)default_handler, // 0xd0
    (const int)default_handler, // 0xd1
    (const int)default_handler, // 0xd2
    (const int)default_handler, // 0xd3
    (const int)default_handler, // 0xd4
    (const int)default_handler, // 0xd5
    (const int)default_handler, // 0xd6
    (const int)default_handler, // 0xd7
    (const int)default_handler, // 0xd8
    (const int)default_handler, // 0xd9
    (const int)default_handler, // 0xda
    (const int)default_handler, // 0xdb
    (const int)default_handler, // 0xdc
    (const int)default_handler, // 0xdd
    (const int)default_handler, // 0xde
    (const int)default_handler, // 0xdf
    (const int)default_handler, // 0xe0
    (const int)default_handler, // 0xe1
    (const int)default_handler, // 0xe2
    (const int)default_handler, // 0xe3
    (const int)default_handler, // 0xe4
    (const int)default_handler, // 0xe5
    (const int)default_handler, // 0xe6
    (const int)default_handler, // 0xe7
    (const int)default_handler, // 0xe8
    (const int)default_handler, // 0xe9
    (const int)default_handler, // 0xea
    (const int)default_handler, // 0xeb
    (const int)default_handler, // 0xec
    (const int)default_handler, // 0xed
    (const int)default_handler, // 0xee
    (const int)default_handler, // 0xef
    (const int)default_handler, // 0xf0
    (const int)default_handler, // 0xf1
    (const int)default_handler, // 0xf2
    (const int)default_handler, // 0xf3
    (const int)default_handler, // 0xf4
    (const int)default_handler, // 0xf5
    (const int)default_handler, // 0xf6
    (const int)default_handler, // 0xf7
    (const int)default_handler, // 0xf8
    (const int)default_handler, // 0xf9
    (const int)default_handler, // 0xfa
    (const int)default_handler, // 0xfb
    (const int)default_handler, // 0xfc
    (const int)default_handler, // 0xfd
    (const int)default_handler, // 0xfe
    (const int)default_handler, // 0xff
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xfffffffe
};
