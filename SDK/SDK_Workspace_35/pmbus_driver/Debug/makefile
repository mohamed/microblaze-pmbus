################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
ELFSIZE += \
pmbus_driver.elf.size \

ELFCHECK += \
pmbus_driver.elf.elfcheck \


# All Target
all: pmbus_driver.elf secondary-outputs

# Tool invocations
pmbus_driver.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MicroBlaze gcc linker'
	mb-gcc -Wl,-T -Wl,../src/lscript.ld -L../../standalone_bsp_0/mb_0/lib -mxl-pattern-compare -mno-xl-soft-div -mcpu=v8.00.a -mno-xl-soft-mul -mhard-float -o"pmbus_driver.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

pmbus_driver.elf.size: pmbus_driver.elf
	@echo 'Invoking: MicroBlaze Print Size'
	mb-size pmbus_driver.elf  |tee "pmbus_driver.elf.size"
	@echo 'Finished building: $@'
	@echo ' '

pmbus_driver.elf.elfcheck: pmbus_driver.elf
	@echo 'Invoking: Xilinx ELF Check'
	elfcheck pmbus_driver.elf -hw ../../hw_platform_0/system.xml -pe mb_0  |tee "pmbus_driver.elf.elfcheck"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES)$(ELFSIZE)$(ELFCHECK)$(S_UPPER_DEPS) pmbus_driver.elf
	-@echo ' '

secondary-outputs: $(ELFSIZE) $(ELFCHECK)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
