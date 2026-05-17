################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CDD/CDD_INA219/src/CDD_INA219.c 

OBJS += \
./CDD/CDD_INA219/src/CDD_INA219.o 

C_DEPS += \
./CDD/CDD_INA219/src/CDD_INA219.d 


# Each subdirectory must supply rules for building sources it contributes
CDD/CDD_INA219/src/%.o: ../CDD/CDD_INA219/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@CDD/CDD_INA219/src/CDD_INA219.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


