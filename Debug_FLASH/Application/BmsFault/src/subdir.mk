################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/BmsFault/src/BmsFault.c 

OBJS += \
./Application/BmsFault/src/BmsFault.o 

C_DEPS += \
./Application/BmsFault/src/BmsFault.d 


# Each subdirectory must supply rules for building sources it contributes
Application/BmsFault/src/%.o: ../Application/BmsFault/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Application/BmsFault/src/BmsFault.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


