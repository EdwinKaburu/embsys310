#if 1
//stm32l4xx_it.h - defines the standard prototypes for the  Vector Table
#include "stm32l4xx_it.h"

#pragma section="CSTACK" 

void __iar_program_start(void);

void Unused_Handler(void);

void Unused_Handler(void)
{
    while(1);
}

int const __vector_table[] @ ".intvec" = {
    (int)__section_end("CSTACK"), // Pointer To Top of Stack
    (int)__iar_program_start,      // Pointer to Reset Handler
    (int)NMI_Handler,
    (int)HardFault_Handler,
    (int)MemManage_Handler,
    (int)BusFault_Handler,
    (int)UsageFault_Handler,
    0,      // Reserved
    0,      // Reserved
    0,      // Reserved
    0,      // Reserved
    (int)SVC_Handler,
    (int)DebugMon_Handler,
    0,      // Reserved
    (int)PendSV_Handler,
    (int)SysTick_Handler
        
};

#pragma weak NMI_Handler = Unused_Handler
#pragma weak HardFault_Handler = Unused_Handler
#pragma weak MemManage_Handler = Unused_Handler
#pragma weak BusFault_Handler = Unused_Handler
#pragma weak UsageFault_Handler = Unused_Handler

#pragma weak SVC_Handler = Unused_Handler
#pragma weak DebugMon_Handler = Unused_Handler
#pragma weak PendSV_Handler = Unused_Handler
#pragma weak SysTick_Handler = Unused_Handler

#endif