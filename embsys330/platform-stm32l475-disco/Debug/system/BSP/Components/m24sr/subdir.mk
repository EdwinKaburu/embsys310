################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/BSP/Components/m24sr/m24sr.c 

OBJS += \
./system/BSP/Components/m24sr/m24sr.o 

C_DEPS += \
./system/BSP/Components/m24sr/m24sr.d 


# Each subdirectory must supply rules for building sources it contributes
system/BSP/Components/m24sr/%.o: ../system/BSP/Components/m24sr/%.c system/BSP/Components/m24sr/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -DSTM32L475xx -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l4xx" -I"../system/BSP/B-L475E-IOT01" -I"../system/BSP/Components" -I"../system/BSP/Components/vl53l0x" -I"../tinyml" -I"../framework/include" -I"../src/Wifi" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

