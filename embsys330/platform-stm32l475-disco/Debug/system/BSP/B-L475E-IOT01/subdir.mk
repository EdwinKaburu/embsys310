################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/BSP/B-L475E-IOT01/stm32l475e_iot01.c \
../system/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.c \
../system/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.c \
../system/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.c \
../system/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.c \
../system/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.c \
../system/BSP/B-L475E-IOT01/stm32l475e_iot01_qspi.c \
../system/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.c 

OBJS += \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01.o \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.o \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.o \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.o \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.o \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.o \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_qspi.o \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.o 

C_DEPS += \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01.d \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.d \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.d \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.d \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.d \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.d \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_qspi.d \
./system/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.d 


# Each subdirectory must supply rules for building sources it contributes
system/BSP/B-L475E-IOT01/%.o: ../system/BSP/B-L475E-IOT01/%.c system/BSP/B-L475E-IOT01/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -DSTM32L475xx -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l4xx" -I"../system/BSP/B-L475E-IOT01" -I"../system/BSP/Components" -I"../system/BSP/Components/vl53l0x" -I"../tinyml" -I"../framework/include" -I"../src/Wifi" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


