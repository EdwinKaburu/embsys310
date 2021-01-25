/*******************************************************************************
File name       : user_led.s
Description     : Assembly language function for controlling the user LED
*******************************************************************************/   

    EXTERN delay                    // delay() is defined outside this file

    PUBLIC control_user_led1        // Exports symbols to other modules

// Code is split into logical sections using the SECTION directive.
// Source: http://ftp.iar.se/WWWfiles/arm/webic/doc/EWARM_AssemblerReference.ENU.pdf
// SECTION  section  :type [:flag] [(align)]
//      The data section is used for declaring initialized data or constants. This data does not change at runtime
//      The bss section is used for declaring variables. The syntax for declaring bss section is -
//      The text section is used for keeping the actual code.

// CODE: Interprets subsequent instructions as Arm or Thumb instructions, 
// depending on the setting of related assembler options.

// NOREORDER (the default mode) starts a new fragment in the section
// with the given name, or a new section if no such section exists.
// REORDER starts a new section with the given name.

// NOROOT means that the section fragment is discarded by the linker if
// no symbols in this section fragment are referred to. Normally, all
// section fragments except startup code and interrupt vectors should
// set this flag.

// The (2) is for the (align)
// The power of two to which the address should be aligned.
// The permitted range is 0 to 8. 
// Code aligned at 4 Bytes.

    SECTION .text:CODE:REORDER:NOROOT(2)
    
    THUMB               // Indicates THUMB code is used
                        // Subsequent instructions are assembled as THUMB instructions
    
/*******************************************************************************
Function Name   : control_user_led1
Description     : - Uses Peripheral registers at base 0x4800.0000
                    to set GPIOA Output Data Register.
                  - Calls another assembly function delay to insert
                    delay loop

C Prototype     : void control_user_led(uint8_t state, uint32_t duration)
                : Where state indicates if LED should be on or off.
Parameters      : R0: uint8_t state
                : R1: uint32_t duration
Return value    : None
*******************************************************************************/  

GPIOA_BASE  EQU 0x48000000
GPIOA_ODR   EQU 0x14
GPIOA_BIT_5 EQU 0x20

control_user_led1
    PUSH {R3-R5,LR}     // PUSH R-R5 and LR to Stack
    MOVS R4, R0         // MOVE the R0 into R4
    MOVS R5, R1         // MOVE the R1 into R5
    CMP R0, #1          // COMP R0 with #1 
    BNE offcheck        // If R0 is equal to 0, turn Off Led (Not Equal to 1 go to offcheck)
    BEQ turnon          // If R0 is eqaul to 1, turn On Led (Equal to 1,  go to turnon)
turnon
    LDR R0, =GPIOA_BASE // Load GPIOA_BASE into R0
    LDR R1, =GPIOA_ODR  // Load GPIOA_ODR Offset into R0
    ADD R0, R0, R1      // Add GPIOA_BASE + GPIOA_ODR (offset) to generate the GPIOA_ODR adddress
    LDR R1, [R0]        // Load into R1 current value/data, pointed/from the GPIOA_ODR adddress (0x4800 0014)
    B storebit          // R1 value is (0x00000000 ), In order to Turn on
offcheck
   LDR R0, =GPIOA_BASE  // Load GPIOA_BASE into R0
   LDR R1, =GPIOA_ODR   // Load GPIOA_ODR Offset into R0
   ADD R0, R0, R1       // Add GPIOA_BASE + GPIOA_ODR (offset) to generate the GPIOA_ODR adddress
   LDR R1, [R0]         // Load into R1 current value/data, pointed/from the GPIOA_ODR adddress (0x4800 0014)
   CMP R1, #0           // Comparison
   BEQ delayfun         // R1 value is (0x00000000 ), no need to reset, go to delay
   BNE storebit         // R1 value is (0x00000020 ), In order to reset PA5, Turn off
storebit
   LDR R2, =GPIOA_BIT_5
   EORS.W R1, R1, R2  // XOR R1 with GPIOA_BIT_5; In order to reset or turn depending on current state
   STR R1, [R0]       // Store R1 back into R0(0x4800 0014)'s data/value
   B delayfun         // Go to delayfun
delayfun
    MOVS R0, R5
    BL delay          // Go to Delay Function (parameter set with the #items to count)
    POP {R0,R4,R5,PC}
    //BX LR
    END
