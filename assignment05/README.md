
# Assignment 5

## 1. Create a function in “C” that allows swapping of two pointers.
A. Explain what the “main” function does in order to setup the input arguments prior to calling the swap_pointer() function?
--
- So for showing the swapping of the 2 pointers, I first initialized two variables (int val = 100000 and int val1 = 200000); Both will have an accommodating pointer pointing/storing their addresses rather than their value. As such in code you will see, (int *xPtr = &val and int*yPtr = &val1). The pointers themselves are what we are swapping, as such we are going pass them are argument to the swap_ptr(&xPtr, &yPtr) routine /function/method().
See main.c in the [**TwoPointerSwap folder**](https://github.com/EdwinKaburu/embsys310/tree/main/assignment05/TwoPointerSwap) for implementation.
 - Now Before calling or branching into the swap_pointer() function, there are a few (LDR.N) instructions and (STR) Store to set up R0 and R1 parameters with the address/memory location in the Stack(SP) for the 2 pointers we are swapping.
	 - As such after the PUSH {R3-R7,LR}, the initialization of the two variables (val and val1), occurred with LDR.N instruction to load, the respectively value (100000 or 200000) into R0, that will be later be STR (stored) into the Stack, as R0 will be required in other operations.
	 - The initialization of the pointers is similar to the initial variables (val, val1); with a minor difference in R0 will contain the result of the Addition of what the currently SP address/memory is currently is or being pointed to, with a certain value ( RO, SP, #0xc). (We are initializing xPtr, so the value/result in R0 before the STR will contain the memory/addresses location in the Stack of val; since val is already initialized and stored in the Stack).
	- After ADD instruction, we have to STR it, because as stated before R0 is going to needed/use in follow up operations to set up the parameters/argument to that are going to to passed.

|Location| Data | Variable| |
|--|--|--|--|
| 0x20001FE8 | 0x20001FF0  | yPtr|
| 0x20001FEC | 0x20001FF4  | xPtr |
| 0x20001FF0 | 0x00030D40  | val1 | 200000
| 0x20001FF4 | 0x000186A0  | val | 100000

![BeforeImg](https://github.com/EdwinKaburu/embsys310/blob/main/assignment05/Img/beforeBL.PNG)

## B. 	What are the values in R0 & R1 when swap_pointer() is called?
- After initializing the values (see image above), there a few instructions left (MOV, ADD) that are going to put into R0 and R1 the Stack memory/addresses location
of the two pointers that we can now use in the swap_ptr() function. [R0 - first input parameter (0x20001FEC) , R1 - second input parameter (0x20001FE8)].
see above Table and Image

- See Images below for a before and after, pay attention the Data Field (Stack), we see after swapping both will be storing the other in that Data field.
This means we have successful swap the pointers in terms of the address/memory location they are poiting to. See for code for implementation.

 ![combBeforeAfter](https://github.com/EdwinKaburu/embsys310/blob/main/assignment05/Img/combBeforeAfer.png)

![combBeforeAfter](https://github.com/EdwinKaburu/embsys310/blob/main/assignment05/Img/finalPointer.PNG)

   

## 2. Create a new file divAsm.s and add the file to the same HelloWorld project above 
For question you can see in more details and comments at  [C_Assembler/divAsm.s](https://github.com/EdwinKaburu/embsys310/tree/main/assignment05/C_Assembler/divAsm.s)
Overall, it can be summed up into 2 instructions, where we are first moving/loading **[MOV] into R1 (register) 2**  that we need in order to divided. As such to get , a  division; [SDIV], we divide R0 (contains our input) by R1 and write/store  it back to register R0.  ( SDIV R0, R0, R1 ), then we return to address LR (will be loaded into PC), so BX LR.

## 3. Implement a swap function in assembly and call it “swapCharsAsm”:   	 4. Bonus: Implement the swap_pointer() function from #1 above in assembly and call it swapPointersAsm()

The assembly code for question 3 and 4 are very similar, but differ in what is being  in the loaded and stored. The comment in code make the distinction very clear.
Both when it comes to setting the input/parameters, The R0 and R1 registers contains/points to  the SP Memory/Address  of the inputs/parameters, thus when storing and loading [LDR or LDRB]  the value stored in the SP, it is becomes much more easier.

- For instance, (Swapping the 2 pointers), in the registers it will contain the address of the 2 pointers in SP

|Name|Value  |  |
|--|--|--|
| R0 |0x20001FF0  | xPtr |
| R1 | 0x20001FEC | yPtr |

- In SP (Stack), it will have a Pointer (xPtr, yPtr) Address in SP; and the SP address (data) it is pointing to, see [Question 1 Above](https://github.com/EdwinKaburu/embsys310/tree/main/assignment05#a-explain-what-the-main-function-does-in-order-to-setup-the-input-arguments-prior-to-calling-the-swap_pointer-function)

|Location| Data  |
|--|--|
| 0x20001FEC | 0x20001FF4 |

 **- For Question 2 - 4 , you can find them in C_Assembler folder** 
 
  ![AsmPicture](https://github.com/EdwinKaburu/embsys310/blob/main/assignment05/Img/AsmPicture.PNG)
  
  
  

