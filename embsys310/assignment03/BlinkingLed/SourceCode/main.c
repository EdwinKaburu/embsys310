#if 1

#define RCC_BASE 0x40021000

#define GPIOA_BASE 0x48000000

#define RCC_AHB2ENR (*((unsigned int*) (RCC_BASE + 0x4C) ))

#define GPIOA_MODER (*((unsigned int*) (GPIOA_BASE + 0x0) ))

#define GPIOA_ODR (*((unsigned int*) (GPIOA_BASE + 0x14) ))

#define ORD5 (1 << 5) //shift 1 to the left n times



volatile int counter = 0;

int main()
{
    
    // GPIOA = 0x1 for clock enable register Rcc_AHB2ENR; Enable clock to GPIOA (Bit 0)
    RCC_AHB2ENR |= 0x1; 
    
    //GPIOA Base Address : 0x48000000; GpioA_MODER: SET GPIOA Mode to Output
    
    GPIOA_MODER &=  0xFFFFF7FF; // Will be 0xABFFF7FF
    
    
    //volatile unsigned int ord5 = 0;
    
    while(1)
    {
        counter = 0;
        
        while(counter < 50000)
        {
            counter++;
        }
        
        //Turn on Led 1 : GpioA_ODR [0x48000014] (PA5 ; 0x0020) working with GpioA
        
        GPIOA_ODR ^= ORD5;
        
  
        counter = 0;
        
        while(counter < 50000)
        {
            counter++;
        }
        
        //Turn off or Reset led 1 (PA5; 0x0000) GpioA_ODR [0x48000014] 
                
        GPIOA_ODR ^= ORD5;
        
        
    }
    
}

#endif
