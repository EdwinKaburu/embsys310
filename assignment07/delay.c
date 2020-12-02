#include "delay.h"


#define SYS_CLOCK_HZ 4000000u

void delay(uint32_t delayInMilliseconds)
{
    uint32_t time_delay = (SYS_CLOCK_HZ * delayInMilliseconds) / 1000u;
    
    SysTick->LOAD = time_delay - 1;        // 0xE000E014 - Counts down to 0.
    SysTick->VAL = 0x0;                     // 0xE000E018 - Clears initial value
    SysTick->CTRL = 0x7;                    // 0xE000E010 - Enable interrupts
}

//void delay(unsigned int iteration)
//{
//    while (iteration > 0)
//    {
//        iteration--;    
//    }
//}