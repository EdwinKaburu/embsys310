#include "delay.h"
#include "stm32l475xx.h"

#define SYS_CLOCK_HZ_MS 4000u; 


void delay(uint32_t delay_ms)
{
   
    int elapse_ms =  delay_ms * SYS_CLOCK_HZ_MS; 
    
    // 0xE000E014 - Counts down to 0.
    SysTick->LOAD = elapse_ms - 1;
    
    // 0xE000E018 - Clears initial value
    SysTick->VAL = 0x0;      
    
    // 0xE000E010 - Enable interrupts
    SysTick->CTRL = 0x7;                    
    
    while(SysTick->VAL)
    {
        
    }
    
}
