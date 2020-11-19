
# Assignment 5

## 1. Create a function in “C” that allows swapping of two pointers.
A. Explain what the “main” function does in order to setup the input arguments prior to calling the swap_pointer() function?
--

 - So prior to calling the swap_pointer() function, I first intialized the [int] variables data needed, and then I have (*) pointers pointing to each [int]variables memory location respectively. Then I pass the 2 pointers as arguments in order for the swapping to occur.
 - As in the image below, the program starts with pushing the registers (R3-R7, LR) into the stack.
 - Next the initialization of the variables (val and val1) occur, the data value is respectively, first loaded (**LDR.N**) and then stored (**STR) into the stack.
 - As such for the first initialization (int val = 100000) the **LDR.N** gets the value (100000),stored at memory/address [PC, #0x1c (offest)] into R0, next the value in RO is stored into the stack at [SP,0xc], because the intialization of the next value (int val1 = 200000) would require R0, so/thus it must be stored into the stack for later use.
 - For the pointers we have an **ADD** instruction followed by a **STR**  instruction for each of the pointers
 - The first pointer (xPtr) for example, stores or put into R0 the address of [int val] that is currently in the SP ( at the address  [0x20001ff4] ), through a SUM of the [SP + 0xC(offset)]; as pointing to the memory location of (int val) is the initialization of xPtr pointer.
 - After this the value in R0 ( which is the memory location of val) is stored back in the **SP** , at address [SP,0x4],  because the next  pointer will be using R0 to point to memory/address of (int val1) (that is also stored in the Stack).
 - Thus when xPtr is stored in **SP** (its data value will be the **SP memory/address location of (int val), thus it will be something like this (0x20001FEC , data = 0x20001ff4))**, see below or Image
 
 |Location of Pointer(xPtr) in SP| Data: SP Address/Memory Location of (Val)  |
|--|--|
| 0x2000 1FEC  |0x2000 1FF4  |
---
| Location of Val in SP | Data: Actual int Value|
|--|--|
| 0x2000 1FF4 | 0x0001 86A0  | 


Passing the Values to the swap_ptr() function

 - Prior to Branching into, BL, there is a **MOV** and ADD instruction, initialize the registers R0 and R1 with the required data/values
   needed.
 - As such the MOV will store the **SP memory/address** location of yPtr (0x20001FE8) into R1; R0 will contain the SP memory/address location of xPtr (0x20001FEC), before branching out.
 
 ![BeforeImg](https://github.com/EdwinKaburu/embsys310/blob/main/assignment05/Img/beforeBL.PNG)
 

B. What are the values in R0 & R1 when swap_pointer() is called?
-
Before

 - The value currently stored in R0, is the address in SP of the xPtr
   pointer (0x20001FEC, its data value is the SP address/memory location
   of [ [int val] also in the stack ]). The value stored into R1 is the
   address in SP for the yPtr pointer (0x20001FE8 its data value is the
   SP address/memory location of [ [int val1] also in the stack ]).
   
   ![combBeforeAfter](https://github.com/EdwinKaburu/embsys310/blob/main/assignment05/Img/combBeforeAfer.png)

After

 - After, swapping the SP address/memory of where the xPtr and yPtr are pointing to; **[ xPtr ] will be point to the address/memory [yPtr ]is pointing to prior to performing the swap**; so (xPtr -> val1 and yPtr -> val). We get the result below. See Source Code (main.c)
   
   ![combBeforeAfter](https://github.com/EdwinKaburu/embsys310/blob/main/assignment05/Img/finalPointer.PNG)

