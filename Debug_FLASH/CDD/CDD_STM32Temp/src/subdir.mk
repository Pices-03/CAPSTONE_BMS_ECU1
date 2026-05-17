################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CDD/CDD_STM32Temp/src/CDD_STM32Temp.c 

OBJS += \
./CDD/CDD_STM32Temp/src/CDD_STM32Temp.o 

C_DEPS += \
./CDD/CDD_STM32Temp/src/CDD_STM32Temp.d 


# Each subdirectory must supply rules for building sources it contributes
CDD/CDD_STM32Temp/src/%.o: ../CDD/CDD_STM32Temp/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@CDD/CDD_STM32Temp/src/CDD_STM32Temp.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


