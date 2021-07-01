################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include system/src/stm32l4xx/subdir.mk
-include system/src/newlib/subdir.mk
-include system/src/diag/subdir.mk
-include system/src/cortexm/subdir.mk
-include system/src/cmsis/subdir.mk
-include system/BSP/Components/m24sr/subdir.mk
-include system/BSP/Components/lsm6dsl/subdir.mk
-include system/BSP/Components/lps22hb/subdir.mk
-include system/BSP/Components/lis3mdl/subdir.mk
-include system/BSP/Components/hts221/subdir.mk
-include system/BSP/Components/es_wifi/subdir.mk
-include system/BSP/B-L475E-IOT01/subdir.mk
-include src/Wifi/NotCheckedIn/subdir.mk
-include src/Wifi/subdir.mk
-include src/UartAct/UartOut/subdir.mk
-include src/UartAct/UartIn/subdir.mk
-include src/UartAct/subdir.mk
-include src/Traffic/Lamp/subdir.mk
-include src/Traffic/subdir.mk
-include src/Template/SimpleReg/subdir.mk
-include src/Template/SimpleAct/subdir.mk
-include src/Template/CompositeAct/CompositeReg/subdir.mk
-include src/Template/CompositeAct/subdir.mk
-include src/System/subdir.mk
-include src/Sensor/SensorPress/subdir.mk
-include src/Sensor/SensorMag/subdir.mk
-include src/Sensor/SensorHumidTemp/subdir.mk
-include src/Sensor/SensorAccelGyro/subdir.mk
-include src/Sensor/subdir.mk
-include src/Passive/subdir.mk
-include src/Node/NodeParser/subdir.mk
-include src/Node/subdir.mk
-include src/LevelMeter/subdir.mk
-include src/GpioOutAct/GpioOut/subdir.mk
-include src/GpioOutAct/subdir.mk
-include src/GpioInAct/GpioIn/subdir.mk
-include src/GpioInAct/subdir.mk
-include src/FuelPump/subdir.mk
-include src/Disp/Ili9341/subdir.mk
-include src/Disp/Adafruit/subdir.mk
-include src/Disp/subdir.mk
-include src/Demo/subdir.mk
-include src/Console/CmdParser/subdir.mk
-include src/Console/CmdInput/subdir.mk
-include src/Console/subdir.mk
-include src/AOWashingMachine/subdir.mk
-include src/subdir.mk
-include qpcpp/src/qxk/subdir.mk
-include qpcpp/src/qf/subdir.mk
-include qpcpp/ports/arm-cm/qxk/gnu/subdir.mk
-include qpcpp/include/subdir.mk
-include framework/source/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := platform-stm32l475-disco
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
platform-stm32l475-disco.hex \

SECONDARY_SIZE += \
platform-stm32l475-disco.siz \


# All Target
all: platform-stm32l475-disco.elf secondary-outputs

# Tool invocations
platform-stm32l475-disco.elf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -T mem.ld -T libs.ld -T sections.ld -nostartfiles -Xlinker --gc-sections -L"../ldscripts" -Wl,-Map,"platform-stm32l475-disco.map" --specs=nano.specs -u _printf_float -o "platform-stm32l475-disco.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

platform-stm32l475-disco.hex: platform-stm32l475-disco.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "platform-stm32l475-disco.elf"  "platform-stm32l475-disco.hex"
	@echo 'Finished building: $@'
	@echo ' '

platform-stm32l475-disco.siz: platform-stm32l475-disco.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "platform-stm32l475-disco.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS)$(CPP_DEPS) platform-stm32l475-disco.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
