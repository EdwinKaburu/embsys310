
## 2. Generate the map file for your program and provide details on:

 - a. How much total ROM your program is occupying? The program is
   currently using a grand total of 432 bytes of ROM with (ro code = 342
   and ro write = 90).
   
  - b. How much total RAM your program is using? The number of Ram being
   used in the program is 8196 bytes (8.196 kilobytes )
   
  - c. What part of your program is using the most ROM? Based on the map
   file, the delay.o is currently using 52 bytes (ro code) with
   startup_stm32l475xx at 2 bytes(ro code) making it the least. The
   main.o is the most, as it using 84 bytes(ro code) with 4 bytes (ro
   data) (88 bytes of ROM)  and 4 bytes (rw data)
   
   - d. What part of your program is using the most RAM? Well both delay,
   main and startup_stm32l475xx are minimal in comparison to the linker,
   that accounts for the most ram creation of about 8212 bytes.

## 3. Bonus: Is there anything that can be done to optimize the usage of ROM or RAM resources? Explain any options.

 - In the startup_stm32l475xx.c, if we use the #pragma 'weak' aliasing
   for the handlers, we would be reducing the amount of ROM consumed 
   from (ro code = 42 bytes  ro data = 64 bytes ) without [#pragma
   'weak']; **to** current implementation of (ro code = 2 bytes  ro data = 64
   bytes) with  [#pragma 'weak'].
   
  - In the main.c, for the ms delay value/variable, if we make it const
   we would be consuming around (ro code = 84 bytes, ro data = 4 bytes);
   without making it const (current implementation) it would consume (ro
   code = 84 bytes, ro data = 4 bytes, rw data = 4 bytes), not much rw
  data to RAM

see Map/cmsis_led.map file
