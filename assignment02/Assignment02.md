## IAR

### 1. Inject 0x7FFFFFFF for the “counter” value in the variable window, then step thru the program only once to increment “counter”.
 - What is the value of the “counter” from the “Locals” window?
	 - While (0x7FFFFFFF) is in decimal (2147483647), after one increment of the counter, its values change to least negative number ( -2147483648)
 - What is the value of the “counter” in the “Registers” window?
	 - In the Register (R1), when the counter variable is changed to (0x7FFFFFFF), it is reflected in the R1 registry, when we increment the counter once; The hex value in R1 this time changes after incrementation, to 0x80000000 (decimal format: 2147483648 )   
 - Please note down if the N and/or V flags are set in the APSR register. And explain why.
	 - Then N and V values in the APSR register, both get flagged, with the N (negative becoming 1) and the V (overflow becoming 1), because it flipped from highest positive to least negative.

### 2.If your write all Fs (0XFFFFFFFF) in the Register value for “counter” then step thru the program once to increment “counter”
- What happens to the value of “counter” in the “Locals” window?
	- After one incrementation the counter value in the Local window changes to 0 
- Please note down if the N and/or V flags are set in the APSR register. And explain why.
	- In the APSR Register the N condition is 0 bit and the V is 0 bit (there is no overflow), as we flip to positive, there is however a Zero and Carry condition flag. As with the first question we are within the same range of (-2,147,483,648 to 2,147,483,647), thus the reason there was no overflow and negative bit after incrementation.

### 3. Change the “counter” variable type in your code to “unsigned int”. Inject the values “0x7FFFFFFF” then step thru the program to increment the “counter” once:
- What is the value of “counter” in the “Locals” window after incrementing for each value?
	-	After one Increment, the value of counter in the Locals windows is incremented by one to (0x80000000 or 2147483648) and continues to grow sequentially with each incrementation.
- Please note down if the N and/or V flags are set in the APSR register. And explain why.
	- The instruction result of V and N conditional are flag with there being a negative bit and overflow bit, after incrementing 0x7FFFFFFF once. The value of the counter does not become a negative number (remains positive due to the unsigned) on the remain subsequent increments, but will remain a negative condition, due to the reserve of the most significant bit. 
### 4. Change the “counter” variable type in your code to “unsigned”. Inject the values “0xFFFFFFFF” then step thru the program to increment the “counter” once:
- What is the value of “counter” in the “Locals” window after incrementing for each value?
	- With unsigned counter; In the local’s windows, after incrementing (0xFFFFFFFF) once, the variable resets back to 0. Incrementing again sets the value to 1 and so on with each incrementation.
- Please note down if the N and/or V flags are set in the APSR register. And explain why.
	- With the N and V flags in the APSR register, both conditional flags remain in the 0 bit, and do not change with any sub-sequential increment, after that. Because it unsigned and will always be positive.
	
###  5. Move the “counter’ variable outside of main (at the top of the file):
- What is the scope of the variable “counter”? **It is now a global variable as it is defined out of the function**.
- Is it still visible in the “Locals” view? **No, it not visible in the Local view**.
- In which window view can we track “counter” now? **The watch window is where you can track the “counter” variable, along with viewing its address, value and type**. 
- What is the address of the “counter” variable in memory? **It is at the beginning of the ram at 0x20000000**.

### 6 -Source code change-
- What is the value of “counter” at the end of the program (halting at the return 0 statement)? **The value of counter at the end is 4.**
- Explain why the counter value has changed? **Counter was initialized as global variable with 0. In the main method, p_int (pointer), was pointing to the global variable (counter) in memory. So, any increment made to the pointer p_int, transition/updated back to counter. As such there was 3 increment using the pointer variable and one increment onto the counter itself, resulting in counter value being 4.**

### 7 -Evaluation Board-
- What is the address where “counter” is stored? **The Address of counter is at 0x20000000** 
- Is the “counter” variable stored in RAM or ROM? **Counter seems to be in Ram**
- What is the value of “counter” at the end of the program (halting at the return 0
statement). **The counter value is 4**

