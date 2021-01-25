#if 1

#define RCC_BASE 0x40021000

#define GPIOA_BASE 0x48000000

#define RCC_AHB2ENR (*((unsigned int*) (RCC_BASE + 0x4C) ))

#define GPIOA_MODER (*((unsigned int*) (GPIOA_BASE + 0x0) ))

#define GPIOA_ODR (*((unsigned int*) (GPIOA_BASE + 0x14) ))

#define ORD5 (1 << 5) //shift 1 to the left n times


// One Unit of Time
#define DOT_TIME ((unsigned int ) 100000)

// Three Units of Time
#define DASH_TIME ( (unsigned int ) (DOT_TIME * 3) )

// Seven Units of Time
#define WORD_SPACE ( (unsigned int) (DASH_TIME * 2) )


// More Code for " Edwin "; there is a space after each letter
const char lc_ptr[] = ". -.. .-- .. -.";

// Size of ModeCode
const unsigned int ed_morse_size = ( sizeof(lc_ptr)/sizeof(char) );

volatile int counter = 0;

// Toggle between On or Off State using  XOR (bitwise) 
void ledToggle(unsigned int timer)
{
    counter = 0;
    
    //Toggle Led : On or Off State
    GPIOA_ODR ^= ORD5;
    
    // Unit of TIme
    while (counter < timer)
    {
        counter++;
    }
}


// Character Unit Time function: c_Unit(const char element)
// Set the Time delays for led whether On or Off, based on character input
void c_Unit(const char* elem)
{
    switch (*elem) {
    case '.':
        
        //Turn On Led 1 : [1] One Unit of Time  
        
        ledToggle(DOT_TIME);
        
        break;
    default:
        // Dashes ( "-" ) and Spaces,between letters, ( " " ) are 3 Units respectively. 
        
        ledToggle(DASH_TIME);
        
        break;
    }
}

int main()
{
    
    // GPIOA = 0x1 for clock enable register Rcc_AHB2ENR;
    // Enable clock to GPIOA (Bit 0)
    RCC_AHB2ENR |= 0x1; 
    
    // GPIOA Base Address : 0x48000000; GpioA_MODER: SET GPIOA Mode to Output
    // Will be 0xABFFF7FF
    GPIOA_MODER &=  0xFFFFF7FF; 
    
    
    while(1)
    {
        for (int a = 0; a < ed_morse_size - 1; a++)
        {
            // Call and assign;The Character Unit Time function: (c_Unit(const char elem))
            c_Unit(&lc_ptr[a]);
            
            //Check where this char represent a letter
            //(There is a space/pause for one [1] unit; to the parts representing a letter)
            if(lc_ptr[a+1] != ' ' && lc_ptr[a] != ' ' )
            {   
                //Turn off Led 1 : On Letter Condition : Represent 0ne Unit (DOT TIME)
                 ledToggle(DOT_TIME);
                 
            }
            
        }
        
        counter = 0;
        
        // LED 1 is off: End of Word (Letter is Finished: 
        // There is a Pause/Space for 7 units equivalent to 7 dots)
        while (counter < WORD_SPACE)
        {
            counter++;
        }
    }  
    
    // (unreachable) return 0;
    
}

#endif

