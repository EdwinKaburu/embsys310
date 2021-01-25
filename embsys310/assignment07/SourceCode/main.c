#include "stm32l475xx.h"
#include "stm32l4xx_it.h"
#include "stdint.h"

#include "delay.h"

// Half A Second : 500ms
uint32_t delay_val = 500u;

/*

// One Millisecond : 1ms
uint32_t delay_val = 1u;

// Two Second : 2 sec (2000ms)
uint32_t delay_val = 2000u;

// One Second : 1 sec (1000ms)
uint32_t delay_val = 1000u;

*/
int main()
{
    /*
         Enable RCC: Enable clock to Peripheral 
         RCC AHB2 peripheral clock enable register (RCC_AHB2ENR)
         RCC Base Address: 0x40021000
         Address offset: 0x4C
         Set bit[1] to 1
    */
    
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    
    // GPIOA Port MODER Register
    GPIOA->MODER &= ~GPIO_MODER_MODE5_1;
    
    GPIOA->MODER |= GPIO_MODER_MODE5_0;
    
    
    while(1)
    {
        // Shift left 5 times : Toggle LED
        GPIOA->ODR ^= GPIO_ODR_OD5;
        delay(delay_val);
    }
    
}

void SysTick_Handler(void)
{
    __disable_irq();
    
    // Do Something in Interrupt Handler
    int temp_delay = delay_val;
    
    while(temp_delay)
    {
        temp_delay--;
    }
    
    __enable_irq();
}
