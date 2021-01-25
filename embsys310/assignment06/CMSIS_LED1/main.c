#if 1

#include "stm32l475xx.h"

//#define RCC_BASE 0x40021000
//#define RCC_AHB2ENR (*((unsigned int*)(RCC_BASE + 0x4C)))
//
//#define GPIOB_BASE 0x48000400
//#define GPIOB_MODER (*((unsigned int*)(GPIOB_BASE + 0x00)))
//#define GPIOB_ODR (*((unsigned int*)(GPIOB_BASE + 0x14)))

//#define ORD5 (1 << 5) 

//#define ORD14   (1<<14)

volatile int counter = 0;

void delay(void);

void delay(void)
{
    counter=0;
    while (counter < 100000)
    {
        counter++;
    }
    
    // Shift left 5 times
    
    GPIOA->ODR ^= GPIO_ODR_OD5;
}

int main()
{
    // RCC Base Address: 0x40021000
    // RCC AHB2 peripheral clock enable register (RCC_AHB2ENR)
    // Address offset: 0x4C
    // Set bit[1] to 1
    // 1. Enable clock to Peripheral
    
    // Enable RCC 
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    
    // GPIO MODER 
    GPIOA->MODER &= ~GPIO_MODER_MODE5_1;
    GPIOA->MODER |= GPIO_MODER_MODE5_0;
    
    
    while(1)
    {
        delay();
        
        
    }
    
}

#endif