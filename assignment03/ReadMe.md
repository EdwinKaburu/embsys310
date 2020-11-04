
# Assignment 3
## Problem 1 and 2

 - So following the same steps with some changes to the GPIO port and output register bit. With led 1 we are dealing with GPIO port A  since the led1 is at PA5. The activation of the clock circuit (clock gating), is controlled by the same register (RCC_AHBxENR) as in led 2; only in this case Bit 0 needs to be activated for GPIOA (at Bit 0). The GPIO register mode is different as we need to assigned/set the two bits (11 -> 0 and 10 -> 1 ) for the general purpose output mode. The GPIO output data register  (GPIO_ODR) is different as we need to write 1 to bit 5 due to it being PA5 (as such it will be in hex (0x00000020) or (100000) in binary)
## Use of Xor
 - The bitwise (Xor) is used for the bit manipulation, as you want to manipulate only the bits required and leave the rest.
 - In our cause we using the bitwise to toggle between the on or off state of the led, as we need to write/store certain bits to have these changes came into effect. As to turn on or off the led1 we need to manipulate the data, at the location 0x4800 0014 (0x14 offset) which is the GPIO_ODR. Since we need bit 5 to set in order to turn on, we want to change it to 0x0000 0020; from its reset value of 0x0000 0000 and vice versa . Xor works well, for both states, since its result is 1 is the two bits are different .
 -  So in binary  0x0000 0020 (is 100000)
 
 
|Value |  | | | | | | | | 
|--|--|--|--|--|--| --|--|--|
| 0 |  | 0| 0|0 | 0 |0 |0 |
| 32| | 1 | 0 | 0 |0 | 0| 0 |
||
|Result Turn Led On| | 1| 0|0|0 |0|0
 - To turn off it will be the same process just in reverse.
## Problem 3

### - What is Morse Code?
According to Britannica, Morse code refers to "two systems for representing letters of the alphabet, numerals, and punctuation marks by an arrangement of dots, dashes, and spaces ", that are transmitted as electrical pulses.
### - Who developed Morse Code? 
 -  The credited inventors are Samuel Morse, Joseph Henry and Alfred Vail with the introduction of the electrical telegraph system in 1836.
### Problem 4
See main.c and video link below.
 - As for my first name in Morse : Edwin will be ( . -.. .-- .. -. ), this means the led will blink 11 times to represent all the letters, after which there is a long pause/space before doing the processes over and over again.
 
|Video Version|  |
|--|--|
| Morse Video Long | https://vimeo.com/475619976 |
| Morse Video Short| https://vimeo.com/475622138


 