################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/mb_0.c \
../src/pmbus.c \
../src/sysmon.c \
../src/util.c 

OBJS += \
./src/mb_0.o \
./src/pmbus.o \
./src/sysmon.o \
./src/util.o 

C_DEPS += \
./src/mb_0.d \
./src/pmbus.d \
./src/sysmon.d \
./src/util.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MicroBlaze gcc compiler'
	mb-gcc -Wall -O0 -g3 -c -fmessage-length=0 -I../../standalone_bsp_0/mb_0/include -mxl-pattern-compare -mno-xl-soft-div -mcpu=v8.00.a -mno-xl-soft-mul -mhard-float -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


