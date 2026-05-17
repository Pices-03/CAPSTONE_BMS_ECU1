################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/BmsSoc/src/BmsSoc.c 

OBJS += \
./Application/BmsSoc/src/BmsSoc.o 

C_DEPS += \
./Application/BmsSoc/src/BmsSoc.d 


# Each subdirectory must supply rules for building sources it contributes
Application/BmsSoc/src/%.o: ../Application/BmsSoc/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Application/BmsSoc/src/BmsSoc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


