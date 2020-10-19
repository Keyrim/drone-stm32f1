################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include lib/middleware/subdir.mk
-include lib/hal/src/Legacy/subdir.mk
-include lib/hal/src/subdir.mk
-include lib/btm/subdir.mk
-include lib/bsp/tft_ili9341/subdir.mk
-include lib/bsp/MPU6050/subdir.mk
-include lib/bsp/subdir.mk
-include lib/subdir.mk
-include appli/telemetrie/subdir.mk
-include appli/tasks/subdir.mk
-include appli/scheduler/subdir.mk
-include appli/events/subdir.mk
-include appli/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: ground_station.elf

# Tool invocations
ground_station.elf: $(OBJS) $(USER_OBJS) ../lib/LinkerScript.ld
	@echo 'Building target: $@'
	@echo 'Invoking: MCU GCC Linker'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -u_printf_float -T"../lib/LinkerScript.ld" -Wl,-Map=output.map -Wl,--gc-sections -o "ground_station.elf" @"objects.list" $(USER_OBJS) $(LIBS) -lm
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	-@echo 'Generating binary and Printing size information:'
	arm-none-eabi-objcopy -O binary "ground_station.elf" "ground_station.bin"
	arm-none-eabi-size "ground_station.elf"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
