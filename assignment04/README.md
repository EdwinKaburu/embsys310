## Assignment 04
### 1. Follow the same steps as we did in the class to blink LED1 (PA5) at a visible rate using the bit band region to enable the clock to Port A, then answer the following: 
 
 a. What instructions does the compiler produce in assembly for “writing” to the RCC_AHB2ENR bit when using bit-band address?
 - The instructions that were produced by the compiler using the bit
   banding address for the RCC_AHB2NE, included. So in the image below
   we first move #1 into R0, we then load the value stored in [PC,
   #0x4c] memory location into R1, which will be the bit band peripheral alias (0x42420980). We then store/write R0 value into R1 (R1 provides
   the memory location). The instructions are fewer with bit banding. 
   
   ![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/BlinkingAndFunct/Img/Img3.PNG)
   
b.	What were the instructions produced when writing to the RCC_AHB2ENR without using bit banding?  
   - So we had to load the value, in our case, stored in Register 0 into
   Register 1.  After which we OR (bitwise) register 1 with #1 (the bit
   we want to modify or writing to [GPIOA]) and store it back into
   register 1. To enable the the enable the peripheral clock to GPIOA we
   write/store it back R0 (R0 provides the memory location) . 
   
   ![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/BlinkingAndFunct/Img/Img1.PNG)

### 2. Create a function “func1” with 5  arguments and call “func1” from within another function “func2”. Trace thru the assembler and note: 

a.	How does the calling function “func2” pass the values to the called function “func1”? 

 - Well funct1(), preforms and return the addition of the 5 arguments, the main purpose of func2() is to call func1() and initializes the required fields into the parameter.
As such the compiler generate the BL instruction, ASM to, branch/jump to funct2() at the address 0x8000064, which will be loaded into the Program Counter as the next instruction to perform.

![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/BlinkingAndFunct/Img/Img4.PNG)

[ Click Here To See Image](https://github.com/EdwinKaburu/embsys310/tree/main/assignment04/BlinkingAndFunct/Img/Img4.PNG)

b.	What extra code did the compiler generate before calling the function “func1” with the multiple arguments? 

 - Before calling the funct1(), there are few instructions generate. First of all  the registers (R7 (0x00000000) and LR (0x08000097)) are pushed into the stack (SP).
The stack pointer will be pointing to the last item to inserted; at address (0x20001FF8) with the data  (0x00000000). 

![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/BlinkingAndFunct/Img/Img5.PNG)

[ Click Here To See Image](https://github.com/EdwinKaburu/embsys310/tree/main/assignment04/BlinkingAndFunct/Img/Img5.PNG)

With the calling of the arguments to be initialized into func1, are multiple MOVS instructions into registers, before Branching to funct1 address (BL).
As such the instruction generate are as followed for the 5 values: 
First #5 is moved into R0, followed by storing RO value into the add that SP (stack pointer) is pointing at location/address(0x20001FF8) , so its data will be overwritten to (0x00000005).

![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/BlinkingAndFunct/Img/Img6.PNG)

[ Click Here To See Image](https://github.com/EdwinKaburu/embsys310/tree/main/assignment04/BlinkingAndFunct/Img/Img6.PNG)

c.	What extra code did the compiler generate inside the called function “funct1” with the multiple list of arguments? 

 - Since we have a Thumb instruction on the machine, the LR (Link Register), will hold the address (0x08000077), to return to when the function call, funct1() ends/complete.
 
 ![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/BlinkingAndFunct/Img/Img8.PNG)
 
 [ Click Here To See Image](https://github.com/EdwinKaburu/embsys310/tree/main/assignment04/BlinkingAndFunct/Img/Img8.PNG)
 
In the generate instruction we see that at address 0x08000076 will be a POP instruction; to pop values to (R0 and PC). 

![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/BlinkingAndFunct/Img/Img7.PNG)

[ Click Here To See Image](https://github.com/EdwinKaburu/embsys310/tree/main/assignment04/BlinkingAndFunct/Img/Img7.PNG)

Also we see that the registers R0 - R3 have the values we need to perform the addition, five is still stored into the SP (since we are adding (1+2+3+4+5))
- So in Funct1(), we start with pushing the registers (R4-R8, LR) into the stack pointer. 

![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/BlinkingAndFunct/Img/Img10.PNG)

[ Click Here To See Image](https://github.com/EdwinKaburu/embsys310/tree/main/assignment04/BlinkingAndFunct/Img/Img10.PNG)

- Next we have MOVS instruction which will move the data value in R0 to R4 (R0 will still have a value of 1), after that we load, LDR, into R5 the value stored at [SP,#0x18] which will be 5. 

![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/BlinkingAndFunct/Img/Img11.PNG)

[ Click Here To See Image](https://github.com/EdwinKaburu/embsys310/tree/main/assignment04/BlinkingAndFunct/Img/Img11.PNG)

- To perform the sum, the instructions ADDS and ADDS.W are used, all of them are writing back into the R0 . 
After the addition instructions are performed R0 will have the total sum which will be 15 or 0x0000000f. 

![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/BlinkingAndFunct/Img/Img14.PNG)

[ Click Here To See Image](https://github.com/EdwinKaburu/embsys310/tree/main/assignment04/BlinkingAndFunct/Img/Img14.PNG)

d.	Any other observations? 

 - After that there is a POP.W to {R4-R8, PC}; 
Before the initializations and addition calculation, we stored the values in the registers {R4-R8,LR}, the PC will now contain the address to execute (LR).

![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/BlinkingAndFunct/Img/Img15.PNG)

[ Click Here To See Image](https://github.com/EdwinKaburu/embsys310/tree/main/assignment04/BlinkingAndFunct/Img/Img15.PNG)

- The next instruction to be executed will be a POP instruction to update the R0 (15 will be overwritten with what was in the SP) and PC (instruction to execute next). 

![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/BlinkingAndFunct/Img/Img17.PNG)

[ Click Here To See Image](https://github.com/EdwinKaburu/embsys310/tree/main/assignment04/BlinkingAndFunct/Img/Img17.PNG)

This is a very wordy, there is a video on this, for reference at (vimeo - https://vimeo.com/479594367) or the gif below

- ![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/BlinkingAndFunct/Img/functOverview.gif)


## Problem 3 : Stack ADT
- Stack LIFO, the ADT is as follows

|adt | functionality |
|--|--|
| void stack_init(void)|  Initialize Values|
|int stack_push(int data);| push or insert data|
|int stack_pop(int *data)| remove/pop value 
|int isempty(void)| Check if stack is empty|
|int isfull(void)|Check if stack is full|

See files (source code) for implementation, Test and Comments 

![alt text](https://github.com/EdwinKaburu/embsys310/blob/main/assignment04/Stack_DST/Img/capture.PNG)

