################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/BmsScheduler/src/BmsScheduler.c 

OBJS += \
./Application/BmsScheduler/src/BmsScheduler.o 

C_DEPS += \
./Application/BmsScheduler/src/BmsScheduler.d 


# Each subdirectory must supply rules for building sources it contributes
Application/BmsScheduler/src/%.o: ../Application/BmsScheduler/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Application/BmsScheduler/src/BmsScheduler.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


