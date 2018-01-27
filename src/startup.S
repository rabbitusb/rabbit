

    .section .isr_vector
    .long    0x20000200 /*__stack_end__*/         /* Top of Stack */
    .long    Reset_Handler         /* Reset Handler */
    .globl    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
    bl    main

    .end
