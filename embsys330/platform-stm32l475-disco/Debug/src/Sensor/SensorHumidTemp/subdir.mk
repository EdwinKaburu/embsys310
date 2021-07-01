################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Sensor/SensorHumidTemp/SensorHumidTemp.cpp 

OBJS += \
./src/Sensor/SensorHumidTemp/SensorHumidTemp.o 

CPP_DEPS += \
./src/Sensor/SensorHumidTemp/SensorHumidTemp.d 


# Each subdirectory must supply rules for building sources it contributes
src/Sensor/SensorHumidTemp/%.o: ../src/Sensor/SensorHumidTemp/%.cpp src/Sensor/SensorHumidTemp/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -DSTM32L475xx -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l4xx" -I"../system/BSP/B-L475E-IOT01" -I"../system/BSP/Components" -I"../system/BSP/Components/es_wifi" -I../qpcpp/ports/arm-cm/qxk/gnu -I../qpcpp/include -I../qpcpp/src -I"../tinyml" -I"../tinyml/third_party" -I"../tinyml/third_party/flatbuffers/include" -I"../tinyml/third_party/gemmlowp" -I"../tinyml/third_party/ruy" -I"../framework/include" -I"../src/Passive" -I"../src/Template/CompositeAct" -I"../src/Template/CompositeAct/CompositeReg" -I"../src/Template/SimpleAct" -I"../src/Console" -I"../src/Console/CmdInput" -I"../src/Console/CmdParser" -I"../src/Traffic" -I"../src/Traffic/Lamp" -I"../src/FuelPump" -I"../src/AOWashingMachine" -I"../src/System" -I"../src/UartAct" -I"../src/UartAct/UartIn" -I"../src/UartAct/UartOut" -I"../src/GpioInAct" -I"../src/GpioInAct/GpioIn" -I"../src/GpioOutAct" -I"../src/GpioOutAct/GpioOut" -I"../src/Disp" -I"../src/Disp/Ili9341" -I"../src/Disp/Adafruit" -I"../src/Disp/Adafruit/Fonts" -I"../src/Sensor" -I"../src/Sensor/SensorAccelGyro" -I"../src/Sensor/SensorHumidTemp" -I"../src/Sensor/SensorMag" -I"../src/Sensor/SensorPress" -I"../src/LevelMeter" -I"../src/Wifi" -I"../src/Node" -I"../src/Node/NodeParser" -std=gnu++14 -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


