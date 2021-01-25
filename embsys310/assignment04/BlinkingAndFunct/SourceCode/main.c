// LD1 --> PA5 -- Homework
// LD2 --> PB14 -- In Class

#if 1

#define RCC_BASE 0x40021000
#define RCC_AHB2ENR (*((unsigned int*)(RCC_BASE + 0x4C)))

//#define GPIOB_BASE 0x48000400
//#define GPIOB_MODER (*((unsigned int*)(GPIOB_BASE + 0x00)))
//#define GPIOB_ODR (*((unsigned int*)(GPIOB_BASE + 0x14)))



//GPIO A

#define GPIOA_BASE 0x48000000

#define GPIOA_MODER (*((unsigned int*) (GPIOA_BASE +  0x0) ))

//Write to GPIOA_ODR: offset is 0x14
#define GPIOA_ODR (*((unsigned int*) (GPIOA_BASE +  0x14) ))

#define ORD5 (1 << 5) 

volatile int counter = 0;

void delay(void);
int funct1(int val0, int val1, int val2, int val3, int val4);
void funct2(void);


int funct1(int val0, int val1, int val2, int val3, int val4)
{
    int lvar0;
    int lvar1;
    int lvar2;
    int lvar3;
    int lvar4;
    
    int sum;
    
    lvar0 = val0;
    lvar1 = val1;
    lvar2 = val2;
    lvar3 = val3;
    lvar4 = val4;
    
    sum = lvar0 + lvar1 + lvar2 + lvar3 + lvar4;
    
    return sum;
    //return (val0 + val1 +val2 + val3+ val4);
}

void funct2(void)
{
    funct1(1,2,3,4,5);
}

void delay(void)
{
    counter=0;
    while (counter < 100000)
    {
        counter++;
    }
}

void main(void)
{
    funct2();
    
    // RCC Base Address: 0x40021000
    // RCC AHB2 peripheral clock enable register (RCC_AHB2ENR)
    // Address offset: 0x4C
    // Set bit[1] to 1
    // 1. Enable clock to Peripheral
    //RCC_AHB2ENR |= 0X1;
    
    
    //Bit Banding Peripherals range is from ( 0x4000 0000  to 0x400F FFFF)
    // GPIO registers are out of bounds, but the RCC are within the range.
    // Enable Bit 0 for GPIO A (PA15) , RCC_AHB2ENR
    
    //bit band alias region + (offest * 32)+ (the the bit * 4) set it equal to 1
    
    // Result will be 0x4242 0890
    *((unsigned int*) ((0x42000000) + (0x2104c * 32)+ (0 * 4))) = 0x1; 
    
    
    
    // GPIOA Base Address: 0x4800 0000.
    // SET Mode to General Purpose Output Mode (01) to [11:10]
    // Bit 11 will be set to 0
    // Bit 10 will be set to 1
    GPIOA_MODER &= 0xFFFFF7FF;
    
    
    while(1)
    {
        delay();
        
        GPIOA_ODR ^= ORD5;
        
        delay();
        
        GPIOA_ODR ^= ORD5;
        
    }
}
#endif
