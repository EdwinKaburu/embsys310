## 1. PWM
The video can be found at vimeo using the [following link](https://vimeo.com/490002744)  

- In the diagram below I had selected 800 minus 1, as the PSC (Prescaler), with a counter period of 2000ms (2 seconds). 
- I changed it to 400 minus 1 for the Prescaler in code with a period of 1000ms (1 second), for curiosity experimental purposes, both settings worked in dimming both leds (the video and source code contains this setting).PB-14 (led 2) is a TIM15 at channel 1, PA5 (led 1) is at TIM2 at channel 1, both utilized the PWM.

Video: https://vimeo.com/490002744
Main Location : https://github.com/EdwinKaburu/embsys310/blob/main/assignment08/SourceCode/Core/Src/main.c
