#if 1

#include "stdint.h"

#define RCC_BASE 0x40021000
#define RCC_AHB2ENR (*((unsigned int*)(RCC_BASE + 0x4C)))

#define GPIOA_BASE 0x48000000

#define GPIOA_MODER (*((unsigned int*) (GPIOA_BASE +  0x0) ))

//#define GPIOA_ODR (*((unsigned int*) (GPIOA_BASE +  0x14) ))

//#define ORD5 (1 << 5) 


void delay(uint32_t duration);

void control_user_led1(uint8_t state, uint32_t duration);

int main()
{
    uint8_t on_state = 1u;
    
    uint8_t off_state = 0u;
    
    uint32_t duration = 100000u;
    
    
    RCC_AHB2ENR |= 0X1;
    
    GPIOA_MODER &= 0xFFFFF7FF;
    
    
    while(1)
    {
        // Off State
        control_user_led1(off_state,  duration);
        
        // On State
        control_user_led1(on_state,  duration);
        
    }
    
}

#endif













