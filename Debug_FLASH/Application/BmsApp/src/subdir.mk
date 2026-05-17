################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/BmsApp/src/BmsApp.c 

OBJS += \
./Application/BmsApp/src/BmsApp.o 

C_DEPS += \
./Application/BmsApp/src/BmsApp.d 


# Each subdirectory must supply rules for building sources it contributes
Application/BmsApp/src/%.o: ../Application/BmsApp/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Application/BmsApp/src/BmsApp.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


